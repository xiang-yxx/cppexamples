#include <iostream>
#include <sys/timerfd.h>
#include <string.h>

#include "event_loop.h"
#include "channel.h"

using namespace std;
using namespace lightmuduo;

EventLoop* g_loop;

void timeout() {
    cout << "time out" << endl;
    g_loop->quit();
}

int main() {
    EventLoop loop;
    g_loop = &loop;

    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel chan(&loop, timerfd);
    chan.setReadCallback(timeout);
    chan.enableReading();

    itimerspec howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, nullptr);

    loop.loop();

    ::close(timerfd);

    return 0;
}