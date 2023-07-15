#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>
#include <cerrno>
#include <cassert>
#include <unistd.h>
#include <signal.h>
#include <memory>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "ThreadPool/Task.hpp"
#include "ThreadPool/log.hpp"
#include "ThreadPool/threadPool.hpp"
#include <netinet/in.h>

// // 接收到客户端数据后进行的处理
static void service(int sock, const std::string &clientip,
                    const uint16_t &clientport, const std::string &thread_name)
{
    // echo server
    char buffer[1024];
    while (true)
    {
        // read && write 可以直接被使用
        ssize_t s = read(sock, buffer, sizeof(buffer) - 1);
        if (s > 0)
        {
            buffer[s] = 0; // 将对应的发过来的数据当作字符串
            std::cout << thread_name << "|" << clientip << ":" << clientport << "#" << buffer << std::endl;
        }
        else if (s == 0) // 读取到返回值为0，代表对端关闭连接
        {
            logMessage(NORMAL, "%s : %d shutdown, me too!", clientip.c_str(), clientport);
            break;
        }
        else // 异常
        {
            logMessage(FATAL, "read socket error%d:%s", errno, strerror(errno));
            break;
        }
        // 读取成功，继续向socket写入
        write(sock, buffer, strlen(buffer));
    }
    close(sock);
}

static void change(int sock, const std::string &clientip,
                   const uint16_t &clientport, const std::string &thread_name)
{
    // 大小写转换

    char buffer[1024];

    // read && write 可以直接被使用
    ssize_t s = read(sock, buffer, sizeof(buffer) - 1);
    if (s > 0)
    {
        buffer[s] = 0; // 将对应的发过来的数据当作字符串
        std::cout << thread_name << "|" << clientip << ":" << clientport << "#" << buffer << std::endl;
        std::string message;
        char *start = buffer;
        while (*start)
        {
            char c;
            if (islower(*start))
                c = toupper(*start);
            else
                c = *start;
            message.push_back(c);
            start++;
        }
        write(sock, message.c_str(), message.size());
    }
    else if (s == 0) // 读取到返回值为0，代表对端关闭连接
    {
        logMessage(NORMAL, "%s : %d shutdown, me too!", clientip.c_str(), clientport);
    }
    else // 异常
    {
        logMessage(FATAL, "read socket error%d:%s", errno, strerror(errno));
    }
    // 读取成功，继续向socket写入
    write(sock, buffer, strlen(buffer));
    close(sock);
}

static void dictOnline(int sock, const std::string &clientip,
                    const uint16_t &clientport, const std::string &thread_name)
{
    // echo server
    char buffer[1024];
    static std::unordered_map<std::string, std::string> dict = {
        {"apple", "苹果"},
        {"bite", "比特"},
        {"banana", "香蕉"},
        {"hard", "好难啊"}
    };

        // read && write 可以直接被使用
        ssize_t s = read(sock, buffer, sizeof(buffer) - 1);
        if (s > 0)
        {
            buffer[s] = 0; // 将对应的发过来的数据当作字符串
            std::cout << thread_name << "|" << clientip << ":" << clientport << "#" << buffer << std::endl;
            std::string message;
            auto iter = dict.find(buffer);
            if(iter == dict.end()) message = "我不知道...";
            else message = iter->second;
            write(sock, message.c_str(), message.size());
        
        }
        else if (s == 0) // 读取到返回值为0，代表对端关闭连接
        {
            logMessage(NORMAL, "%s : %d shutdown, me too!", clientip.c_str(), clientport);
        }
        else // 异常
        {
            logMessage(FATAL, "read socket error%d:%s", errno, strerror(errno));
        }
        // 读取成功，继续向socket写入
    close(sock);
}
// class ThreadData{
// public:
//     int _sock;
//     std::string _ip;
//     uint16_t _port;

// };

