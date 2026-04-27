#pragma once

#include "SysProgAPI.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <fstream>

using namespace std;

enum MessageTypes
{
    MT_CLOSE,
    MT_DATA,
};

struct MessageHeader
{
    int messageType;
    int size;
};

struct Message
{
    MessageHeader header = { 0 };
    wstring data;

    Message() = default;

    Message(MessageTypes messageType, const wstring& data = L"")
        : data(data)
    {
        header = { messageType, (int)data.length() };
    }
};

class Session
{
private:
    queue<Message> messages;
    mutex mtx;
    condition_variable cv;
    int sessionID;

public:
    Session(int id) : sessionID(id) {}

    void addMessage(Message& m)
    {
        lock_guard<mutex> lock(mtx);
        messages.push(m);
        cv.notify_one();
    }

    bool getMessage(Message& m)
    {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this] { return !messages.empty(); });

        if (!messages.empty())
        {
            m = messages.front();
            messages.pop();
            return true;
        }
        return false;
    }

    void addMessage(MessageTypes type, const wstring& data = L"")
    {
        Message m(type, data);
        addMessage(m);
    }

    void saveToFile(const wstring& text)
    {
        wstring filename = to_wstring(sessionID) + L".txt";

        HANDLE hFile = CreateFileW(filename.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hFile != INVALID_HANDLE_VALUE)
        {
            SetFilePointer(hFile, 0, NULL, FILE_END);
            DWORD written;
            WriteFile(hFile, text.c_str(), (DWORD)(text.size() * sizeof(wchar_t)), &written, NULL);
            WriteFile(hFile, L"\n", 2, &written, NULL);
            CloseHandle(hFile);
        }
    }

    int getID() const { return sessionID; }
};