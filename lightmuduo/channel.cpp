
#include <poll.h>
#include <iostream>

#include "event_loop.h"
#include "channel.h"

using namespace std;

namespace lightmuduo {
    const int Channel::kNonEvents = 0;
    const int Channel::kReadEvents = POLLIN | POLLPRI;
    const int Channel::kWriteEvents = POLLOUT;

    Channel::Channel(EventLoop *loop, int fd)
      : loop_(loop), 
        fd_(fd),
        events_(0),
        revents_(0),
        index_(-1) 
    {
    }

    void Channel::update() {
        loop_->updateChannel(this);
    }

    void Channel::handleEvent() {
        if (revents_ & POLLNVAL) {
            cerr << "channel handle event POLLNVAL" << endl;
        }

        if (revents_ & (POLLERR | POLLNVAL)) {
            if (errorCallback_) errorCallback_();
        }

        if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
            if (readCallback_) readCallback_();
        }

        if (revents_ & POLLOUT) {
            if (writeCallback_) writeCallback_();
        }
    }
}