class TcpServer
{
private:
    const static int gbacklog = 20; // 一般不能太大也不能太小
    // static void *threadRoutine(void *args){
    //     //线程分离
    //     pthread_detach(pthread_self());
    //     ThreadData *td = static_cast<ThreadData *>(args);
    //     service(td->_sock, td->_ip, td->_port);
    //     delete td;
    // }

public:
    TcpServer(uint16_t port, std::string ip = "0.0.0.0")
        : _listensock(-1), _port(port), _ip(ip), _threadpool_ptr(ThreadPool<Task>::getThreadPool())
    {
    }
    void initServer()
    {
        // 1. 创建套接字 -- 进程和文件（以进程方式打开了一个文件）
        _listensock = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensock < 0)
        {
            logMessage(FATAL, "create socket error%d:%s", errno, strerror(errno));
            exit(2);
        }
        // 打印出创建的套接字，此处为流式
        logMessage(NORMAL, "create socket success, listensock: %d", _listensock);

        // 2. bind -- 文件 + 网络
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        inet_pton(AF_INET, _ip.c_str(), &local.sin_addr);
        // local.sin_addr.s_addr = _ip.empty() ? INADDR_ANY : inet_addr(_ip.c_str());

        if (bind(_listensock, (struct sockaddr *)&local, sizeof(local)) < 0)
        {
            logMessage(FATAL, "bind error, %d:%s", errno, strerror(errno));
            exit(3);
        }
        // 3. 因为tcp是面向连接的，当正式通信的时候，需要先建立连接
        if (listen(_listensock, gbacklog) < 0)
        {
            logMessage(FATAL, "listen error, %d:%s", errno, strerror(errno));
            exit(4);
        }
        logMessage(NORMAL, "init server success!");
    }
    void Start()
    {
        // signal(SIGCHLD, SIG_IGN); //主动忽略SIGCHLD信号，子进程退出的时候，自动退出僵尸状态
        // SIGCHLD：子进程终止或停止时，父进程收到的信号
        _threadpool_ptr->run(); // push任务
        while (true)
        {
            // sleep(1);
            // 4. 获取链接
            struct sockaddr_in src;
            socklen_t len = sizeof(src);
            //(servicesock)通过每一次获得的新连接进行IO vs (_sock)j监听socket：获取新连接
            int servicesock = accept(_listensock, (struct sockaddr *)&src, &len);

            if (servicesock < 0)
            {
                // 获取连接失败
                logMessage(ERROR, "accept error, %d:%s", errno, strerror(errno));
                continue;
            }
            // 获取连接成功
            uint16_t client_port = ntohs(src.sin_port);
            std::string client_ip = inet_ntoa(src.sin_addr);
            logMessage(NORMAL, "link success, servicesock: %d | %s : %d | \n",
                       servicesock, client_ip.c_str(), client_port);
            // 开始进行通信服务

            // version4 --线程池版本
            Task t(servicesock, client_ip, client_port, dictOnline);
            _threadpool_ptr->pushTask(t);

            // version3 --多线程版本
            // ThreadData *td = new ThreadData();
            // td->_sock = servicesock;
            // td->_ip = client_ip;
            // td->_port = client_port;
            // pthread_t tid;
            // pthread_create(&tid, nullptr, threadRoutine, td);

            // version2.1 --多进程版，不使用signal
            // pid_t id = fork();
            // if(id == 0){
            //     //child
            //     close(_listensock);
            //     if(fork() > 0) exit(0); //子进程本身立即退出
            //     //孙子进程被os接管
            //     service(servicesock, client_ip, client_port);
            //     exit(0);
            // }
            // //parent
            // waitpid(id, nullptr, 0); //因为上面的子进程直接退出，所以不会造成阻塞
            // close(servicesock);

            // version 1 -- 单进程循环版 只能够进行一次处理一个客户端，处理完了一个，才能处理下一个
            // 很显然，是不能够直接被使用的
            // service(servicesock, c lient_ip, client_port);

            // version 2 -- 多进程版 -- 创建子进程
            // 让子进程给新的连接提供服务，子进程能打开父进程曾经打开的文件fd
            //  pid_t id = fork();
            //  assert(id != -1);
            //  if(id == 0){
            //      //子进程
            //      //子进程会继承父进程打开的文件与文件fd，因此此时需要关闭不需要的套接字
            //      close(_listensock);
            //      service(servicesock, client_ip, client_port);
            //      exit(0);
            //  }

            // 父进程
            //  close(servicesock); //不关闭会造成文件描述符泄漏
        }
    }
    ~TcpServer() {}

private:
    uint16_t _port;
    std::string _ip;
    int _listensock;
    std::unique_ptr<ThreadPool<Task>> _threadpool_ptr;
};