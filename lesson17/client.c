#include "comm.h"
int main(){
  //无需再创建fifo，只需获取即可
  int fd = open(MY_FIFO, O_WRONLY);
  if(fd < 0){
    perror("open");
    return 1;
  }
  //业务逻辑
  while(1){
    printf("请输入# ");
      fflush(stdout);
    char buffer[64] = {0};
    //先把数据从标准输入拿到我们的client进程内部
    ssize_t s = read(0, buffer, sizeof(buffer)-1); //使用的系统调用，/n也是输入字符的一部分，故需要s-1
    if(s > 0){
      buffer[s-1] = 0;
      printf("%s\n", buffer);

      //拿到了数据
      write(fd, buffer, strlen(buffer));
    }
  }
  close(fd);
  return 0;
}
