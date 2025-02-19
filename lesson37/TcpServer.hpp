#pragma once
#include <iostream>
#include <string>
#include <cerrno>
#include <functional>
#include <ctime>
#include <vector>
#include <time.h>
#include <cassert>
#include <unordered_map>
#include "Sock.hpp"
#include "Log.hpp"
#include "Epoll.hpp"
#include "Protocol.hpp"

class TcpServer;
class Connection;
using func_t = std::function<void(Connection *)>;
using callback_t = std::function<void(Connection *, std::string request)>;

// 为了能够正常工作，常规的sock必须是要有自己独立的接收缓冲区
class Connection
{

public:
    Connection(int sock = -1) : _sock(sock), _tsvr(nullptr)
    {
    }
    void SetCallBack(func_t recv_cb, func_t send_cb, func_t except_cb)
    {
        _recv_cb = recv_cb;
        _send_cb = send_cb;
        _except_cb = except_cb;
    }
    ~Connection()
    {
    }

public:
    // 负责进行IO的文件描述符
    int _sock;
    // 三个回调方法，表征的就是对_sock进行特定读写对应的方法
    func_t _recv_cb;   // 读回调
    func_t _send_cb;   // 写回调
    func_t _except_cb; // 异常回调
    // 接收缓冲区&&发送缓冲区
    std::string _inbuffer; // 暂时没有办法处理二进制流（视频/音频）, 文本是可以的
    std::string _outbuffer;
    // 设置对TcpServer的回值指针
    TcpServer *_tsvr;

    // 添加时间戳
    uint64_t _lasttimestamp; // time();
};

// 这个网络服务器不要和上层业务强耦合
class TcpServer
{
    const static int gport = 8080;
    const static int gnum = 128;

public:
    TcpServer(int port = gport) : _port(port), _revs_num(gnum)
    {
        // 1. 创建listensock
        _listensock = Sock::Socket();
        Sock::Bind(_listensock, _port);
        Sock::Listen(_listensock);

        // 2. 创建多路转接对象
        _poll.CreateEpoll();

        // 3. 添加listensock到服务器中
        AddConnection(_listensock, std::bind(&TcpServer::Accepter, this, std::placeholders::_1), nullptr, nullptr);

        // 4. 构建一个获取就绪事件的缓冲区
        _revs = new struct epoll_event[_revs_num];
    }

    // 专门针对任意sock进行添加TcpServer
    void AddConnection(int sock, func_t recv_cb, func_t send_cb, func_t except_cb)
    {
        Sock::SetNonBlock(sock); // 设置为非阻塞

        // 添加sock到epoll[]中
        // 除了_listensock,未来会存大量的socket,每一个sock都必须被封装成一个Connection
        // 当服务器中存在大量的Connection的时候,TcpServer就需要将所有的Connection进行管理
        // 1. 构建conn对象，封装sock
        Connection *conn = new Connection(sock);
        conn->SetCallBack(recv_cb, send_cb, except_cb);
        conn->_tsvr = this;
        // conn->_lasttimestamp = time();

        // 2. 添加sock[]到epoll中
        _poll.AddSockToEpoll(sock, EPOLLIN | EPOLLET); // 任何多路转接的服务器，一般默认只会打开对读取事件的打开，写入事件会按需进行打开

        // 3. 将对应的Connection*对象指针添加到connections映射表中
        _connections.insert(std::make_pair(sock, conn));
    }
    void Accepter(Connection *conn)
    {
        // logMessage(DEBUG, "Accepter been called");
        // 一定是listensock已经就绪了，此次读取不会阻塞
        while (true)
        {
            std::string clientip;
            uint16_t clientport;
            int accept_errno = 0;
            // sock一定是常规的IO sock
            int sock = Sock::Accept(conn->_sock, &clientip, &clientport, &accept_errno);
            if (sock < 0)
            {
                // 表示底层无连接
                if (accept_errno == EAGAIN || accept_errno == EWOULDBLOCK)
                    break;
                else if (accept_errno == EINTR)
                    continue; // 表示被信号中断IO，概率非常低
                else
                {
                    // accept失败
                    logMessage(WARNING, "accept error, %d : %s", accept_errno, strerror(accept_errno));
                    break;
                }
            }
            // 链接事件就绪
            // 将sock托管给TcpServer
            if (sock >= 0)
            {
                AddConnection(sock, std::bind(&TcpServer::Recver, this, std::placeholders::_1),
                              std::bind(&TcpServer::Sender, this, std::placeholders::_1),
                              std::bind(&TcpServer::EXcepter, this, std::placeholders::_1));
                logMessage(DEBUG, "accept client %s : %d success, add to epoll&&TcpServer success, sock: %d",
                           clientip.c_str(), clientport, sock);
            }
        }
    }
    // 使能读写
    void EnableReadWrite(Connection *conn, bool readable, bool writeable)
    {
        uint32_t events = (readable ? EPOLLIN : 0) | (writeable ? EPOLLOUT : 0);
        bool res = _poll.CtrlEpoll(conn->_sock, events);
        assert(res); // 更改成if
    }

