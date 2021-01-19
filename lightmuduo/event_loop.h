#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__

#include <thread>
#include <vector>
#include <memory>
#include <boost/noncopyable.hpp>

using namespace std;

namespace lightmuduo {
    class Channel;
    class Poller;

    class EventLoop : boost::noncopyable {
    public:
        EventLoop();
        ~EventLoop();

        void loop();

        bool isInLoopThread() const;

        void assertInLoopThread();

        static EventLoop* getEventLoopOfCurrentThread();

        void updateChannel(Channel*);

        void quit();
    private:
        void abortNotInLoopThread();

        typedef std::vector<Channel*> ChannelList;

        bool looping_;
        bool quit_;
        const thread::id threadId_;
        shared_ptr<Poller> poller_;
        ChannelList activeChannels_;
    };
    
}



#endif
