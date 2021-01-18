// atomic example

#include <iostream>
#include <atomic>
#include <thread>

using namespace std;

static atomic<int> atomicvalue(0);
static int value = 0;

constexpr int count = 10000;

void increase() {
    for (auto i = 0; i < count; i ++) {
        ++atomicvalue;
        ++value;
    }
}

void decrease() {
    for (auto i = 0; i < count; i ++) {
        --atomicvalue;
        --value;
    }
}

int main() {
    thread incr(increase);
    thread decr(decrease);

    incr.join();
    decr.join();

    // when thread all done
    // atomic value will be zero
    // non-atomic value may not be zero
    cout << "atomic value:" << atomicvalue << endl;
    cout << "non-atomic value:" << value << endl;

    return 0;
}
