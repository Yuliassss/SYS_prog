#pragma once

#include "SysProgAPI.h"
#include <queue>
#include <mutex>
#include <map>

using namespace std;

class Message;

class Sender
{
public:
    virtual void send(Message&) const = 0;
};

class Receiver
{
public:
    virtual void receive(Message&) const = 0;
};

enum MessageTypes
{
    MT_CLOSE,
    MT_DATA,
};

struct MessageHeader
{
    int messageType;
    int size;
    int to;
    int from;
};

struct Message
{
    MessageHeader header = { 0 };
    wstring data;

    Message() = default;

    Message(MessageTypes messageType, const wstring& data = L"")
        : data(data)
    {
        header = { messageType, int(data.length() * sizeof(wchar_t)) };
    }

    Message(int to, MessageTypes messageType, const wstring& data = L"")
        : data(data)
    {
        header = { messageType, int(data.length() * sizeof(wchar_t)), to };
    }

    void send(const Sender& sender)
    {
        sender.send(*this);
    }

    void receive(const Receiver& receiver)
    {
        receiver.receive(*this);
    }

    static void sendMessage(const Sender& sender, MessageTypes messageType, const wstring& data = L"")
    {
        Message m(messageType, data);
        m.send(sender);
    }

    static void sendMessage(const Sender& sender, int to, MessageTypes messageType, const wstring& data = L"")
    {
        Message m(to, messageType, data);
        m.send(sender);
    }

    static Message receiveMessage(const Receiver& receiver)
    {
        Message m;
        m.receive(receiver);
        return m;
    }
};

class Session
{
    queue<Message> messages;
    mutex mx;
    HANDLE hEvent;

public:
    int sessionID;

    Session(int sessionID)
        : sessionID(sessionID)
    {
        hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    }

    ~Session()
    {
        CloseHandle(hEvent);
    }

    void addMessage(Message& m)
    {
        lock_guard<mutex> lg(mx);
        messages.push(m);
        SetEvent(hEvent);
    }

    bool getMessage(Message& m)
    {
        bool res = false;
        WaitForSingleObject(hEvent, INFINITE);

        lock_guard<mutex> lg(mx);
        if (!messages.empty())
        {
            res = true;
            m = messages.front();
            messages.pop();
        }
        if (messages.empty())
        {
            ResetEvent(hEvent);
        }
        return res;
    }
};