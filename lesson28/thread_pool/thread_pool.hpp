#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <unistd.h>
#include <pthread.h>

namespace ns_thread_pool
{
    const int g_num = 5;
    template<class T>
    class ThreadPool{
    private:
        int num_; //线程池内线程数目
        std::queue<T> task_queue_; //任务队列，该成员是一个临界资源

        pthread_mutex_t mtx_;
        pthread_cond_t cond_;
    public:
        void Lock(){
            pthread_mutex_lock(&mtx_);
        }
        void Unlock(){
            pthread_mutex_unlock(&mtx_);
        }
        void Wait(){
            pthread_cond_wait(&cond_, &mtx_);
        }
        void Wakeup(){
            pthread_cond_signal(&cond_);
        }
        bool IsEmpty(){
            return task_queue_.empty();
        }
    public:
        ThreadPool(int num = g_num):num_(num)
        {
            pthread_mutex_init(&mtx_, nullptr);
            pthread_cond_init(&cond_, nullptr);
        }
        //在类中，要让线程执行类内成员方法，是不可行的
        //默认会传this， 必须让线程执行静态方法(类内静态方法无this指针，也无法直接访问类内非static成员属性)
        static void *Rountine(void *args){
            pthread_detach(pthread_self()); //分离线程，执行完自动回收
            ThreadPool<T> *tp = (ThreadPool<T> *)args;
            while(true){
                tp->Lock();
                while(tp->IsEmpty()){
                    //任务队列为空，线程该做什么呢?
                    tp->Wait();
                }
                //被Wakeup了
                //该任务队列中一定有任务了
                T t;
                tp->PopTask(&t);
                tp->Unlock();

                t();
            }
        }
        void InitThreadPool()
        {
            pthread_t tid;
            for(int i = 0; i < num_; i++){
                pthread_create(&tid, nullptr, Rountine, (void *)this);
            }

        }
        void PushTask(const T& in){
            Lock();
            task_queue_.push(in);
            Unlock();
            Wakeup();
        }
        void PopTask(T *out){
            *out = task_queue_.front();
            task_queue_.pop();
        }
        ~ThreadPool()
        {
            pthread_mutex_destroy(&mtx_);
            pthread_cond_destroy(&cond_);
        }
    };
}