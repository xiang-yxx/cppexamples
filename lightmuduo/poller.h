#ifndef __POLLER_H__
#define __POLLER_H__

#include <vector>
#include <unordered_map>

struct pollfd;  // poll.h

namespace lightmuduo{
    class Channel;
    class EventLoop;

    class Poller {
    public:
        typedef std::vector<Channel*> ChannelList;

        explicit Poller(EventLoop *loop_);
        ~Poller();

        void poll(int timeOutMs, ChannelList *activeChannels);

        void updateChannel(Channel* channel);

        void assertInLoopThread();
    private:
        void fillActiveChannels(int numEvent, ChannelList *activeChannels) const;

        typedef std::vector<pollfd> PollFdList;
        typedef std::unordered_map<int, Channel*> ChannelMap;

        EventLoop *eventLoop_;
        PollFdList pollfds_;
        ChannelMap channels_;
    };
}



#endif