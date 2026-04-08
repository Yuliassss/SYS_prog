#include "SysProgAPI.h"
#include "SRLocal.h"
#include <vector>
#include <utility>

using namespace std;

// Глобальные переменные для связи с C#
vector<pair<Session*, HANDLE>> sessions;
int nextThreadId = 0;

HANDLE hStartEvent;
HANDLE hStopEvent;
HANDLE hConfirmEvent;
HANDLE hExitEvent;

void start()
{
    hStartEvent = CreateEvent(NULL, FALSE, FALSE, L"StartEvent");
    hStopEvent = CreateEvent(NULL, FALSE, FALSE, L"StopEvent");
    hConfirmEvent = CreateEvent(NULL, FALSE, FALSE, L"ConfirmEvent");
    hExitEvent = CreateEvent(NULL, FALSE, FALSE, L"ExitEvent");

    HANDLE hControlEvents[3] = { hStartEvent, hStopEvent, hExitEvent };

    SetEvent(hConfirmEvent);
    SafeWrite("Console app started");

    do
    {
        int n = WaitForMultipleObjects(3, hControlEvents, FALSE, INFINITE) - WAIT_OBJECT_0;

        switch (n)
        {
        case 0:
        {
            int newId = nextThreadId++;

             SRLocal::addThread(newId);

           
            SafeWrite("Thread created, ID:", newId, "total:", (int)SRLocal::sessions.size());
            SetEvent(hConfirmEvent);
            break;
        }

        case 1:
        {
            if (!SRLocal::sessions.empty())
            {
                
                int lastId = SRLocal::sessions.rbegin()->first;

               Message::sendMessage(SRLocal(), lastId, MT_CLOSE);

                SafeWrite("Thread terminated, ID:", lastId, "remaining:", (int)SRLocal::sessions.size() - 1);
            }
            SetEvent(hConfirmEvent);
            break;
        }

        case 2:
        {
            
            for (auto& pair : SRLocal::sessions)
            {
                Message::sendMessage(SRLocal(), pair.first, MT_CLOSE);
            }

            SRLocal::waitThreads();

            SetEvent(hConfirmEvent);
            SafeWrite("Console app exiting");

            CloseHandle(hStartEvent);
            CloseHandle(hStopEvent);
            CloseHandle(hConfirmEvent);
            CloseHandle(hExitEvent);

            return;
        }
        }
    } while (true);
}

int main()
{
    start();
    return 0;
}