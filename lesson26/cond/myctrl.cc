#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
using namespace std;

pthread_mutex_t mtx;
pthread_cond_t cond;
//ctrl thread控制work线程，让他定期运行
void *ctrl(void *args){
    string name = (char*)args;
    while(true){
        //唤醒在该条件变量下等待的线程，哪一个?
        cout << "master say : begin work" << endl;
        // pthread_cond_signal(&cond);
        //唤醒所有线程
        pthread_cond_broadcast(&cond);
        sleep(5);
    }
}
void *work(void *args){
    int number = *(int*)args;
    delete (int*)args;
    while(true){
        //此处我们的mutex不用
        pthread_cond_wait(&cond, &mtx);
        cout << "worker: " << number << "is working ... " << endl;
    }
}
#define NUM 3
int main(){
    pthread_mutex_init(&mtx, nullptr);
    pthread_cond_init(&cond, nullptr);

    pthread_t master;
    pthread_t worker[5];
    pthread_create(&master, nullptr, ctrl, (void*)"boss");
    for(int i = 0; i < NUM; i++){
        int *number = new int(i);
        pthread_create(worker+i, nullptr, work, (void*)number);
    }
    for(int i = 0; i < NUM; i++){

        pthread_join(worker[i], nullptr);
    }
    pthread_join(master, nullptr);
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cond);
    return 0;
} 