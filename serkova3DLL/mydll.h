#pragma once

#ifdef SERKOVA3DLL_EXPORTS
#define SERKOVA3DLL_API __declspec(dllexport)
#else
#define SERKOVA3DLL_API __declspec(dllimport)
#endif

#include "asio.h"

// Типы сообщений в заголовке
enum MessageType
{
    INIT    = 0,
    EXIT    = 1,
    START   = 2,
    SEND    = 3,
    STOP    = 4,
    CONFIRM = 5
};

// Заголовок сообщения, передаваемый по TCP
//   type — MessageType
//   num  — количество (для START) или счётчик потоков (в CONFIRM)
//   addr — адресат: -2 = все, -1 = главный, >= 0 = поток #addr
//   size — размер данных в байтах (без заголовка)
struct Header
{
    int type;
    int num;
    int addr;
    int size;
};

// ---- Паттерн «Стратегия» для транспорта ----

class TransportSender
{
public:
    virtual ~TransportSender() {}
    virtual void send(int type, int num, int addr, const wchar_t* str) = 0;
};

class TransportReceiver
{
public:
    virtual ~TransportReceiver() {}
    virtual std::wstring receive(Header& h) = 0;
};

// Конкретная реализация — TCP через boost::asio
class SERKOVA3DLL_API TcpSender : public TransportSender
{
    tcp::socket& sock;
public:
    explicit TcpSender(tcp::socket& s) : sock(s) {}
    void send(int type, int num, int addr, const wchar_t* str) override;
};

class SERKOVA3DLL_API TcpReceiver : public TransportReceiver
{
    tcp::socket& sock;
public:
    explicit TcpReceiver(tcp::socket& s) : sock(s) {}
    std::wstring receive(Header& h) override;
};

// ---- C-точки для C# (DllImport) ----
// Возвращает num из CONFIRM (количество активных потоков на сервере) или -1 при ошибке.
extern "C"
{
    SERKOVA3DLL_API int SendM_C(int type, int num, int addr, const wchar_t* str);
}
