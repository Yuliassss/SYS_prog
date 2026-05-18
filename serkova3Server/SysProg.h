#pragma once

#include <iostream>
#include <mutex>
#include <string>

inline void DoWrite() { std::wcout << std::endl; }

template <class T, typename... Args>
inline void DoWrite(const T& value, const Args&... args)
{
    std::wcout << value;
    DoWrite(args...);
}

inline std::mutex& OutMutex()
{
    static std::mutex m;
    return m;
}

template <typename... Args>
inline void SafeWrite(const Args&... args)
{
    std::lock_guard<std::mutex> lg(OutMutex());
    DoWrite(args...);
}
