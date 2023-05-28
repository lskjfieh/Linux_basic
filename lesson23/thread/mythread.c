#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread_run(void *args){
  const char *id = (const char*)args;
  while(1){
    printf("我是%s线程，%d\n", id, getpid());
    sleep(1);
  }
}
int main(){

  pthread_t tid;
  pthread_create(&tid, NULL, thread_run, (void*)"thread 1");

  while(1){
    printf("我是main线程, getpid: %d\n", getpid());
    sleep(1);
  }
  return 0;
}
