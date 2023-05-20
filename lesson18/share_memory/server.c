#include "comm.h"
int main(){
  key_t key = ftok(PATH_NAME, PROJ_ID);
  if(key < 0){
    perror("ftok");
    return 1;
  }
  //创建共享内存
  int shmid = shmget(key, SIZE, IPC_CREAT|IPC_EXCL|0666);//创建全新的shm，如果和系统已经存在ID冲突，我们出错返回
  if(shmid < 0){
    perror("shmget");
    return 2;
  }
  printf("key: %u, shmid: %d\n", key, shmid);
  //sleep(10);

  char *mem =(char*)shmat(shmid, NULL, 0); //因为shmat返回值为void*，所以此处要做强转
  printf("attaches shm success\n");
  //sleep(15);

  //此处即为后面要进行的通信逻辑
  while(1){
    sleep(2);
    //这里有没有调用类似pipe or fifo中的read这样的接口呢？
    printf("client# %s\n", mem); // server 任务共享内存里面放的是一个长字符串

  }
  
  shmdt(mem);
  printf("detaches shm success\n"); 
  //sleep(5);
  shmctl(shmid, IPC_RMID, NULL); //属性设为NULL
  printf("key: 0X%x, shmid: %d -> shm delete success\n", key, shmid);

  //sleep(10);
  return 0;
}
