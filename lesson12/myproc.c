#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
  pid_t id = fork();
  if(id == 0){
    //child
    int cnt = 3;
    while(cnt){
      printf("child[%d] is running: cnt is : %d\n", getpid(), cnt);
      cnt--;
      sleep(1);
    }
    exit(11);
  }
  sleep(10);
  printf("father wait begin!\n");
//  pid_t ret = wait(NULL);
  int status = 0;
  pid_t ret = waitpid(id, &status, 0/*默认行为，阻塞等待，WNOHANHG: 设置等待方式为非阻塞*/);
  if(ret > 0){
    if(WIFEXITED(status)){ //没有收到任何退出信号的
      //正常结束的，获取对应的退出码
      printf("exit code: %d\n", WIFEXITED(status));
    }
  }
//
//  if(ret > 0){
//    printf("father wait: %d, success, status exit code: %d, status exit signal：%d\n", ret, (status>>8)&0XFF, status&0X7F);
//  }
//  else{
//    printf("father wait failed!\n");
//  }
//  sleep(10);
  //parent

}







//int fun(){
//  exit(12);
//  printf("fun test!\n");
//  return 1;
//}
//int main(){
//  fun();
//  for(int i = 0; i < 140; i++){
//    printf("%d: %s\n", i, strerror(i));
//  }
//  return 0;
//}
