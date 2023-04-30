#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int g_val = 100;
int main(){
  //数据是各自私有一份（写时拷贝）
  if(fork() == 0){
    //child
    int cnt = 5;
    while(cnt){
      printf("I am child, times: %d, g_val = %d, &g_val = %p\n", cnt, g_val, &g_val);
      cnt--;
      sleep(1);
      if(cnt == 3){
        printf("####################child更改数据#####################\n");
        g_val = 200;
        printf("###################child更改数据done##################\n");
      }
    }
  }
  else{
    while(1){
      printf("I am father, g_val = %d, &g_val = %p\n", g_val, &g_val);
      sleep(1);
    }
  }
  return 0;
}
