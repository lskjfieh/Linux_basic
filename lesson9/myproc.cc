#include <iostream>
#include <cstdlib>
#include <unistd.h>
int main(){
  pid_t id = fork();
  if(id == 0){
     //child
    while(true){
      std::cout << "I am child, running !" << std::endl;
      sleep(2);
    }
  }
  else{
    //parent
    std::cout << "father do nothing!\n" << std::endl;
    sleep(10);
    exit(1);
  }
  /*while(true){
    std::cout << " hello world !" << std::endl;
  }
  pid_t id = fork(); //int

  if(id == 0){
    //child
    while(true){
      std::cout << "I am child, pid: " << getpid() << ", ppid: " << getppid() << std::endl;
      sleep(1);
    }
  }
  else if(id > 0){
    //parent
    while(true){
      std::cout << "I am parent, pid: " << getpid() << ", ppid: " << getppid() << std::endl;
      sleep(2);
    }
  }
  else{
    //TODU
  }
  //std::cout << "hello proc: " << getpid() << "  hello parent:" << getppid() << "  ret: "<< id << std:: endl;
  
  sleep(1); 
*/return 0;
}
