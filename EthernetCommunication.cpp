#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "constants.h"
#include <cstring> // for strerror
#include <cerrno>  // for errno

#define RL_TOOLS_BACKEND_ENABLE_OPENBLAS
#include <rl_tools/operations/cpu_mux.h>
#include <rl_tools/nn/operations_cpu_mux.h>
#include <rl_tools/nn_models/operations_cpu.h>

#include "furuta/furuta.h"
#include "furuta/operations_generic.h"
#include <rl_tools/rl/algorithms/ppo/loop/core/config.h>
#include <rl_tools/rl/algorithms/ppo/loop/core/operations_generic.h>
#include <rl_tools/rl/loop/steps/evaluation/config.h>
#include <rl_tools/rl/loop/steps/evaluation/operations_generic.h>


namespace rlt = rl_tools;

using DEVICE = rlt::devices::DEVICE_FACTORY<>;
using T = float;
using TI = typename DEVICE::index_t;
using RNG = decltype(rlt::random::default_engine(typename DEVICE::SPEC::RANDOM{}));
using FURUTA_SPEC = MyFurutaSpecification<T, TI, MyFurutaParameters<T>>;
using ENVIRONMENT = MyFuruta<FURUTA_SPEC>;

struct LOOP_CORE_PARAMETERS: rlt::rl::algorithms::ppo::loop::core::Parameters<T, TI, ENVIRONMENT>{
    static constexpr TI EPISODE_STEP_LIMIT = 200;
    static constexpr TI TOTAL_STEP_LIMIT = 300000;
    static constexpr TI STEP_LIMIT = TOTAL_STEP_LIMIT/(ON_POLICY_RUNNER_STEPS_PER_ENV * N_ENVIRONMENTS) + 1; // number of PPO steps
};
using LOOP_CORE_CONFIG = rlt::rl::algorithms::ppo::loop::core::Config<T, TI, RNG, ENVIRONMENT, LOOP_CORE_PARAMETERS>;
template <typename NEXT>
struct LOOP_EVAL_PARAMETERS: rlt::rl::loop::steps::evaluation::Parameters<T, TI, NEXT>{
    static constexpr TI EVALUATION_INTERVAL = 4;
    static constexpr TI NUM_EVALUATION_EPISODES = 10;
    static constexpr TI N_EVALUATIONS = NEXT::CORE_PARAMETERS::STEP_LIMIT / EVALUATION_INTERVAL;
};
using LOOP_CONFIG = rlt::rl::loop::steps::evaluation::Config<LOOP_CORE_CONFIG, LOOP_EVAL_PARAMETERS<LOOP_CORE_CONFIG>>;
using LOOP_STATE = typename LOOP_CONFIG::template State<LOOP_CONFIG>;


typedef struct dataToBeSent {
    int encPos;
    float encVel;
} dati;
dati datum;

using namespace std;
int main() {
  DEVICE device;
  TI seed = 1337;
  LOOP_STATE ls;
  rlt::malloc(device, ls);
  rlt::init(device, ls, seed);
  ls.actor_optimizer.parameters.alpha = 1e-3; // increasing the learning rate leads to  faster training of the -v1 environment
  ls.critic_optimizer.parameters.alpha = 1e-3;
  while(!rlt::step(device, ls)){
  }
    
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

    /*
       
   }
 

   // Close the socket.
   closesocket(sockfd);

   // Cleanup Winsock
   WSACleanup();

   return 0;

}
*/
