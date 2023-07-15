#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static void usage(std::string proc){
    std::cout << "\nUsage: " << proc << "serverIp serverPort\n" << std::endl;
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
    //client一般不需要显示的bind指定port，而是让OS自动随机选择（什么时候选择）
    std::string message;
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    server.sin_addr.s_addr = inet_addr(argv[1]);
    
    char buffer[1024];
    while(true){

        std::cout << "请输入你的信息# ";
        std::getline(std::cin, message);
        //当client首次发送消息给服务器的时候，OS会自动给client bind它的IP和PORT
        sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr*)&server, sizeof(server));

        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);
        ssize_t s = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&temp, &len);
        if(s > 0){
            buffer[s] = 0;
            std::cout << "server echo# " << buffer << std::endl;
        }
    }
    close(sock);
    return 0;
}