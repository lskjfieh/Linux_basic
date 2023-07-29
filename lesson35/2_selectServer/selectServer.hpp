#ifndef __SELECT_SVR_H__
#define __SELECT_SVR_H__

#include <iostream>
#include <sys/select.h>
#include <sys/time.h>
#include <vector>
#include <string>
#include "Log.hpp"
#include "Sock.hpp"

#define BITS 8
#define NUM (sizeof(fd_set) * BITS)
#define FD_NONE -1
using namespace std;
// select 只完成读取，写入和异常不做处理 -- epoll（写完整）
class SelectServer
{
public:
    SelectServer(const uint16_t &port = 8080) : _port(port)
    {
        _listensock = Sock::Socket();
        Sock::Bind(_listensock, _port);
        Sock::Listen(_listensock);
        logMessage(DEBUG, "%s", "create base socket success");
        for (int i = 0; i < NUM; i++)
            _fd_array[i] = FD_NONE;
        // 规定_fd_array[0] = _listensock;
        _fd_array[0] = _listensock;
    }
    void Start()
    {
        while (true)
        {
            // struct timeval timeout = {0, 0};

            // int sock = Sock::Accept(_listensock); //不能直接调用accept
            // 加入select中，让select等
            // FD_SET(_listensock, &rfds);

            DebugPrint();
            fd_set rfds;
            FD_ZERO(&rfds); // 初始化 清空
            int maxfd = _listensock;
            for (int i = 0; i < NUM; i++)
            {
                if (_fd_array[i] == FD_NONE)
                    continue;
                FD_SET(_fd_array[i], &rfds);
                if (maxfd < _fd_array[i])
                    maxfd = _fd_array[i];
            }
            int n = select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);
            switch (n)
            {
            case 0:
                logMessage(DEBUG, "%s", "time out...");
                break;
            case -1:
                logMessage(WARNING, "select error: %d : %s", errno, strerror(errno));
            default:
                // 成功的
                logMessage(DEBUG, "get a new link event...");
                HandlerEvent(rfds);
                break;
            }
        }
    }
    ~SelectServer()
    {
        if (_listensock >= 0)
            close(_listensock);
    }

private:
    void HandlerEvent(const fd_set &rfds)
    {
        for (int i = 0; i < NUM; i++)
        {
            if (_fd_array[i] == FD_NONE)
                continue;
            if (FD_ISSET(_fd_array[i], &rfds))
            {
                // 指定的fd，读事件就绪
                // 读事件就绪：连接事件到来，accept
                if (_fd_array[i] == _listensock) Accepter();                
                else Recver(i);           
                
            }
        }
    }
    void Accepter()
    {
        string clientip;
        uint16_t clientport = 0;

        // listensock上面的读事件就绪了，表示可以读取了
        // 获取新连接
        int sock = Sock::Accept(_listensock, &clientip, &clientport);
        if (sock < 0)
        {
            logMessage(WARNING, "accept error");
            return;
        }
        logMessage(DEBUG, "get a new line success : [%s:%d] : %d", clientip.c_str(), clientport, sock);
        int pos = 1;
        for (; pos < NUM; pos++){
            if (_fd_array[pos] == FD_NONE)
                break;
        }
        if (pos == NUM){
            logMessage(WARNING, "%s:%d", "select server already full, close: %d", sock);
            close(sock);
        }else{
            _fd_array[pos] = sock;
        }
    }
    void Recver(int pos){
            // 读事件就绪：INPUT事件到来，recv，read
            logMessage(DEBUG, "messsage in, get IO event: %d", _fd_array[pos]);
            // 先不考虑阻塞
            char buffer[1024];
            int n = recv(_fd_array[pos], buffer, sizeof(buffer)-1, 0);
            if(n > 0){
                buffer[n] = 0;
                logMessage(DEBUG, "client[%d]# %s", _fd_array[pos], buffer);
            }
            else if(n == 0){
                logMessage(DEBUG, "cilent[%d] quit, me too...", _fd_array[pos]);
                // 1. 不让select关心当前的fd了
                close(_fd_array[pos]);
                _fd_array[pos] = FD_NONE;
            }else{
                logMessage(WARNING, "%d sock recv error, %d : %s", _fd_array[pos], errno, strerror(errno));
                // 1. 不让select关心当前的fd了
                close(_fd_array[pos]);
                _fd_array[pos] = FD_NONE;
            }
    }

    void DebugPrint()
    {
        cout << "_fd_array[]: ";
        for (int i = 0; i < NUM; i++)
        {
            if (_fd_array[i] == FD_NONE)
                continue;
            cout << _fd_array[i] << " ";
        }
        cout << endl;
    }

private:
    uint16_t _port;
    int _listensock;
    int _fd_array[NUM];
};

#endif