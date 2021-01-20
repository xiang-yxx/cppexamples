#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include <functional>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>   // memset

#include <boost/noncopyable.hpp>

#include "channel.h"

using namespace std;

namespace lightmuduo {
    template<typename To, typename From>
    inline To implicit_cast(From const &f) {
        return f;
    }

    sockaddr* cast_addr(sockaddr_in *addr);

    class EventLoop;
    class Channel;

    int accept_connect(int sockfd, sockaddr_in *addr);

    class InetAddressV4{
    public:
        InetAddressV4(const std::string &ip, uint16_t port) {
            memset(&addr_, 0, sizeof addr_);
            addr_.sin_family = AF_INET;
            addr_.sin_port = htobe16(port);
            if (::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr) < 0) {
                std::cout << "InetAddrV4 error：" << ip << std::endl;
            }
        }

        void setAddr(const sockaddr_in& addr) {
            addr_ = addr;
        }

        string getHost() const {
            return string(inet_ntoa(addr_.sin_addr));
        }

        uint16_t getPort() const {
            return ntohs(addr_.sin_port);
        }

        sockaddr_in addr() const { return addr_; }

        sockaddr_in *addrPtr() { return &addr_; }
    private:
        sockaddr_in addr_;
    };

    
    class Socket : boost::noncopyable {
    public:
        explicit Socket(int fd) : fd_(fd) {}

        int fd() const { return fd_; }

        // void setReuseAddr(bool);
        void bindAddress(InetAddressV4 &addr) {
            int ret = ::bind(fd_, cast_addr((addr.addrPtr())), static_cast<socklen_t>(sizeof addr.addr()));
            if (ret < 0) {
                cerr << "socket bind error" << endl;
            }
        }

        void listen() {
            int ret = ::listen(fd_, SOMAXCONN);
            if (ret < 0) {
                cerr << "socket listen error" << endl;
            }
        }

        int accept(InetAddressV4 *peerAddr) {
            sockaddr_in addr;
            memset(&addr, 0, sizeof addr);
            int connfd = accept_connect(fd_, &addr);
            if (connfd >= 0) {
                peerAddr->setAddr(addr);
            }
            return connfd;
        }

    private:
        int fd_;
    };
    

    // accpetor
    class Acceptor : boost::noncopyable {
    public:
        typedef std::function<void(int sockfd, const InetAddressV4 &)> NewConnectionCallback;

        Acceptor(EventLoop *loop, InetAddressV4 &listenAddr);
        
        void setNewConnectionCallback(const NewConnectionCallback &cb) {
            connectionCallback_ = cb;
        }

        void listen();
    private:
      
        void handleRead();

        EventLoop *loop_;
        Socket    acceptSocket_;
        Channel   acceptChannel_;
        NewConnectionCallback connectionCallback_;
        bool      listenning_;
    };
}


#endif