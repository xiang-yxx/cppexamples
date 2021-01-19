#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <functional>

#include <boost/noncopyable.hpp>

namespace lightmuduo{

    class EventLoop;

    class Channel : boost::noncopyable {
    public:
        typedef std::function<void()> EventCallback;

        Channel(EventLoop* loop, int fd);

        void handleEvent();

        void setReadCallback(const EventCallback &cb_) { readCallback_ = cb_; }
        void setWriteCallback(const EventCallback &cb_) { writeCallback_ = cb_; }
        void setErrorCallback(const EventCallback &cb_) { errorCallback_ = cb_; }

        void enableReading() { events_ |= kReadEvents; update(); }
        void disableReading() { events_ &= ~kReadEvents; update(); }
        void enableWriting() { events_ |= kWriteEvents; update(); }
        void disableWriting() { events_ &= ~kWriteEvents; update(); }
        void disableAll() { events_ = kNonEvents; update(); }
        bool isWriting() const { return events_ & kWriteEvents; }
        bool isReading() const { return events_ & kReadEvents; }

        int fd() const { return fd_; }
        int events() const { return events_; }
        void setRevents(int revt) { revents_ = revt; }
        bool isNoneEvents() const { return events_ == kNonEvents; }

        int index() const { return index_; }
        void setIndex(int idx_) { index_ = idx_; };
        EventLoop* ownerEventLoop() const { return loop_; }

    private:
        void update();

        int fd_;
        int events_;
        int revents_;
        int index_;
        
        EventLoop *loop_;

        EventCallback readCallback_;
        EventCallback writeCallback_;
        EventCallback errorCallback_;
        
        static const int kNonEvents;
        static const int kReadEvents;
        static const int kWriteEvents;
    };
}

#endif