#include<stdio.h>
#include<unistd.h>
#include<string.h>

int main(){
#define NUM 100
 char bar[NUM+1];
 memset(bar, '\0', sizeof(bar));
 int i = 0;
 //添加旋转光标，同一位置刷新不同字符，参考倒计时
 const char *lable="|/-\\";
 while(i <= 100){
   printf("[%-100s][%3d%%][%c]\r", bar, i, lable[i%4]);//右对齐并预留100个位置,加上百分数
   fflush(stdout);
   bar[i]='#';
   i++;
   usleep(50000);
 }
  return 0;
}
