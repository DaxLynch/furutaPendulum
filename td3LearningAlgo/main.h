#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "constants.h"
#include <cstring> // for strerror
#include <cerrno>  // for errno

#include <rl_tools/operations/cpu_mux.h>
#include <rl_tools/nn/operations_cpu_mux.h>
#include "furuta/furuta.h"
#include "furuta/operations_cpu.h"
#include <rl_tools/nn_models/sequential/operations_generic.h>
#include <rl_tools/nn_models/mlp/operations_generic.h>
//#include <rl_tools/rl/algorithms/td3/loop/core/config.h>
//#include <rl_tools/rl/loop/steps/evaluation/config.h>
//#include <rl_tools/rl/loop/steps/timing/config.h>
//#include <rl_tools/rl/algorithms/td3/loop/core/operations_generic.h>
//#include <rl_tools/rl/loop/steps/evaluation/operations_generic.h>
//#include <rl_tools/rl/loop/steps/timing/operations_cpu.h>
#include "temp.txt"
#include "furuta/loop.h"


