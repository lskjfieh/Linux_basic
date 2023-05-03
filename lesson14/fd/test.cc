#include <iostream>
#include <string>
#include <fstream>

int main(){
  std::ofstream out("./log.txt", std::ios::out | std::ios::binary);
  if(!out.is_open()){
    std::cerr << "open error" << std::endl;
    return 1;
  }
  int cnt = 5;
  while(cnt--){
    std::string msg = "hello world\n";
    out.write(msg.c_str(), msg.size());
  }
  out.close();
}
