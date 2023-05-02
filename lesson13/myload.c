#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
  if(fork() == 0){
    //child
    printf("command begin...\n");
    execl("/usr/bin/python3", "python", "test.py", NULL);
    //exec*
//    execl("/usr/bin/ls","ls","-a", "-l", "-n", "-i", NULL);
 
//    char *argv[]={
//      "ls",
//      "-a",
//      "-l",
//      "-i",
//      NULL
//    };
//    execv("/usr/bin/ls", argv);

//    execlp("ls", "ls", "-a", "-l", "-d");
   
//    char *argv[]={
//      "ls",
//      "-a",
//      "-l",
//      "-i",
//      NULL
//    };
//    execvp("ls", argv);

   // execl("./myexe", "myexe", NULL);
    char *env[] = {
      "MYENV=hahahahahaaaaaahehe\n", 
      "MYENV=hahahahahaaaaaahehe\n",
      "MYENV=hahahahahaaaaaahehe\n",
      "MYENV=hahahahahaaaaaahehe\n",
      "MYENV=hahahahahaaaaaahehe\n",
      NULL
    };
    char *argv[] = {
      "myexe",
      NULL
    };
    execve("./myexe", argv, env);
    printf("command end.. \n");
    exit(1);
  }
  //parent
  waitpid(-1, NULL, 0);
  printf("wait success!\n");












//  printf("I am a process!, pid: %d\n", getpid());
//  //只要进程的程序替换成功，就不会执行后续代码，意味着exec*函数，成功的时候，不需要返回值检测！
//  只要exec*返回了，就一定是因为调用失败了！！
//  execl("/usr/bin/ls", "ls", "-a", "-l", NULL);
//  printf("hahahahhahaha\n");
//  printf("hahahahhahaha\n");
//  printf("hahahahhahaha\n");
//  printf("hahahahhahaha\n");

//  pid_t id = fork();
//  if(id == 0){
//       printf("I am a process!, pid: %d\n", getpid());                      
//       sleep(5);
//       //execl执行程序替换
//       execl("/usr/bin/ls", "ls", "-a", "-l", NULL);
//       //execl("/usr/bin/top", "top", NULL);
//       printf("hahahahhahaha\n");
//       printf("hahahahhahaha\n");
//       printf("hahahahhahaha\n");
//       printf("hahahahhahaha\n");
//       printf("hahahahhahaha\n");
//       printf("hahahahhahaha\n");
//       exit(0);
//  }
//
//  while(1){
//    printf("I am a father!\n");
//    sleep(1);
//  }
//  //parent
//  waitpid(id, NULL, 0);
//  printf("wait success!\n");
//
  return 0;
}

