#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define NUM 128
#define CMD_NUM 64
int main(){

  char command[NUM];
  for( ; ;  ){
    char *argv[CMD_NUM] = {NULL};
    //1. 打印提示符
    command[0] = 0; //用这种方式，可以做到O(1)时间复杂度，清空字符串
    printf("[who@myhostname mydir]# ");
   
    //2. 获取命令字符串
    fgets(command, NUM, stdin);
    command[strlen(command)-1] = '\0'; 
    printf("echo: %s\n",command);
    //fflush(stdout);
    
    //3. 解析命令字符串, char *argv[]
    const char *sep = " ";
    argv[0] = strtok(command, sep);
    int i = 1;
    while(argv[i] = strtok(NULL, sep)){
      i++;
    }
    //4. 检测命令是否是需要shell本身执行的，内建命令
    if(strcmp(argv[0], "cd") == 0){
      if(argv[1] != NULL) chdir(argv[1]);
      continue;
    }

    //5. 执行第三方命令
    if(fork() == 0){
      //child
      execvp(argv[0], argv);
      exit(1);//若替换失败，直接终止
    }
    int status = 0;
    waitpid(-1, &status, 0);
    printf("exit code: %d\n", (status >> 8)&0xFF);
    //for(i = 0; argv[i]; i++){
    //  printf("argv[%d]: %s\n", i, argv[i]);
    //}
  }
  return 0;
}
