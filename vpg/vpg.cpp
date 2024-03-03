//#define RL_TOOLS_BACKEND_ENABLE_OPENBLAS

#include <chrono>

#include "furuta/furuta.h"

using namespace std;

auto rng = rlt::random::default_engine(typename DEVICE::SPEC::RANDOM{}, 1);
DEVICE device;


RETURN_VECTOR pendulum(STATE_VECTOR state, T action){
	T theta = rlt::random::uniform_real_distribution(typename DEVICE::SPEC::RANDOM(), -3.141419, 3.1415, rng);
	T theta_dot = rlt::random::uniform_real_distribution(typename DEVICE::SPEC::RANDOM(), 5*-3.141419, 5*3.1415, rng);
	return RETURN_VECTOR{theta,theta_dot,0,4};	
};
STATE_VECTOR pendulumInitialState(){
	T theta = rlt::random::uniform_real_distribution(typename DEVICE::SPEC::RANDOM(), -3.141419, 3.1415, rng);
	T theta_dot = rlt::random::uniform_real_distribution(typename DEVICE::SPEC::RANDOM(), 5*-3.141419, 5*3.1415, rng);
	return STATE_VECTOR{theta,theta_dot};
};



int main() {
OPTIMIZER optimizer;
MODEL_TYPE model;
typename MODEL_TYPE::Buffer<BATCH_SIZE> buffer;

rlt::malloc(device, model);
rlt::init_weights(device, model, rng); // recursively initializes all layers using kaiming initialization
rlt::zero_gradient(device, model); // recursively zeros all gradients in the layers
rlt::reset_optimizer_state(device, optimizer, model);
rlt::malloc(device, buffer);

rlt::MatrixDynamic<rlt::matrix::Specification<T, TI, BATCH_SIZE, INPUT_DIM_MLP>> states, d_input_mlp;
rlt::MatrixDynamic<rlt::matrix::Specification<T, TI, 1, INPUT_DIM_MLP>> state_input;
rlt::MatrixDynamic<rlt::matrix::Specification<T, TI, BATCH_SIZE, 1>> dones;
rlt::MatrixDynamic<rlt::matrix::Specification<T, TI, BATCH_SIZE, 1>> actions;
rlt::MatrixDynamic<rlt::matrix::Specification<T, TI, BATCH_SIZE, 1>> rews;
rlt::MatrixDynamic<rlt::matrix::Specification<T, TI, BATCH_SIZE, OUTPUT_DIM_MLP>> d_output_mlp;
rlt::malloc(device, states);
rlt::malloc(device, state_input);
rlt::malloc(device, dones);
rlt::malloc(device, actions);
rlt::malloc(device, rews);

rlt::malloc(device, d_input_mlp);
rlt::malloc(device, d_output_mlp);

auto start = std::chrono::high_resolution_clock::now();
STATE_VECTOR state;
RETURN_VECTOR returnV; 
T action;
for(TI z = 0; z < BATCH_SIZE; z++){
	if ((z == 0) || (returnV.done)){
		state = pendulumInitialState();
	}
	cout << state.theta << " " << state.theta_dot;
	set(state_input, 0,0, state.theta);
	set(state_input, 0,1, state.theta_dot);

	set(states, z, 0, state.theta);
	set(states, z, 1, state.theta_dot);
	rlt::forward(device, model, state_input);
	action  = get(model.output_layer.output, 0, 0);
	cout << "Action:" << action << endl; 
	set(actions, z, 0, action);
	returnV = pendulum(state,action);
	state.theta = returnV.next_theta;
	state.theta_dot = returnV.next_theta_dot;
//	set(rews, z, 0, returnV.reward);
//	set(dones, z, 0, returnV.done);
}

auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration<double>(end - start);
std::cout << "Time to run inference: " << duration.count() << " seconds" << std::endl;
cout << "kys" << endl;
}

