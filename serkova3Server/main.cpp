// serkova3Server — TCP-сервер на boost::asio
//
// Архитектура:
//   - Главный поток слушает порт 12345, на каждое подключение запускает std::thread.
//   - Сессии (рабочие потоки) общие для всех клиентов. Защищены std::mutex.
//   - Транспортная стратегия — TcpSender/TcpReceiver из serkova3DLL (неявная загрузка).
//   - Каждый рабочий поток принимает сообщения через сессионную очередь Session (Strategy).

#include "mydll.h"      // boost::asio (winsock2) — должен идти первым
#include "session.h"

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

// Глобальный список активных сессий — общий для всех клиентов
static std::vector<Session*> g_sessions;
static std::mutex            g_sessMtx;
static std::atomic<int>      g_nextId{ 0 };

// ---- Рабочий поток ----
void Worker(Session* session)
{
    SafeWrite(L"[Session ", session->sessionID, L"] created");

    QueueReceiver qr;   // стратегия чтения из очереди
    while (true)
    {
        Message m;
        if (!qr.receive(session, m)) continue;

        switch (m.header.messageType)
        {
        case MT_CLOSE:
            SafeWrite(L"[Session ", session->sessionID, L"] closed");
            delete session;
            return;

        case MT_DATA:
            SafeWrite(L"[Session ", session->sessionID, L"] received: ", m.data);
            session->saveMessage(m);
            break;
        }
    }
}

// ---- Обработчик подключения клиента ----
void processClient(tcp::socket sock)
{
    try
    {
        TcpReceiver receiver(sock);
        TcpSender   sender(sock);

        while (true)
        {
            Header h;
            std::wstring str = receiver.receive(h);

            switch (h.type)
            {
            case INIT:
                SafeWrite(L"[Client] connected");
                break;

            case EXIT:
                SafeWrite(L"[Client] disconnected");
                sender.send(CONFIRM, 0, 0, nullptr);
                return;

            case START:
            {
                std::lock_guard<std::mutex> lg(g_sessMtx);
                for (int j = 0; j < h.num; ++j)
                {
                    int id = g_nextId++;
                    Session* s = new Session(id);
                    g_sessions.push_back(s);
                    std::thread(Worker, s).detach();
                }
                SafeWrite(L"[Server] threads now: ", (int)g_sessions.size());
                break;
            }

            case STOP:
            {
                std::lock_guard<std::mutex> lg(g_sessMtx);
                if (!g_sessions.empty())
                {
                    Session* last = g_sessions.back();
                    g_sessions.pop_back();
                    last->addMessage(MT_CLOSE);
                }
                SafeWrite(L"[Server] threads now: ", (int)g_sessions.size());
                break;
            }

            case SEND:
            {
                QueueSender qs;
                std::lock_guard<std::mutex> lg(g_sessMtx);

                if (h.addr == -2)
                {
                    SafeWrite(L"[Main] broadcast: ", str);
                    for (Session* s : g_sessions)
                    {
                        Message m(MT_DATA, str);
                        qs.send(s, m);
                    }
                }
                else if (h.addr == -1)
                {
                    SafeWrite(L"[Main] received: ", str);
                }
                else if (h.addr >= 0 && h.addr < (int)g_sessions.size())
                {
                    SafeWrite(L"[Main] to thread ", h.addr, L": ", str);
                    Message m(MT_DATA, str);
                    qs.send(g_sessions[h.addr], m);
                }
                else
                {
                    SafeWrite(L"[Main] target ", h.addr, L" not found");
                }
                break;
            }

            default:
                break;
            }

            // Подтверждение — всегда возвращаем актуальное количество активных потоков
            int count;
            {
                std::lock_guard<std::mutex> lg(g_sessMtx);
                count = (int)g_sessions.size();
            }
            sender.send(CONFIRM, count, 0, nullptr);
        }
    }
    catch (std::exception& e)
    {
        SafeWrite(L"[Client] error: ", std::wstring(e.what(), e.what() + strlen(e.what())));
    }
}

int main()
{
    // Локаль для корректного вывода кириллицы в консоль (CP1251 cmd на Windows)
    SetConsoleOutputCP(1251);
    std::locale::global(std::locale(".1251"));
    std::wcout.imbue(std::locale());

    try
    {
        const int port = 12345;
        boost::asio::io_context io;
        tcp::acceptor acc(io, tcp::endpoint(tcp::v4(), port));
        SafeWrite(L"[Server] started on port ", port);

        while (true)
        {
            tcp::socket s = acc.accept();
            std::thread(processClient, std::move(s)).detach();
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "[Server] error: " << e.what() << std::endl;
    }
    return 0;
}
