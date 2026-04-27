#include "SysProgAPI.h"
#include "Session.h"
#include <vector>
#include <thread>

using namespace std;

vector<Session*> sessions;
int nextThreadId = 0;

HANDLE hStartEvent;
HANDLE hDataEvent;
HANDLE hStopEvent;
HANDLE hConfirmEvent;
HANDLE hExitEvent;

struct header
{
    int addr;
    int size;
};

// Обновленные объявления функций DLL
extern "C" void MapSendMessage(int addr, const wchar_t* str);
extern "C" void MapReceiveMessage(header* h, wchar_t* buffer, int bufferSize);

void WorkerThread(Session* session)
{
    SafeWrite(L"session", session->getID(), L"created");

    while (true)
    {
        Message msg;
        if (session->getMessage(msg))
        {
            switch (msg.header.messageType)
            {
            case MT_CLOSE:
                SafeWrite(L"session", session->getID(), L"closed");
                delete session;
                return;

            case MT_DATA:
                SafeWrite(L"session", session->getID(), L"received:", msg.data);
                session->saveToFile(msg.data);
                break;
            }
        }
    }
}

void Start()
{
    hStartEvent = CreateEventW(NULL, FALSE, FALSE, L"StartEvent");
    hDataEvent = CreateEventW(NULL, FALSE, FALSE, L"DataEvent");
    hStopEvent = CreateEventW(NULL, FALSE, FALSE, L"StopEvent");
    hConfirmEvent = CreateEventW(NULL, FALSE, FALSE, L"ConfirmEvent");
    hExitEvent = CreateEventW(NULL, FALSE, FALSE, L"ExitEvent");

    HANDLE events[4] = { hStartEvent, hDataEvent, hStopEvent, hExitEvent };

    SetEvent(hConfirmEvent);
    SafeWrite(L"Console app started");

    while (true)
    {
        int n = WaitForMultipleObjects(4, events, FALSE, INFINITE) - WAIT_OBJECT_0;

        switch (n)
        {
        case 0:
        {
            Session* newSession = new Session(nextThreadId++);
            sessions.push_back(newSession);
            thread t(WorkerThread, newSession);
            t.detach();
            SetEvent(hConfirmEvent);
            break;
        }

        case 1:
        {
            header h;
            wchar_t buffer[4096];
            MapReceiveMessage(&h, buffer, 4096);
            wstring text(buffer);

            if (h.addr == -2)
            {
                SafeWrite(L"Main thread, broadcast:", text);
                for (auto* s : sessions)
                    s->addMessage(MT_DATA, text);
            }
            else if (h.addr == -1)
            {
                SafeWrite(L"Main thread, message:", text);
            }
            else
            {
                if (h.addr >= 0 && h.addr < (int)sessions.size())
                    sessions[h.addr]->addMessage(MT_DATA, text);
            }
            SetEvent(hConfirmEvent);
            break;
        }

        case 2:
        {
            if (!sessions.empty())
            {
                sessions.back()->addMessage(MT_CLOSE);
                sessions.pop_back();
            }
            SetEvent(hConfirmEvent);
            break;
        }

        case 3:
        {
            for (auto* s : sessions)
                delete s;
            sessions.clear();
            SetEvent(hConfirmEvent);

            CloseHandle(hStartEvent);
            CloseHandle(hDataEvent);
            CloseHandle(hStopEvent);
            CloseHandle(hConfirmEvent);
            CloseHandle(hExitEvent);

            SafeWrite(L"Console app exiting");
            return;
        }
        }
    }
}

int main()
{
    setlocale(LC_ALL, "rus");
    Start();
    return 0;
}