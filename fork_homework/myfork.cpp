#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;
int main(){
  pid_t pid = fork();
  if(pid < 0){
   cout << "fork error" << __FUNCTION__ << endl;
   return 1;
  }else if( pid == 0 ){
    //child
    cout << "I am child process" << endl;
    sleep(5);
    exit(1);
  }else{
    int status = 0;
    pid_t ret = 0;
    do{
      ret = waitpid(-1, &status, WNOHANG);
      if(ret == 0){
        cout << "child is running\n" << endl;
      }
      sleep(1);
    }while(ret == 0);

    if(WIFEXITED(status) && ret == pid){
      cout << "wait child 5s success, child return code is : " << WEXITSTATUS(status) << endl;
    }
    else{
      cout << "wait child failed" << endl;
      return 1;
    }
  }
  return 0;
}
