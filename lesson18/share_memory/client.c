#include "comm.h"
int main(){
  key_t key = ftok(PATH_NAME, PROJ_ID);
  if(key < 0){
    perror("ftok");
    return 1;
  }
  printf("%u\n", key);
  //client这里只需获取即可
  int shmid = shmget(key, SIZE, IPC_CREAT);
  if(shmid < 0){
    perror("shmget");
    return 1;
  }
  char *mem = (char*)shmat(shmid, NULL, 0);
  //sleep(5);
  printf("client process attaches success!\n");
  
  //这个地方就是我们要通信的区域
  char c = 'A';
  while(c <= 'Z'){
    mem[c-'A'] = c;
    c++;
    mem[c-'A'] = 0;
    sleep(2);
  }
  shmdt(mem);
  //sleep(5);
  printf("client process detaches success\n");
  return 0;
}
