//#define RL_TOOLS_BACKEND_ENABLE_OPENBLAS

#include <chrono>

#include "furuta/furuta.h"

using namespace std;
auto now = chrono::high_resolution_clock::now();

auto timeMillis = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
auto rng = rlt::random::default_engine(typename DEVICE::SPEC::RANDOM{}, timeMillis);
DEVICE device;

STATE_VECTOR pendulumInitialState(){
	T theta     = rlt::random::uniform_real_distribution(typename DEVICE::SPEC::RANDOM(), -3.14159, 3.14159, rng);
	T theta_dot = rlt::random::uniform_real_distribution(typename DEVICE::SPEC::RANDOM(), 5*-3.14159, 5*3.14159, rng);
	return STATE_VECTOR{theta, theta_dot};
}
RETURN_VECTOR pendulum(STATE_VECTOR state, T action){
        const T g = -9.8;
        const T m = 1;
        const T l = 1;
        const T dt = .025;
        //u = clip(u, -PARAMS::MAX_TORQUE, PARAMS::MAX_TORQUE); //Maybe add this
 	TI done = 0;
        T next_theta_dot = state.theta_dot + (3 * g / (2 * l) * rlt::math::sin(device.math, state.theta) + 3.0 / (m * l * l) * action) * dt;
	T next_theta = state.theta + next_theta_dot * dt;
	if ((rlt::math::abs(device.math, next_theta) < .1) && (next_theta_dot < .1)){
		done = 1;
	}
	T angle_norm = ((next_theta + 3.14159)/(2*3.14159));
	T rew = angle_norm*angle_norm + 0.1 * .2*next_theta_dot*next_theta_dot;
	return RETURN_VECTOR{next_theta, next_theta_dot, done, rew};
}


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
rlt::MatrixDynamic<rlt::matrix::Specification<T, TI, 1, OUTPUT_DIM_MLP>> state_output;
rlt::MatrixDynamic<rlt::matrix::Specification<T, TI, BATCH_SIZE, 1>> dones;
rlt::MatrixDynamic<rlt::matrix::Specification<T, TI, BATCH_SIZE, 1>> actions;
rlt::MatrixDynamic<rlt::matrix::Specification<T, TI, BATCH_SIZE, 1>> rews;
rlt::MatrixDynamic<rlt::matrix::Specification<T, TI, BATCH_SIZE, OUTPUT_DIM_MLP>> d_output_mlp;
rlt::malloc(device, states);
rlt::malloc(device, state_input);
rlt::malloc(device, state_output);
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
//	cout << state.theta << " " << state.theta_dot;
	set(state_input, 0,0, state.theta);
	set(state_input, 0,1, state.theta_dot);

	set(states, z, 0, state.theta);
	set(states, z, 1, state.theta_dot);
	rlt::evaluate(device, model, state_input, state_output);
	action  = get(state_output, 0, 0);
	cout << "Action:" << action << endl; 
	set(actions, z, 0, action);
	returnV = pendulum(state,action);
	state.theta = returnV.next_theta;
	state.theta_dot = returnV.next_theta_dot;
	set(rews, z, 0, returnV.reward);
	set(dones, z, 0, returnV.done);

}
int print = 1;
if (print){
	cout << "actions" << endl;
	rlt::print(device, actions);	

	cout << "rewards" << endl;
	rlt::print(device, rews);	

	cout << "dones" << endl;
	rlt::print(device, dones);	

	cout << "inputs" << endl;
	rlt::print(device, states);	
}
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration<double>(end - start);
std::cout << "Time to run inference: " << duration.count() << " seconds" << std::endl;
cout << "kys" << endl;
}

