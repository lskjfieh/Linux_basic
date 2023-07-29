#include "selectServer.hpp"
#include <memory>
int main()
{
    //1. fd_set是一个固定大小的位图，直接决定了selec能同时关心的fd的个数是有上限的
    // std::cout << sizeof(fd_set) * 8 << std::endl;
    // fd_set fds;
    std::unique_ptr<SelectServer> svr(new SelectServer());
    svr->Start();
    return 0;
}