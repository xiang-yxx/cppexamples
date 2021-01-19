#include <iostream>
#include <poll.h>
#include <cassert>

#include "poller.h"
#include "event_loop.h"
#include "channel.h"


using namespace std;

namespace lightmuduo {
    Poller::Poller(EventLoop *loop_) : eventLoop_(loop_) {}

    Poller::~Poller() {}

    void Poller::assertInLoopThread() {
        eventLoop_->assertInLoopThread();
    }

    void Poller::poll(int timeOutMs, ChannelList *activeChannels) {
        int numEvents = ::poll(&(*pollfds_.begin()), pollfds_.size(), timeOutMs);
        if (numEvents > 0) {
            cout << "POLL RES:" << numEvents << " events happened" << endl;
            fillActiveChannels(numEvents, activeChannels);
        } else if (numEvents == 0) {
            cout << "POLL RES: nothing" << endl;
        } else {
            cerr << "Poller::poll" << endl;
        }
    }

    void Poller::fillActiveChannels(int numEvent, ChannelList *activeChannels) const {
        for (PollFdList::const_iterator pfd = pollfds_.begin(); pfd != pollfds_.end() && numEvent > 0; ++pfd) {
            // events that actually occurred
            if (pfd->revents > 0) {
                --numEvent;
                ChannelMap::const_iterator ch = channels_.find(pfd->fd);
                assert(ch != channels_.end());
                Channel *channel = ch->second;
                assert(channel->fd() == pfd->fd);
                channel->setRevents(pfd->revents);

                activeChannels->push_back(channel);
            }
        }
    }

    void Poller::updateChannel(Channel *channel) {
        assertInLoopThread();
        if (channel->index() < 0) {
            // a new channel，add to pollfds_
            assert(channels_.find(channel->fd()) == channels_.end());
            pollfd pfd;
            pfd.fd = channel->fd();
            pfd.events = static_cast<short>(channel->events());
            pfd.revents = 0;
            pollfds_.push_back(pfd);
            int index = static_cast<int>(pollfds_.size()) - 1;
            channel->setIndex(index);
            channels_[channel->fd()] = channel;
        } else {
            assert(channels_.find(channel->fd()) != channels_.end());
            assert(channels_[channel->fd()] == channel);

            int index = channel->index();
            assert(0 <= index && index < static_cast<int>(pollfds_.size()));
            pollfd &pfd = pollfds_[index];
            assert(pfd.fd == channel->fd() || pfd.fd == -1);

            pfd.events = static_cast<short>(channel->events());
            pfd.revents = 0;
            
            if (channel->isNoneEvents()) {
                pfd.fd = -1;   // 不监听任何 event，忽略
            }
        }
    }
}