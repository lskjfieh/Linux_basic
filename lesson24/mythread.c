#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

void *thread_run(void* args){
  while(1){
    printf("main thread id: %lu\n", pthread_self());
    sleep(2);
  }
}
int main(){
  while(1){

    pthread_t tid;
    pthread_create(&tid, NULL, thread_run, (void*)"new thread");
    printf("thread id :%lu, tid:%lu\n", pthread_self(), tid);
    sleep(1);
  }
}

//void *thread_run(void *args){
//  pthread_detach(pthread_self());
//  int num = *(int*)args;
//  while(1){
//    printf("我是新线程[%s]，我创建的线程ID是：%lu\n", (const char*)args, pthread_self());
//    sleep(2);
//    break;
//    //野指针问题，测试一个线程崩整个进程则崩的问题
//    //if(num == 3){
//    //  printf("thread number: %d quit\n", num);
//    //  int *p = NULL;
//    //  *p = 1000;
//    //}
//  }
//  //线程出异常不需要处理
//  //这里的返回值可以是其他变量，并非只能int，但不能是临时变量
//  
//  //pthread_exit((void*)123);
//  return (void*)111;  
//}
//
//#define NUM 1
//
//int main(){
//   
//  pthread_t tid[NUM];
//  int i;
//  for(i = 0; i < NUM; i++){
//    pthread_create(tid+i, NULL, thread_run, (void *)&i);
//    sleep(1);
//  }
//  printf("wait sub thread...\n");
//  sleep(1);
//  printf("cancel sub thread....\n");
//  //pthread_cancel(tid[0]);
//
//  //void* 32.4 64.8, 指针变量。本身就可以充当某种容器保存数据
//  void *status = NULL;
//
//  int ret = 0;
//  for(i = 0; i < NUM; i++){
//    ret = pthread_join(tid[i], &status);
//  }
//
//  printf("ret: %d, status: %d\n", ret, (int)status);
//  sleep(3);
////while(1){
//
//  //  printf("我是主线程，我的thread ID：%lu\n", pthread_self());
//  //  printf("##############################begin#########################\n");
//  //  for(i = 0; i < NUM; i++){
//  //    printf("我创建的线程[%d]是：%lu\n", i, tid[i]);
//  //  }
//  //  printf("###############################end##########################\n");
//  //  sleep(1);
//  //}
//  return 0;
//}
