#include <iostream>
#include <thread>

#include "acceptor.h"
#include "event_loop.h"


using namespace std;
using namespace lightmuduo;


void newConnection(int sockfd, const InetAddressV4 &peerAddr) {
    cout << "newConnection(): accept a new connection, addr:" << peerAddr.getHost()
        << " port:" << peerAddr.getPort() << endl;
    string ans = "hello\n";
    ::write(sockfd, ans.c_str(), sizeof(ans.c_str()));
    if (close(sockfd) < 0) {
        cerr << "socket::close" << endl;
    }
}

int main () {
    cout << "main pid:" << this_thread::get_id() << endl;

    // 启动后使用 nc 127.0.0.1 8888 连接进行测试
    InetAddressV4 listenAddr("127.0.0.1", 8888);
    EventLoop loop;

    Acceptor acceptor(&loop, listenAddr);
    acceptor.setNewConnectionCallback(newConnection);
    acceptor.listen();

    loop.loop();

    return 0;
}

