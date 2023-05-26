#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

int count = 0;

void HandlerAlarm(int signo){
  printf("hello: %d\n", count);
  exit(1);
}
//统计一下，1s，server能够对int递增到多少
int main(){
  signal(SIGALRM, HandlerAlarm);
  alarm(1); //没有设置alarm信号的捕捉动作(无自定义)，执行默认动作，即终止进程

  while(1){
    count++;
  //  printf("hello: %d\n", count++);
  }
  return 0;
}
//void handler(int signo){
//  switch(signo){
//    case 2:
//      printf("hello everyday, get a signal: %d\n", signo);
//      break;
//    case 3:
//      printf("hello world, git a signal: %d\n", signo);
//      break;
//    case 9:
//      printf("hello ....get a signal: %d\n", signo);
//      break;
//    default:
//      printf("hello signo：%d\n", signo);
//      break;
//  }
//  exit(1);
//}
//int main(){
//
//  //将所有的信号进行捕捉
//  int sig = 1;
//  for(; sig <= 31; sig++){
//    signal(sig, handler);
//  }
//  //3秒后终止进程
//  int ret = alarm(30);
//
//  while(1){
//  printf("I am a process! ret: %d\n", ret);
//  sleep(1);
//  
//  int res = alarm(0); //取消闹钟
//  printf(" res:%d\n", res);
//
//  }
//  return 0;
//}
//使用手册
//static void Usage(const char *proc){
//  printf("Usage:\n\t %s signo who\n", proc);
//}
////2. raise 调用系统函数给自己发信号 fork子进程
//int main(int argc, char *argv[]){
//  while(1){
//    printf("I am a process\n");
//    sleep(3);
//
//    raise(8); //出现浮点数指针异常
//  }
//
//}
//1. kill 调用系统函数给别人发信号
// ./mytest signo who
//int main(int argc, char *argv[]){
//  if(argc != 3){
//    Usage(argv[0]);
//    return 1;
//  }
//  int signo = atoi(argv[1]); //字符串转换为整数
//  int who = atoi(argv[2]);
//
//  //调用kill系统函数
//  kill(who, signo);
//  printf("signo: %d, who: %d\n", signo, who);
//  return 0;
//}
//
//int main(){
//  if(fork() == 0){
//    while(1){
//      printf("I am child....\n");
//      int a = 10;
//      a /= 0;
//    }
//  }
//  int status = 0;
//  waitpid(-1, &status, 0);
//
//  printf("exit code: %d, exit sig: %d, core dump flag: %d\n", (status>>8)&0xFF, status&0x7F, (status>>7) & 1);
//
//}

//void handler(int signo){
//  switch(signo){
//    case 2:
//      printf("hello everyday, get a signal: %d\n", signo);
//      break;
//    case 3:
//      printf("hello world, git a signal: %d\n", signo);
//      break;
//    case 9:
//      printf("hello ....get a signal: %d\n", signo);
//      break;
//    default:
//      printf("hello signo：%d\n", signo);
//      break;
//  }
//}
//int main(){
//
//  //将所有的信号进行捕捉
//  int sig = 1;
//  for(; sig <= 31; sig++){
//    signal(sig, handler);
//  }
//  //通过signal注册对2号信号的处理动作，改成我们的自定义动作
//  //signal(2, handler);
//
//  while(1){
//
//    int *p = NULL;
//    p = (int*)100;
//    printf("hello world! pid：%d\n", getpid());
//    sleep(1);
//  }
//  return 0;
//}
