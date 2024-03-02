#include "main.h"
namespace rlt = rl_tools;

using namespace std;
int main() {
  DEVICE device;
  TI seed = 0;
  rlt::malloc(device);
  rlt::init(device);
  LOOP_STATE ts;
  rlt::malloc(device, ts);
  rlt::init(device, ts, seed);
  while(!rlt::step(device, ts)){
      if(ts.step == 5000){
          std::cout << "steppin yourself > callbacks 'n' hooks: " << ts.step << std::endl;
      }
  }
  rlt::free(device);
using namespace rlt;


   
      // Rest of your code...
  // Remember to handle errors appropriately and close sockets when done.
  close(sockfd);
  return 0;
}

