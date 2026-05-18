#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// Утилиты передачи бинарных данных по TCP
template <class T>
inline void sendData(tcp::socket& s, T* data, std::size_t n = 0)
{
    if (!n) n = sizeof(T);
    boost::system::error_code ec;
    boost::asio::write(s, boost::asio::buffer(data, n), ec);
    if (ec) throw boost::system::system_error(ec);
}

template <class T>
inline void receiveData(tcp::socket& s, T* data, std::size_t n = 0)
{
    if (!n) n = sizeof(T);
    boost::system::error_code ec;
    boost::asio::read(s, boost::asio::buffer(data, n), ec);
    if (ec) throw boost::system::system_error(ec);
}
