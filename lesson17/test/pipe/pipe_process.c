#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(){
  int pipefd[2] = {0};
  if(pipe(pipefd) != 0){
    perror("pipe error");
    return 1;
  }
  //pipefd[0] pipefd[1]
  //0(嘴)：读取端  1(笔)：写入端
  printf("pipefd[0]: %d\n", pipefd[0]);
  printf("pipefd[1]: %d\n", pipefd[1]);

  //我们想让父进程进行读取，子进程写入
  if(fork() == 0){
    //子进程
    close(pipefd[0]); //子进程写入，故关闭读取端
    
    const char *msg = "hahaha!";
    int count = 0;
    while(1){
      //pipe里面只要有缓冲区，就一直写入
      write(pipefd[1], msg, strlen(msg)); //strlen(msg)+1 ?? 不需要 \n不被写入
      //sleep(1);
      //write(pipefd[1], "a", 1);
      //count++;
      //printf("count: %d\n", count);
    }
    close(pipefd[1]);
    exit(0);
  }
  //父进程
  close(pipefd[1]); //父进程读取，故关闭写入端
  while(1){
    //没有让父进程sleep->子进程写得慢，父进程读得快
    //让父进程sleep，只要有数据，就可以一直读取
    //sleep(10);
    char c[64] = {0};
    ssize_t s = read(pipefd[0], c, sizeof(c));
    if(s > 0){
      c[s] = 0;
      printf("father take: %s\n", c);
    }else if(s == 0){
      printf("writer quit...\n");
      break;
    }
    else{
      break;
    }
    break;
//    char buffer[64] = {0};
//    ssize_t s = read(pipefd[0], buffer, sizeof(buffer)-1);//如果read的返回值是0，意味子进程关闭文件描述符
//    if(s == 0){
//      printf("child quit...\n");
//      break;
//    }
//    else if(s > 0){
//      buffer[s] = 0;
//      printf("child say# %s\n", buffer);
//    }else{
//      printf("read error...\n");
//      break;
//      }
  }
  close(pipefd[0]);
  int status = 0;
  waitpid(-1, &status, 0);
  //status 中如何查看子进程是如何结束的呢？
  printf("exit code: %d\n", (status>>8) & 0xFF);
  printf("exit signal: %d\n", status & 0x7F);

 return 0;
}

//pipefd[2]:是一个输出性参数！我们想通过这个参数读取到打开的两个fd
//int pipe(int pipefd[2]); 成功返回0，失败返回-1
