#pragma once


#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>

class Epoll
{
public:
    static const int gsize = 256;
public:
    static int CreateEpoll()
    {
        int epfd = epoll_create(gsize);
        if(epfd > 0) return epfd;
        exit(5);
    }
    static bool CtlEpoll(int epfd, int oper, int sock, uint32_t events)
    {
        struct epoll_event ev;
        ev.events = events;
        ev.data.fd = sock;
        int n = epoll_ctl(epfd, oper, sock, &ev);
        return n == 0;
    }
    static int WaitEpoll(int epfd, struct epoll_event revs[], int num, int timeout)
    {
        // 细节1：如果底层就绪的sock非常多，revs承装不下，怎么办？？不影响！一次拿不完，就下一次再拿
        // 细节2：关于epoll_wait的返回值问题：有几个fd上的事件就绪，就返回几，epoll返回的时候，会将所有
        //       就绪的event按照顺序放入到revs数组中！一共有返回值个！
        return epoll_wait(epfd, revs, num, timeout);
    }
};