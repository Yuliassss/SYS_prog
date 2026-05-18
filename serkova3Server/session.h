#pragma once

#include "SysProg.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <queue>
#include <condition_variable>
#include <fstream>
#include <filesystem>
#include <codecvt>
#include <locale>

// ---- Сессионные сообщения (внутри сервера) ----
enum MessageTypes
{
    MT_CLOSE,
    MT_DATA
};

struct MessageHeader
{
    int messageType;
    int size;
};

struct Message
{
    MessageHeader header = { 0 };
    std::wstring data;

    Message() = default;
    Message(MessageTypes messageType, const std::wstring& d = L"")
        : data(d)
    {
        header = { messageType, (int)d.length() };
    }
};

// ---- Стратегия очереди (Strategy для сессионных сообщений) ----
class Session;

class QSender
{
public:
    virtual ~QSender() {}
    virtual void send(Session* s, Message& m) = 0;
};

class QReceiver
{
public:
    virtual ~QReceiver() {}
    virtual bool receive(Session* s, Message& m) = 0;
};

class Session
{
    std::queue<Message> messages;
    std::mutex mtx;
    std::condition_variable cv;

public:
    int sessionID;

    explicit Session(int id) : sessionID(id) {}

    void addMessage(Message& m)
    {
        std::lock_guard<std::mutex> lg(mtx);
        messages.push(m);
        cv.notify_one();
    }

    void addMessage(MessageTypes t, const std::wstring& data = L"")
    {
        Message m(t, data);
        addMessage(m);
    }

    bool getMessage(Message& m)
    {
        std::unique_lock<std::mutex> ul(mtx);
        cv.wait(ul, [this] { return !messages.empty(); });
        m = messages.front();
        messages.pop();
        return true;
    }

    void saveMessage(const Message& m)
    {
        std::filesystem::create_directory(L"files");
        std::wstring path = L"files\\" + std::to_wstring(sessionID) + L".txt";

        // Пишем UTF-8 (через узкий поток), чтобы кириллица корректно отобразилась в любом редакторе
        std::ofstream file(path, std::ios::app | std::ios::binary);
        if (!file.is_open()) return;

        // конвертация wstring -> UTF-8
        int n = WideCharToMultiByte(CP_UTF8, 0, m.data.c_str(), (int)m.data.size(), nullptr, 0, nullptr, nullptr);
        std::string utf8(n, 0);
        WideCharToMultiByte(CP_UTF8, 0, m.data.c_str(), (int)m.data.size(), utf8.data(), n, nullptr, nullptr);

        file.write(utf8.data(), utf8.size());
        file.put('\n');
    }
};

// Стратегии — простая очередь на основе Session
class QueueSender : public QSender
{
public:
    void send(Session* s, Message& m) override { s->addMessage(m); }
};

class QueueReceiver : public QReceiver
{
public:
    bool receive(Session* s, Message& m) override { return s->getMessage(m); }
};
