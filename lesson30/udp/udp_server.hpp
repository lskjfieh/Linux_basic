#ifndef _UDP_SERVER_HPP
#define _UDP_SERVER_HPP

#include <iostream>
#include <unordered_map>
#include <string>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <queue>
#include "log.hpp"

#define SIZE 1024

class UdpServer{
public:
    UdpServer(uint16_t port, std::string ip="") :_port(port), _ip(ip), _sock(-1)
    {}
    bool initServer(){
        //从这里开始，就是新的系统调用，来完成网络功能
        //1.创建套接字
        _sock = socket(AF_INET, SOCK_DGRAM, 0);
        if(_sock < 0){
            logMessage(FATAL, "%d : %s", errno, strerror(errno));
            exit(2);
        }
        //2. bind：将用户设置的ip和port在内核中和我们当前的进程强关联
        // "192.168.1.3" -> 点分十进制字符串风格的IP地址
        //每一个区域取值范围是[0-255]：1字节 2^8 -> 4个区域
        //点分十进制字符串风格的IP地址 <-> 4字节
        struct sockaddr_in local;

        bzero(&local, sizeof(local)); //当前字段清零
        local.sin_family = AF_INET;//ipv4 协议家族/域
        //服务器的IP和端口未来也是要发送给对方主机的 -> 先要将数据发送到网络
        local.sin_port = htons(_port);  //转参
        //1. 同上，点分十进制先要将字符串风格的IP地址-> 4字节IP
        //2. 4字节主机序列 -> 网络序列
        //有一套接口，可以一次帮我们做完这两件事情
        //绑定任意ip
        local.sin_addr.s_addr = _ip.empty() ? INADDR_ANY : inet_addr(_ip.c_str());

        if(bind(_sock, (struct sockaddr*)&local, sizeof(local)) < 0){
            logMessage(FATAL, "%d : %s", errno, strerror(errno));
            exit(2);            
        }
        logMessage(NORMAL, "init udp server done ... %s", strerror(errno));
        //done
        return true;
    }
    void Start(){
        //作为一款网络服务器，永远不退出的！所以是一个死循环
        //服务器启动 -> 进程 -> 常驻进程 -> 永远在内存中存在，除非挂了/宕机
        //echo server：client给我们发送消息，我们原封不动返回
        char buffer[SIZE];
        for( ; ;){
            //注意：
             //peer，纯输出型参数
            struct sockaddr_in peer;
            bzero(&peer, sizeof(peer));
            //输入：peer缓冲区大小
            //输出：实际读到的peer
            socklen_t len = sizeof(peer);  //输入输出型参数

            char result[256];
            char key[64];
            std::string cmd_echo;
            //start 读取数据
            ssize_t s = recvfrom(_sock, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&peer, &len);
            if(s > 0){
                buffer[s] = 0; //目前数据当做字符串
                // if(strcasestr(buffer, "rm") != nullptr || strcasestr(buffer, "rmdir") != nullptr){
                //     std::string err_message = "坏人...."; 
                //     std::cout << err_message << buffer << std::endl;
                //     sendto(_sock, err_message.c_str(), err_message.size(), 0, (struct sockaddr*)&peer, len);
                //     continue;
                // }
                // FILE *fp = popen(buffer, "r");
                // if(nullptr == fp){
                //     logMessage(ERROR, "popen: %d:%s", errno, strerror(errno));
                //     continue;
                // }

                // while (fgets(result, sizeof(result), fp) != nullptr)
                // {
                //     cmd_echo += result;
                // }
                // fclose(fp);
                uint16_t cli_port = ntohs(peer.sin_port); //peer是从网络中来的，需转换
                std::string cli_ip = inet_ntoa(peer.sin_addr); // 4字节的网络序列的IP->本主机的点分十进制字符串风格的IP地址
                // printf("[%s:%d]# %s\n", cli_ip.c_str(), cli_port, buffer);
                
                //简单的用户托管
                snprintf(key, sizeof key, "%s-%d", cli_ip.c_str(), cli_port); // 127.0.0.1-8080
                logMessage(NORMAL, "key: %s", key);
                auto it = _users.find(key);
                if(it == _users.end()){
                    //not exits
                    logMessage(NORMAL, "add new user : %s", key);
                    _users.insert({key, peer});
                }

            }
            //分析和处理数据， TODO
            //end. 写回数据
            // sendto(_sock, buffer, strlen(buffer), 0, (struct sockaddr*)&peer, len);
            // sendto(_sock, cmd_echo.c_str(), cmd_echo.size(), 0, (struct sockaddr*)&peer, len);
            for(auto &iter : _users){
                /*key，即前面的用ip和port来标识一个用户的身份*/
                std::string sendMessage = key;
                sendMessage += "#";
                sendMessage += buffer;  //127.0.0.1-1234# 你好
                logMessage(NORMAL, "push message to %s", iter.first.c_str());
                sendto(_sock, sendMessage.c_str(), sendMessage.size(),0, (struct sockaddr *)&(iter.second), sizeof(iter.second));
            }
        
        }
    }
    ~UdpServer()
    {
        if(_sock >= 0) close(_sock);
    }
private:
    //一个服务器，一般必须需要ip地址和port
    uint16_t _port;
    std::string _ip;
    int _sock;
    std::unordered_map<std::string, struct sockaddr_in> _users;
    std::queue<std::string> messageQueue;
};
#endif