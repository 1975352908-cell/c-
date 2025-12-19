
// #include<stdio.h>
// #define LOG(fmt,...) printf("[%s:%d]" fmt "\n" ,__FILE__,__LINE__,##__VA_ARGS__)
// int main()
// {
//     LOG("%s-%s","hello","world");
//     LOG("%d,%d",1,2);
//     return 0;
// }

#include"tcp_server.cpp"
#include"tcp_client.cpp"
#include<thread>
int main()
{
    TcpServer server(8081,"127.0.0.1");
    //server线程;
    std::thread server_thread([&]()
    {
        server.start();
    });
    TcpClient client(8081,"127.0.0.1");
    //client线程;
    std::thread client_thread([&](){
        client.start();
    });
    //main线程等待server_thread线程结束;
    client_thread.join();
    server_thread.join();
    return 0;
}