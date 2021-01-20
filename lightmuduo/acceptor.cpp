// 

#include <iostream>


#include "acceptor.h"
#include "event_loop.h"

using namespace std;

int createNonblockingOrDie(sa_family_t family) {
    int sockfd = ::socket(family, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, IPPROTO_TCP);
    if (sockfd < 0) {
        cout << __FILE__ << __LINE__ << ": create socket error" << endl;
    }
    return sockfd;
}

namespace lightmuduo {
    sockaddr* cast_addr(sockaddr_in *addr) {
        return static_cast<struct sockaddr*>(implicit_cast<void *>(addr));
    }

    int accept_connect(int sockfd, sockaddr_in *addr) {
        socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
        int connfd = ::accept4(sockfd, static_cast<struct sockaddr*>(implicit_cast<void *>(addr)), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
        if (connfd < 0)
        {
            int savedErrno = errno;
            cerr << "Socket::accept";
            switch (savedErrno)
            {
            case EAGAIN:
            case ECONNABORTED:
            case EINTR:
            case EPROTO: // ???
            case EPERM:
            case EMFILE: // per-process lmit of open file desctiptor ???
                // expected errors
                errno = savedErrno;
                break;
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
                // unexpected errors
                cerr << "unexpected error of ::accept " << endl;
                break;
            default:
                cerr << "unknown error of ::accept " << endl;
                break;
            }
        }
        return connfd;
    }
    
    Acceptor::Acceptor(EventLoop *loop, InetAddressV4 &listenAddr)
    : loop_(loop),
      acceptSocket_(createNonblockingOrDie(listenAddr.addr().sin_family)),
      acceptChannel_(loop, acceptSocket_.fd()),
      listenning_(false) {
        // acceptSocket_.setReuseAddr(true);
        acceptSocket_.bindAddress(listenAddr);

        acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
    }

    void Acceptor::listen() {
        loop_->assertInLoopThread();
        listenning_ = true;
        acceptSocket_.listen();
        acceptChannel_.enableReading();
    }

    void Acceptor::handleRead() {
        loop_->assertInLoopThread();
        InetAddressV4 peerAddr("127.0.0.1", 0);
        int connfd = acceptSocket_.accept(&peerAddr);
        if (connfd >= 0) {
            if (connectionCallback_) {
                connectionCallback_(connfd, peerAddr);
            }
        } else {
            if (::close(connfd) < 0) {
                cerr << "accptor close socket" << endl;
            }
        }
    }
}

