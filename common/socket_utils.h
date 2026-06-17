#pragma once

#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

class TcpSocket
{
private:
    int sockfd;

public:

    TcpSocket()
        : sockfd(-1)
    {
    }

    explicit TcpSocket(int fd)
        : sockfd(fd)
    {
    }

    ~TcpSocket()
    {
        if (sockfd != -1)
            close(sockfd);
    }

    int getFd() const
    {
        return sockfd;
    }

    bool create()
    {
        sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
        return sockfd >= 0;
    }

    bool connectTo(
        const std::string& ip,
        int port)
    {
        sockaddr_in serverAddr{};

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);

        inet_pton(
            AF_INET,
            ip.c_str(),
            &serverAddr.sin_addr);

        return ::connect(
            sockfd,
            (sockaddr*)&serverAddr,
            sizeof(serverAddr)) == 0;
    }

    bool bindAndListen(int port)
    {
        sockaddr_in addr{};

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        int opt = 1;

        setsockopt(
            sockfd,
            SOL_SOCKET,
            SO_REUSEADDR,
            &opt,
            sizeof(opt));

        if (::bind(
                sockfd,
                (sockaddr*)&addr,
                sizeof(addr)) < 0)
            return false;

        return ::listen(sockfd, 10) == 0;
    }

    int acceptConnection()
    {
        sockaddr_in clientAddr{};
        socklen_t len = sizeof(clientAddr);

        return ::accept(
            sockfd,
            (sockaddr*)&clientAddr,
            &len);
    }

    bool sendAll(
        const void* buffer,
        size_t size)
    {
        const char* ptr =
            static_cast<const char*>(buffer);

        while (size > 0)
        {
            ssize_t sent =
                send(sockfd, ptr, size, 0);

            if (sent <= 0)
                return false;

            ptr += sent;
            size -= sent;
        }

        return true;
    }

    bool recvAll(
        void* buffer,
        size_t size)
    {
        char* ptr =
            static_cast<char*>(buffer);

        while (size > 0)
        {
            ssize_t received =
                recv(sockfd, ptr, size, 0);

            if (received <= 0)
                return false;

            ptr += received;
            size -= received;
        }

        return true;
    }
};