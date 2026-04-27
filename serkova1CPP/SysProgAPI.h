#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <map>
#include <thread>
#include <condition_variable>

using namespace std;

static mutex consoleMutex;

template<typename T>
void PrintValue(const T& value)
{
    wcout << value;
}

void PrintValue(const wstring& value)
{
    wcout << value;
}

void PrintValue(const wchar_t* value)
{
    wcout << value;
}

void PrintValue(const string& value)
{
    cout << value;
}

void PrintValue(const char* value)
{
    cout << value;
}

void PrintValue(int value)
{
    wcout << value;
}

void DoWrite()
{
    wcout << endl;
}

template<typename T, typename... Args>
void DoWrite(const T& first, const Args&... args)
{
    PrintValue(first);
    wcout << L" ";
    DoWrite(args...);
}

template<typename... Args>
void SafeWrite(Args... args)
{
    lock_guard<mutex> lg(consoleMutex);
    DoWrite(args...);
}