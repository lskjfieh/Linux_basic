#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(){

  //int fd = open("./log.txt", O_WRONLY|O_TRUNC);
  int fd = open("./log.txt", O_RDWR);
  if(fd < 0){
    perror("open");
    return 1;
  }

  //dup2(fd, 1); //本来应该显示到显示器的内容，写入到文件！
  dup2(fd, 0); //本来应该从键盘读入，改为从文件读入

  char buffer[1024];
  scanf("%s", buffer);

  printf("%s\n", buffer);
  
  //stdout -> 1 -> struct file* -> log.txt
//  printf("hello printf\n");
//  fprintf(stdout, "hello fprintf\n");
//  fputs("hello fputs\n", stdout);

//  printf("stdin -> %d\n", stdin->_fileno);
//  printf("stdout -> %d\n", stdout->_fileno);
//  printf("stderr -> %d\n", stderr->_fileno);
//  
//  FILE *fp = fopen("./log.txt", "r");
//  if(NULL == fp){
//    perror("fopen");
//    return 0;
//  }

//  printf("fp -> %d\n", fp->_fileno);
//  close(0);
//  int fd = open("./log.txt", O_RDONLY);
//  //fd == 0
//  printf("fd: %d\n", fd);
//  char line[128];
//  while(fgets(line, sizeof(line)-1,stdin)){ //stdin ->FILE * -> FILE 是一个结构体 -> fd == 0
//    printf("%s", line);
//  }
}
  //追加重定向
//  close(1);
//  int fd = open("./log.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);
//  printf("fd: %d\n", fd);
//
//  fprintf(stdout, "hello world\n");
//  fprintf(stdout, "hello world\n");
//  fprintf(stdout, "hello world\n");
//  fprintf(stdout, "hello world\n");
//  fprintf(stdout, "hello world\n");
//  fprintf(stdout, "hello world\n");

//  char buff[64];
//  size_t s = read(0, buff, sizeof(buff));
//  buff[s-1] = 0;
//
//  printf("echo# %s\n", buff);

//}
//  const char *msg = "hello world\n";
//  write(2, msg,  strlen(msg));
//  write(2, msg,  strlen(msg));
//  write(2, msg,  strlen(msg));
//  write(2, msg,  strlen(msg));
//  write(2, msg,  strlen(msg));
//  return 0;
//}
//  //已经创建，故参数只需两个参数
//  int fd = open("./log.txt", O_RDONLY);
//  if(fd < 0) {
//    perror("open");
//    return 1;
//  }
//
//  char buffer[1024];
//  ssize_t s = read(fd, buffer, sizeof(buffer)-1);
//
//  if(s > 0){
//    buffer[s] = 0;
//    printf("%s\n", buffer);
//  }
//  int fd0 = open("./log0.txt", O_WRONLY | O_CREAT, 0644);
//  int fd1 = open("./log1.txt", O_WRONLY | O_CREAT, 0644);
//  int fd2 = open("./log2.txt", O_WRONLY | O_CREAT, 0644);
//  int fd3 = open("./log3.txt", O_WRONLY | O_CREAT, 0644);
//  int fd4 = open("./log4.txt", O_WRONLY | O_CREAT, 0644);
//
//  printf("%d, %d, %d, %d, %d\n", fd0, fd1, fd2, fd3, fd4);
//  
//  if(fd < 0){
//    printf("open\n");
//    return 1;
//  }
//  //printf("fd: %d\n", fd);
//  
//  const char *msg = "hello world\n";
//
//  int cnt = 5;
//  while(cnt){
//    write(fd, msg, strlen(msg)); //在写入文件的过程种，需要写入\0吗？不需要
//    //\0作为字符串结束的标志位，只是c的规定
//    cnt--;
//  }


//  close(fd0);
//  close(fd1);
//  close(fd2);
//  close(fd3);
//  close(fd4);
//}

//int main(){
//  const char *msg = "you can see me!\n"; 
//  //fputs(msg, stdout);//向显示器写入
//  fputs(msg, stderr); 
//}
/*int main(){
  
  //FILE *fp = fopen("./log.txt", "w");
  //FILE *fp = fopen("./log.txt", "r");
  FILE *fp = fopen("./log.txt", "a");//追加
  if(NULL == fp){
    perror("fopen");
    return 1;
  }

 // char buffer[64];
 // while(fgets(buffer, sizeof(buffer), fp)){
 //   printf("%s", buffer);
 // }

 // if(!feof(fp)){
 //   printf("fgets quit not normal!\n");
 // }else{
 //   printf("fgets quit normal!\n");
 // }
 
  const char *msg = "hello world\n";
  int cnt = 10;
  while(cnt--){ 
    fputs(msg, fp);
  }


  fclose(fp);
}*/
