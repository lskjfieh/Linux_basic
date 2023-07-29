#ifndef __POLL_SVR_H__
#define __POLL_SVR_H__

#include <iostream>
#include <sys/select.h>
#include <sys/time.h>
#include <vector>
#include <string>
#include <poll.h>
#include "Log.hpp"
#include "Sock.hpp"

#define FD_NONE -1
using namespace std;
// select 只完成读取，写入和异常不做处理 -- epoll（写完整）
class PollServer
{
    public:
    static const int nfds = 100;
public:
    PollServer(const uint16_t &port = 8080) : _port(port), _nfds(nfds)
    {
        _listensock = Sock::Socket();
        Sock::Bind(_listensock, _port);
        Sock::Listen(_listensock);
        logMessage(DEBUG, "%s", "create base socket success");
        
        _fds = new struct pollfd[_nfds];
        for(int i = 0; i < _nfds; i++){
            _fds[i].fd = FD_NONE;
            _fds[i].events = _fds[i].revents = 0;       
        }
        _fds[0].fd = _listensock;
        _fds[0].events = POLLIN;
        _timeout = 1000;
    }
    void Start()
    {
        while (true)
        {
            int n = poll(_fds, _nfds, _timeout);
            switch (n)
            {
            case 0:
                logMessage(DEBUG, "%s", "time out...");
                break;
            case -1:
                logMessage(WARNING, "select error: %d : %s", errno, strerror(errno));
            default:
                // 成功的
                HandlerEvent();
                break;
            }
        }
    }
    ~PollServer()
    {
        if (_listensock >= 0)
            close(_listensock);
        if (_fds) delete [] _fds;
    }

private:
    void HandlerEvent()
    {
        for (int i = 0; i < _nfds; i++)
        {
            if (_fds[i].fd == FD_NONE)
                continue;
            if (_fds[i].revents & POLLIN)
            {
                // 指定的fd，读事件就绪
                // 读事件就绪：连接事件到来，accept
                if (_fds[i].fd == _listensock) Accepter();                
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
        for (; pos < _nfds; pos++){
            if (_fds[pos].fd == FD_NONE)
                break;
        }
        if (pos == _nfds){
            // 对struct pollfd进行自动扩容
            logMessage(WARNING, "%s:%d", "select server already full, close: %d", sock);
            close(sock);
        }else{
            _fds[pos].fd = sock;
            _fds[pos].events = POLLIN;
        }
    }
    void Recver(int pos){
            // 读事件就绪：INPUT事件到来，recv，read
            logMessage(DEBUG, "messsage in, get IO event: %d", _fds[pos]);
            // 先不考虑阻塞
            char buffer[1024];
            int n = recv(_fds[pos].fd, buffer, sizeof(buffer)-1, 0);
            if(n > 0){
                buffer[n] = 0;
                logMessage(DEBUG, "client[%d]# %s", _fds[pos].fd, buffer);
            }
            else if(n == 0){
                logMessage(DEBUG, "cilent[%d] quit, me too...", _fds[pos].fd);
                // 1. 不让select关心当前的fd了
                close(_fds[pos].fd);
                _fds[pos].fd = FD_NONE;
                _fds[pos].events = 0;
            }else{
                logMessage(WARNING, "%d sock recv error, %d : %s", _fds[pos].fd, errno, strerror(errno));
                // 1. 不让select关心当前的fd了
                close(_fds[pos].fd);
                _fds[pos].fd = FD_NONE;
                _fds[pos].events = 0;
            }
    }

    void DebugPrint()
    {
        cout << "_fd_array[]: ";
        for (int i = 0; i < _nfds; i++)
        {
            if (_fds[i].fd == FD_NONE)
                continue;
            cout << _fds[i].fd << " ";
        }
        cout << endl;
    }

private:
    uint16_t _port;
    int _listensock;
    struct pollfd *_fds;
    int _nfds;
    int _timeout;
};

#endif