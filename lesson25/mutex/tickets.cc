#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <mutex>
using namespace std;

//抢票逻辑，1000张票，5线程同时抢
//对临界区进行加锁
class Ticket{
private:
    int tickets;
    // pthread_mutex_t mtx;  //原生线程库
    // mutex mytex;  //C++语言级别
public:
    Ticket():tickets(1000){
        // pthread_mutex_init(&mtx, nullptr);
    }

    bool GetTicket(){
        static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; //定义一把静态锁
        //锁本身也是临界资源，如何保证其是安全的?
        //lock、unlock是原子性的
        //一行汇编，即为原子
        pthread_mutex_lock(&mtx);
        // mytex.lock();
        //执行这部分代码的执行流就是互斥的，串行执行的
        if(tickets > 0){
            //抢票
            usleep(1000); //usleep 微秒  1s = 1000ms 1ms = 1000us
            cout << "我是[" << pthread_self() << "]我要抢的票是：" << tickets << endl;
            tickets--; 
        }
        else{
            cout << "票已经被抢空了" << endl;
            return false;
        }
        pthread_mutex_unlock(&mtx);
        // mytex.unlock();
        return true;
    }
    ~Ticket(){
        // pthread_mutex_destroy(&mtx);
    }
};


void *ThreadRoutine(void *args){
    // string tname = (char*)args;
    // int id = *(int*)args;
    // delete (int*)args;

    Ticket *t = (Ticket*)args;

    while(true){
        if(!t->GetTicket()) break; //抢票失败，退出
    }

}
int main(){
    Ticket *t = new Ticket();

    pthread_t tid[5];
    for(int i = 0; i < 5; i++){
        // int *id = new int(i); 
        pthread_create(tid+i, nullptr, ThreadRoutine, (void*)t); //i值可能会被主线程修改,所以此处用在堆区新建的id
        // pthread_create(tid+i, nullptr, ThreadRun, (void *)"thread 1");
    }

    for(int i = 0; i < 5; i++)
        pthread_join(tid[i], nullptr); //等待

    return 0;
}