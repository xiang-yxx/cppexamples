// a block queue example
// powered by condition_variable

#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>

#include <boost/noncopyable.hpp>
#include <boost/circular_buffer.hpp>

using namespace std;


template<typename T>
class blockqueue : boost::noncopyable {
public:
    explicit blockqueue(size_t cap) : cb_(cap) {}
    void push_back(const T& e){
        unique_lock<mutex> lck(mtx_);
        while (cb_.full()) {
            not_full_.wait(lck);
        }
        cb_.push_back(e);
        not_empty_.notify_all();
    }

    T pop_front() {
        unique_lock<mutex> lck(mtx_);
        while (cb_.empty()) {
            not_empty_.wait(lck);
        }
        T t = cb_.front();
        cb_.pop_front();
        not_full_.notify_one();
        return t;
    }

    bool is_empty() const {
        unique_lock<mutex> lck(mtx_);
        return cb_.empty();
    }

    bool is_full() const {
        unique_lock<mutex> lck(mtx_);
        return cb_.full();
    }

private:
    mutable mutex mtx_;
    condition_variable not_empty_;
    condition_variable not_full_;
    boost::circular_buffer<T> cb_;
};

struct message {
    int val;
};

static blockqueue<message> mq(5);


void push_func() {
    int val = 0;
    while (true) {
        usleep(1000000);
        message msg = {val};
        cout << "push : " << msg.val << endl;
        ++val;
        mq.push_back(msg);
    }
}

void pop_func() {
    while (true) {        
        // usleep(1000000);
        message msg = mq.pop_front();
        cout << "pop : " << msg.val << endl;
    }
}


int main() {
    thread push_thread(push_func);
    thread pop_thread(pop_func);

    push_thread.join();
    pop_thread.join();


    return 0;
}
