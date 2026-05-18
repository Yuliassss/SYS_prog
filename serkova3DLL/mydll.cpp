#include "pch.h"
#include "mydll.h"
#include <memory>
#include <cstdlib>

// ---- Реализация конкретной стратегии (TCP) ----
void TcpSender::send(int type, int num, int addr, const wchar_t* str)
{
    Header h;
    h.type = type;
    h.num  = num;
    h.addr = addr;
    h.size = str ? (int)(wcslen(str) * sizeof(wchar_t)) : 0;

    sendData(sock, &h);
    if (h.size > 0)
        sendData(sock, (char*)str, h.size);
}

std::wstring TcpReceiver::receive(Header& h)
{
    receiveData(sock, &h);
    std::wstring s;
    if (h.size > 0)
    {
        s.resize(h.size / sizeof(wchar_t));
        receiveData(sock, s.data(), h.size);
    }
    return s;
}

// ---- Клиентское состояние: одно подключение на процесс ----
static boost::asio::io_context g_io;
static std::unique_ptr<tcp::socket> g_sock;

static const char* GetServerHost()
{
    const char* h = std::getenv("SERKOVA_HOST");
    return (h && *h) ? h : "127.0.0.1";
}

static const char* GetServerPort()
{
    const char* p = std::getenv("SERKOVA_PORT");
    return (p && *p) ? p : "12345";
}

extern "C" SERKOVA3DLL_API int SendM_C(int type, int num, int addr, const wchar_t* str)
{
    try
    {
        if ((MessageType)type == INIT)
        {
            g_sock.reset(new tcp::socket(g_io));
            tcp::resolver r(g_io);
            boost::asio::connect(*g_sock, r.resolve(GetServerHost(), GetServerPort()));
        }
        if (!g_sock) return -1;

        TcpSender sender(*g_sock);
        sender.send(type, num, addr, str);

        Header conf{};
        receiveData(*g_sock, &conf);

        if ((MessageType)type == EXIT)
        {
            boost::system::error_code ec;
            g_sock->shutdown(tcp::socket::shutdown_both, ec);
            g_sock->close(ec);
            g_sock.reset();
        }

        return conf.type == CONFIRM ? conf.num : -1;
    }
    catch (std::exception&)
    {
        g_sock.reset();
        return -1;
    }
}