    // v1 -> v2
    void Recver(Connection *conn)
    {
        // conn->_lasttimestamp = time(); //更新最近访问时间
        const int num = 1024;
        bool err = false;
        // logMessage(DEBUG, "Recver event exists, Recver() been called");
        // v1: 直接面向字节流，先进行常规读取
        while (true)
        {
            char buffer[num];
            ssize_t n = recv(conn->_sock, buffer, sizeof(buffer) - 1, 0);
            if (n < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break; // 表示读取完毕
                else if (errno == EINTR)
                    continue; // 读取被中断，重新开始读
                else
                {
                    // 读取异常
                    logMessage(ERROR, "recv error, %d : %s", errno, strerror(errno));
                    conn->_except_cb(conn);
                    err = true;
                    break;
                }
            }
            else if (n == 0)
            {
                // 客户端关闭连接
                logMessage(DEBUG, "client[%d] quit, server close %d", conn->_sock, conn->_sock);
                conn->_except_cb(conn);
                err = true;
                break;
            }
            else
            {
                // 读取成功
                buffer[n] = 0;
                conn->_inbuffer += buffer; // 保存到自己的缓冲区
            }
        } // end while
        logMessage(DEBUG, "conn->_inbuffer[sock: %d]: %s", conn->_sock, conn->_inbuffer.c_str());
        if (!err)
        {
            std::vector<std::string> message;
            SpliteMessage(conn->_inbuffer, &message);
            // 保证走到这里,就是一个完整的报文
            for (auto &msg : message)
                _cb(conn, msg); // 可以在这里将message封装成task,然后push到任务队列，任务处理交给后端线程池
        }
    }

    // 最开始的时候, conn是没有被触发的
    void Sender(Connection *conn)
    {
        while (true)
        {
            ssize_t n = send(conn->_sock, conn->_outbuffer.c_str(), conn->_outbuffer.size(), 0);
            if (n > 0)
            {
                conn->_outbuffer.erase(0, n);
                if (conn->_outbuffer.empty())
                    break;
            }
            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break; // 已发完
                else if (errno == EINTR)
                    continue; // 发送被中断,重新继续发送
                else
                {
                    // 发送异常
                    logMessage(ERROR, "send error, %d : %s", errno, strerror(errno));
                    conn->_except_cb(conn);
                    break;
                }
            }
        }
        // 发完了吗?不确定,但是可以保证, 如果没有出错,一定要么发完了,要么发送条件不满足,下次发送
        if (conn->_outbuffer.empty())
            EnableReadWrite(conn, true, false); // 发完了，只关心读事件，关闭写
        else
            EnableReadWrite(conn, true, true);
    }

    void EXcepter(Connection *conn)
    {
        if(!IsConnectionExists(conn->_sock)) return;
        // 1. 从epoll中移除
        bool res = _poll.DelFromEpoll(conn->_sock);
        assert(res); //要判断
        // 2. 从我们的unordered_map中移除
        _connections.erase(conn->_sock);
        // 3. close(sock)
        close(conn->_sock);
        // 4. delete conn
        delete conn;

        logMessage(DEBUG, "Excepter 回收完毕, 所有的异常情况");
    }

    void LoopOnce()
    {
        int n = _poll.WaitEpoll(_revs, _revs_num);
        for (int i = 0; i < n; i++)
        {
            int sock = _revs[i].data.fd;
            uint32_t revents = _revs[i].events;

            // 将所有的异常全部交给read或者write来统一处理
            if(revents & EPOLLERR) revents |= (EPOLLIN | EPOLLOUT);
            if(revents & EPOLLHUP) revents |= (EPOLLIN | EPOLLOUT);


            if (revents & EPOLLIN)
            {
                // 如果当前sock存在且_recv_cb是被设置过的
                if (IsConnectionExists(sock) && _connections[sock]->_recv_cb != nullptr)
                    _connections[sock]->_recv_cb(_connections[sock]);
            }
            if (revents & EPOLLOUT)
            {
                if (IsConnectionExists(sock) && _connections[sock]->_send_cb != nullptr)
                    _connections[sock]->_send_cb(_connections[sock]);
            }
        }
    }
/*    void ConnectAliveCheck()
    {
        // 遍历所有的_connections, 通过检测最近conn的活动时间,如果长时间没有动
        // 进入到链接超时的逻辑
        for(auto &iter: _connections)
        {
            uint64_t currtime = time();
            deadtime = currtime - iter->_lasttimestamp;
            if(deadtime > XXX) //差错处理逻辑
        }
    }*/
    // 根据就绪的事件，进行特定事件的派发
    void Dispather(callback_t cb)
    {
        _cb = cb;
        while (true)
        {
            // ConnectAliveCheck();
            LoopOnce(); //将epoll当做定时器使用
        }
    }
    ~TcpServer()
    {
        if (_listensock >= 0)
            close(_listensock);
        if (_revs)
            delete[] _revs;
    }
    bool IsConnectionExists(int sock)
    {
        auto iter = _connections.find(sock);
        if (iter == _connections.end())
            return false;
        else
            return true;
    }

private:
    int _listensock;
    int _port;
    Epoll _poll;
    // sock : connection
    std::unordered_map<int, Connection *> _connections;
    struct epoll_event *_revs;
    int _revs_num;

    // 这里是上层的业务处理
    callback_t _cb;
};