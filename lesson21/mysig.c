#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void handler(int signo){
  printf("get a signo: %d\n", signo);
}
int main(){
  struct sigaction act;
  memset(&act, 0, sizeof(act));

  act.sa_handler = handler;
  sigemptyset(&act.sa_mask); //清理工作
  sigaddset(&act.sa_mask, 3);
  //act.sa_handler = SIG_IGN; //也可改为默认
  //act.sa_handler = SIG_DFL; //改为忽略
  
  //本质是修改当前进程的handler函数指针数组特定内容
  sigaction(2, &act, NULL);

  while(1){
    printf("hello world!\n");
    sleep(1);
  }
  return 0;
}
//void show_pending(sigset_t *set){
//  printf("curr process pending: ");
//  int i;
//  for(i = 1; i <= 31; i++){
//    if(sigismember(set, i)){
//      printf("1");
//    }
//    else{
//      printf("0");
//    }
//  }
//  printf("\n");
//}
//void handler(int signo){
//  printf("%d 号信号被递达了，已经处理完成！\n",signo);
//}
//int main(){
//
//  //虽然sigget_t 是一个位图结构，但是不同的OS实现是不一样的，不能让用户直接修改该变量
//  //需要使用特定的函数
//  
//  //set是一个变量，该变量在什么地方保存？
//  //在用户栈上
//  
//  signal(2, handler);
//
//  sigset_t iset, oset;
//  
//  sigemptyset(&iset);
//  sigemptyset(&oset); 
//  sigaddset(&iset, 2);//添加信号
//
// //1. 设置当前进程的屏蔽字
// //2. 获取当前进程老的屏蔽字
//  sigprocmask(SIG_SETMASK, &iset, &oset);
//
//  sigset_t pending;
//
//  int count = 0;
//  while(1){
//    sigemptyset(&pending);
//
//    sigpending(&pending);
//
//    show_pending(&pending);
//    //printf("hello world!\n");
//    sleep(1);
//
//    count++;
//    if(count == 20){
//      //取消屏蔽
//      sigprocmask(SIG_SETMASK, &oset, NULL);
//      printf("恢复2号信号，可以被递达了\n");
//    }
// }
//  return 0;
//}
