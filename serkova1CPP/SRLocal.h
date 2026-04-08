#pragma once

#include "Message.h"
#include <vector>
#include <map>
#include <thread>

using namespace std;

class SRLocal : public Sender, public Receiver
{
public:
    int id;

    SRLocal(int id = -1)
        : id(id)
    {
    }

    static vector<thread> threads;
    static map<int, Session*> sessions;
    static mutex mx;

    virtual void send(Message& m) const override
    {
        if (id < 0)
            sessions[m.header.to]->addMessage(m);
        else
            sessions[id]->addMessage(m);
    }

    virtual void receive(Message& m) const override
    {
        if (!sessions[id]->getMessage(m))
            throw runtime_error("No messages");
    }

    static void worker(int sessionID)
    {
        auto session = new Session(sessionID);
        {
            lock_guard<mutex> lg(mx);
            sessions[sessionID] = session;
        }
        SafeWrite("session", session->sessionID, "created");

        while (true)
        {
            Message m = Message::receiveMessage(SRLocal(sessionID));
            switch (m.header.messageType)
            {
            case MT_CLOSE:
            {
                {
                    lock_guard<mutex> lg(mx);
                    sessions.erase(sessionID);
                }
                delete session;
                SafeWrite("session", sessionID, "closed");
                return;
            }
            case MT_DATA:
            {
                SafeWrite("session", sessionID, "data", m.data);
                Sleep(500 * sessionID);
                break;
            }
            }
        }
    }

    static void addThread(int sessionID)
    {
        threads.push_back(std::move(thread(worker, sessionID)));
    }

    static void waitThreads()
    {
        for (auto& t : threads)
        {
            t.join();
        }
    }
};

vector<thread> SRLocal::threads;
map<int, Session*> SRLocal::sessions;
mutex SRLocal::mx;