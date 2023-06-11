#include "thread_pool.hpp"
#include "Task.hpp"

#include <ctime>
#include <cstdlib>

using namespace ns_task;
using namespace ns_thread_pool;
int main(){

    std::cout << "当前正在运行我的进程其他代码 ..." << std::endl;
    std::cout << "当前正在运行我的进程其他代码 ..." << std::endl;
    std::cout << "当前正在运行我的进程其他代码 ..." << std::endl;
    std::cout << "当前正在运行我的进程其他代码 ..." << std::endl;
    std::cout << "当前正在运行我的进程其他代码 ..." << std::endl;
    sleep(5);
    srand((long long)time(nullptr));
    while(true){
        sleep(1);

        Task t(rand()%20+1, rand()%10+1, "+-*/%"[rand()%5]);
        ThreadPool<Task>::GetInstance()->PushTask(t);
        //单例本身会在任何场景，任何环境下被调用
        //GetInstance(): 被多线程重入，进而导致线程安全的问题
        //打印单例地址
        std::cout << ThreadPool<Task>::GetInstance() << std::endl;
        
    }
    return 0;
}