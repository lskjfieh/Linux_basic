#include "thread_pool.hpp"
#include "Task.hpp"

#include <ctime>
#include <cstdlib>

using namespace ns_task;
using namespace ns_thread_pool;
int main(){
    ThreadPool<Task> *tp = new ThreadPool<Task>(10);
    tp->InitThreadPool();
    srand((long long)time(nullptr));
    while(true){
        //主线程从网络来
        //派发任务交给线程池，线程池有若干线程定期消费任务
        Task t(rand()%20+1, rand()%10+1, "+-*/%"[rand()%5]);
        tp->PushTask(t);
        sleep(1);
    }
    
    return 0;
}