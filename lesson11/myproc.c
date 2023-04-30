#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int g_unval;
int g_val = 100;
int main(int argc, char *argv[], char *env[]){

  const  char *s = "hello world";
  printf("code addr: %p\n", main);
  printf("string rdonly addr: %p\n", s);
  printf("init addr: %p\n", &g_val);
  printf("uninit addr: %p\n", &g_unval);
  char *heap = (char*)malloc(10);
  char *heap1 = (char*)malloc(10);
  char *heap2 = (char*)malloc(10);
  char *heap3 = (char*)malloc(10);
  char *heap4 = (char*)malloc(10);

  printf("heap addr: %p\n", heap1);
  printf("heap addr: %p\n", heap2);
  printf("heap addr: %p\n", heap3);
  printf("heap addr: %p\n", heap4);

  printf("stack addr: %p\n", &s);
  printf("stack addr: %p\n", &heap);

  int a = 10;
  int b = 30;
  printf("stack addr: %p\n", &a);
  printf("stack addr: %p\n", &b);

  for(int i = 0; argv[i]; i++){
    printf("argv[%d]: %p\n", i, argv[i]);
  }
  for(int i = 0; env[i]; i++){
    printf("env[%d]: %p\n", i, env[i]);
  }
//  //数据是各自私有一份（写时拷贝）
//  if(fork() == 0){
//    //child
//    int cnt = 5;
//    while(cnt){
//      printf("I am child, times: %d, g_val = %d, &g_val = %p\n", cnt, g_val, &g_val);
//      cnt--;
//      sleep(1);
//      if(cnt == 3){
//        printf("####################child更改数据#####################\n");
//        g_val = 200;
//        printf("###################child更改数据done##################\n");
//      }
//    }
//}
//  else{
//    while(1){
//      printf("I am father, g_val = %d, &g_val = %p\n", g_val, &g_val);
//      sleep(1);
//    }
//  }
  return 0;
}
