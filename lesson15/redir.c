#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(){
  const char *msg1 = "hello 标准输出\n";
  write(1, msg1, strlen(msg1));

  printf("hello printf\n");          //stdout -> FILE* -> FILE struct -> 1.fd 2. buffer ->刷新策略 ->目标文件类别
  fprintf(stdout, "hello fprintf\n");//stdout
  fputs("hello fputs\n", stdout);    //stdout

  fork();
  
  return 0;
}
