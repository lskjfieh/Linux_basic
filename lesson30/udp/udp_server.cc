#include "udp_server.hpp"
#include <memory>

static void usage(std::string proc){
    std::cout << "\nUsage: " << proc << "ip port\n" << std::endl;
}
int main(int argc, char *argv[]){

    if(argc != 2){
        usage(argv[0]);
        exit(1);
    }
    //std::string ip = argv[1];
    uint16_t port = atoi(argv[1]);
    std::unique_ptr<UdpServer> svr(new UdpServer(port));
    
    svr->initServer();
    svr->Start();
    return 0;
}