#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory>
#include "thread.hpp"

uint16_t serverport = 0;
std::string serverip;

// int sock = -1;
static void usage(std::string proc){
    std::cout << "\nUsage: " << proc << "serverIp serverPort\n" << std::endl;
}

static void *udpSend(void *args){
    
    int sock = *(int*)((ThreadData*)args)->args_;    //线程套接字
    std::string name = ((ThreadData*)args)->name_;   //线程名

    std::string message;
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    server.sin_addr.s_addr = inet_addr(serverip.c_str());
    //不断进行发送
    while (true)
    {
        std::cerr << "请输入你的信息# "; //标准错误 2打印
        std::getline(std::cin, message);
        if(message == "quit")
            break;
            //当client首次发送消息给服务器的时候，OS会自动给client bind它的IP和PORT
            sendto(sock, message.c_str() , message.size(), 0, (struct sockaddr*)&server, sizeof(server));
    }
    return nullptr;
}
static void *udpRecv(void *args){
    int sock = *(int*)((ThreadData*)args)->args_;    //线程套接字
    std::string name = ((ThreadData*)args)->name_;   //线程名

    char buffer[1024];
    while(true){
        //初始化为0
        memset(buffer, 0, sizeof(buffer));
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);

        ssize_t s = recvfrom(sock, buffer, sizeof buffer, 0, (struct sockaddr *)&temp, &len);
        if(s > 0){
            buffer[s] = 0;
            std::cout << buffer << std::endl;
        }
    }

}

int main(int argc, char *argv[]){
    if(argc != 3){
        usage(argv[0]);
        exit(1);
    }
    //创建套接字
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0){
        std::cerr << "socket error" << std::endl;
        exit(2);
    }
    //初始化port和ip
    serverport = atoi(argv[2]);
    serverip = argv[1];
                                    /*线程编号，回调方式，传入套接字*/
    std::unique_ptr<Thread> sender(new Thread(1, udpSend, (void*)&sock));
    std::unique_ptr<Thread> recver(new Thread(2, udpRecv, (void*)&sock));
    // sender->name();
    sender->start();
    recver->start();

    sender->join();
    recver->join();

    close(sock);
    return 0;
}