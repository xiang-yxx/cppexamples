// a mult-thread safe singleton example
// powered by call_once -- pthread_once

#include <iostream>
#include <mutex>
#include <thread>
#include <boost/noncopyable.hpp>

using namespace std;

template<typename T>
class singleton : boost::noncopyable {
public:
    static T& getInstance() {
        call_once(once_, &singleton::init);
        return *value_;
    }
private:
    singleton(){}
    ~singleton(){}
    static void init() {
        value_ = new T();
    }
    static once_flag once_;
    static T* value_;
};

template<typename T>
once_flag singleton<T>::once_;

template<typename T>
T* singleton<T>::value_ = nullptr;

struct myclass {
    int value;
};


typedef singleton<myclass> mysingletonclass;

void func1() {
    myclass obj = mysingletonclass::getInstance();
    cout << &obj << endl;
}

void func2() {
    myclass obj = mysingletonclass::getInstance();
    cout << &obj << endl;
}

void func3() {
    myclass obj = mysingletonclass::getInstance();
    cout << &obj << endl;
}


int main() {
    thread t1(func1);
    thread t2(func2);
    thread t3(func3);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}



