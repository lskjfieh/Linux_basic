#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
void GetChild(int signo){
  //waitpid(); 
  printf("get a singal: %d, pid: %d\n", signo, getpid());
}
int main(){
  //signal(SIGCHLD, GetChild);
  //只在linux下有效
  signal(SIGCHLD, SIG_IGN); //显示设置忽略17号信号，当进程退出后，自动释放僵尸进程
  pid_t id = fork();
  if(id == 0){
    //child
    int cnt = 5;
    while(cnt){
      printf("我是子进程：%d\n", getpid());
      sleep(1);
      cnt--;
    }
    exit(0);
  }
  while(1);
  return 0;
}
