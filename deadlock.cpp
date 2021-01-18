// 一个多线程争夺资源导致死锁的简单例子

#include <iostream>
#include <mutex>
#include <thread>
#include <unistd.h>


using namespace std;

class Manager;

class Resource {
public:
    void doSomething(const Manager *manager) const __attribute_noinline__;
    void print() const;
private:
    mutable mutex mutex_;
};

class Manager {
public:
    Manager(Resource &resource) : resource_(resource) {}
    void doSomething() const {
        lock_guard<mutex> lck(mutex_);

        cout << "manager do some thing, lock manager mutex" << endl;
        usleep(1000);

        // do some thing
        resource_.print();
    }
private:
    Resource &resource_;
    mutable mutex mutex_;
};

void Resource::doSomething(const Manager *manager) const {
    lock_guard<mutex> lck(mutex_);
    
    cout << "resource do some thing, lock resource mutex" << endl;

    usleep(1000);

    manager->doSomething();
}

void Resource::print() const {
    lock_guard<mutex> lck(mutex_);
    
    cout << "resource print success" << endl;
}



int main() {
    cout << "dead lock example" << endl;

    Resource resource;
    Manager manager(resource);
    thread m(&Manager::doSomething, &manager);
    // thread r(&Resource::doSomething, &resource, &manager);
    resource.doSomething(&manager);

    m.join();
    // r.join();

    cout << "example failed" << endl;

    return 0;
}