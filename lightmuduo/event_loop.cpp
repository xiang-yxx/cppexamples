#include <thread>
#include <iostream>
#include <cassert>

#include "event_loop.h"
#include "channel.h"
#include "poller.h"

using namespace std;


// event loop only loop at the thread where it was created
namespace lightmuduo {
    thread_local EventLoop* t_loopInThisThread = 0;
    int kPollTimeMs = 1000;

    EventLoop::EventLoop() 
    : looping_(false),
    quit_(true),
    threadId_(std::this_thread::get_id()),
    poller_(make_shared<Poller>(this))
    {
        if (t_loopInThisThread) {
            cout << "exist event loop in this thread: " << t_loopInThisThread->threadId_ << endl;
        } else {
            t_loopInThisThread = this;
        }
    }

    EventLoop::~EventLoop() {
        assert(!looping_);
        t_loopInThisThread = nullptr;
    }

    bool EventLoop::isInLoopThread() const {
        return threadId_ == std::this_thread::get_id();
    }

    void EventLoop::assertInLoopThread() {
        if (!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    void EventLoop::abortNotInLoopThread() {
        cout << "event loop not in loop thread，current thread:" << std::this_thread::get_id()
            << " loop at thread id:" << threadId_ << endl;
    }

    void EventLoop::loop() {
        assert(!looping_);
        assertInLoopThread();

        looping_ = true;
        quit_ = false;

        while (!quit_) {
            activeChannels_.clear();

            poller_->poll(kPollTimeMs, &activeChannels_);

            for (auto iter = activeChannels_.begin(); iter != activeChannels_.end(); ++iter) {
                (*iter)->handleEvent();
            }
        }

        looping_ = false;
    }

    void EventLoop::quit() {
        quit_ = true;
    }

    void EventLoop::updateChannel(Channel *channel_) {
        assert(channel_->ownerEventLoop() == this);
        assertInLoopThread();
        poller_->updateChannel(channel_);
    }

    EventLoop* EventLoop::getEventLoopOfCurrentThread() {
        return t_loopInThisThread;
    }

}

