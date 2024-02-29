#include "main.h"
namespace rlt = rl_tools;

using namespace std;
int main() {
  DEVICE device;
  TI seed = 1337;
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
  
    // Create a UDP socket.
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd == -1) {
      perror("socket failed");
      return 1;
  }

  // Bind the socket to a local address.
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8081);
  addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) == -1) {
      perror("bind failed");
      close(sockfd);
      return 1;
  }

  sockaddr_in send_addr;
  send_addr.sin_family = AF_INET;
  send_addr.sin_port = htons(8081);
  const char* addie = "10.0.0.2";
  send_addr.sin_addr.s_addr = inet_addr(addie);


  int count = 0;
  int U = 0;
  int result = 0;
  while(1){
    int bytesReceived = recvfrom(sockfd, (char*)(&datum), sizeof(dati), 0, NULL, NULL);
    if (bytesReceived < 0) {
       cerr << "Error receiving data" << endl;
       return 1;
    } else {
       if ((abs(datum.encPos) > hysteresisCutoff)) {
          U = Kc * (proportionalTimeAdjustment * float(datum.encPos) + Td * datum.encVel);
       }
       else {
          U = 0;
       }
       result = sendto(sockfd, (char*)&U, sizeof(int), 0, (sockaddr*)&send_addr, sizeof(send_addr));
       if (result == -1) {
          perror("sendto failed: ");
          close(sockfd);
        
          int x;
          cin >> x;
          return 1;
       }
    
    }
    }
    count++;
    
      // Rest of your code...
  // Remember to handle errors appropriately and close sockets when done.
  close(sockfd);
  return 0;
}

