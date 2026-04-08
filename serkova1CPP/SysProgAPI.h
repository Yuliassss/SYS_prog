#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <map>
#include <thread>

using namespace std;

static CRITICAL_SECTION cs;
static bool initCS = true;

// Базовый вывод для одиночного значения
template<typename T>
void PrintValue(const T& value)
{
    cout << value;
}

// Специализация для wstring (широкие строки)
void PrintValue(const wstring& value)
{
    wcout << value;
}

// Специализация для const wchar_t*
void PrintValue(const wchar_t* value)
{
    wcout << value;
}

// Специализация для string
void PrintValue(const string& value)
{
    cout << value;
}

// Специализация для const char*
void PrintValue(const char* value)
{
    cout << value;
}

// Специализация для int
void PrintValue(int value)
{
    cout << value;
}

// Функция для вывода с пробелами
void DoWrite()
{
    cout << endl;
}

template<typename T, typename... Args>
void DoWrite(const T& first, const Args&... args)
{
    PrintValue(first);
    cout << " ";
    DoWrite(args...);
}

// Потокобезопасный вывод
template<typename... Args>
void SafeWrite(const Args&... args)
{
    if (initCS)
    {
        InitializeCriticalSection(&cs);
        initCS = false;
    }
    EnterCriticalSection(&cs);
    DoWrite(args...);
    LeaveCriticalSection(&cs);
}