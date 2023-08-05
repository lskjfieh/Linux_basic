#ifndef _EPOLL_SERVER_HPP_
#define _EPOLL_SERVER_HPP_
#include <iostream>
#include <string>
#include <assert.h>
#include <functional>
#include "Sock.hpp"
#include "Log.hpp"
#include "Epoll.hpp"
namespace ns_epoll
{
    const static int default_port = 8080;
    const static int gnum = 64;

    // 只处理读取
    class EpollServer
    {
        using func_t = std::function<void(std::string)>;

    public:
        EpollServer(func_t HandlerRequest, const int &port = default_port)
            : _port(port), _revs_num(gnum), _HandlerRequest(HandlerRequest)
        {
            // 0. 申请对应空间
            _revs = new struct epoll_event[_revs_num];
            // 1. 创建listensock
            _listensock = Sock::Socket();
            Sock::Bind(_listensock, _port);
            Sock::Listen(_listensock);

            // 2. 创建epoll模型
            _epfd = Epoll::CreateEpoll();
            logMessage(DEBUG, "init success, listensock: %d, epfd:%d", _listensock, _epfd); // 3, 4

            // 3. 将listensock，先添加到epoll中，让epoll帮我们管理起来
            if (!Epoll::CtlEpoll(_epfd, EPOLL_CTL_ADD, _listensock, EPOLLIN))
                exit(6);
            logMessage(DEBUG, "add listensock to epoll success.");
        }
        void Accepter(int listensock)
        {
            std::string clientip;
            uint16_t clientport;
            int sock = Sock::Accept(listensock, &clientip, &clientport);
            if (sock < 0)
            {
                logMessage(WARNING, "accept error!");
                return;
            }
            // accept成功
            // 不能直接读取，因为不清楚底层是否有数据
            if (!Epoll::CtlEpoll(_epfd, EPOLL_CTL_ADD, sock, EPOLLIN))
                return;
            logMessage(DEBUG, "add new sock: %d to epoll success", sock);
        }
        void Recver(int sock)
        {
            // 1. 读取数据
            char buffer[10240];
            ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (n > 0)
            {
                // 假设读到一个完整的报文
                buffer[n] = 0;
                _HandlerRequest(buffer); // 回调 跳转到上层中处理
            }
            else if (n == 0)
            {
                // 1. 先在epoll中去掉对sock的关心
                bool res = Epoll::CtlEpoll(_epfd, EPOLL_CTL_DEL, sock, 0);
                assert(res);
                (void)res;
                // 2. 再close文件
                close(sock);
                logMessage(NORMAL, "client %d quit, me too...", sock);
            }
            else
            {
                // 1. 先在epoll中去掉对sock的关心
                bool res = Epoll::CtlEpoll(_epfd, EPOLL_CTL_DEL, sock, 0);
                assert(res);
                (void)res;
                // 2. 再close文件
                close(sock);
                logMessage(NORMAL, "client %d error, close error sock...", sock);
            }

            // 2. 处理数据
        }
        void HandlerEvents(int n)
        {
            assert(n > 0);
            for (int i = 0; i < n; i++)
            {
                uint32_t revents = _revs[i].events;
                int sock = _revs[i].data.fd;
                // 读事件就绪
                if (revents & EPOLLIN)
                {
                    // 1. listensock 就绪
                    if (sock == _listensock)
                        Accepter(_listensock);
                    // 2. 一般sock就绪 - read
                    else
                        Recver(sock);
                }
                // 写
                if(revents & EPOLLOUT){
                    //TODO
                }

            }
        }
        void LoopOnce(int timeout)
        {
            int n = Epoll::WaitEpoll(_epfd, _revs, _revs_num, timeout);
            // if(n == _revs_num) //可以在此进行扩容
            switch (n)
            {
            case 0:
                logMessage(DEBUG, "timeout...");
                break;
            case -1:
                logMessage(WARNING, "epoll wait error: %s", strerror(errno));
                break;
            default:
                // 等待成功
                logMessage(DEBUG, "get a event");
                HandlerEvents(n);
                break;
            }
        }
        void Start()
        {
            int timeout = -1;
            while (true)
            {
                LoopOnce(timeout);
            }
        }
        ~EpollServer()
        {
            if (_listensock >= 0)
                close(_listensock);
            if (_epfd >= 0)
                close(_epfd);
            if (_revs)
                delete[] _revs;
        }

    private:
        int _listensock;
        int _epfd;
        uint16_t _port;
        struct epoll_event *_revs; // 已经就绪的元素
        int _revs_num;             // 数组容量
        func_t _HandlerRequest;
    };
}

#endif