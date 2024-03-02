//#define RL_TOOLS_BACKEND_ENABLE_OPENBLAS

#include <chrono>

#include <rl_tools/operations/cpu_mux.h>
#include <rl_tools/nn/operations_cpu_mux.h>

#include <rl_tools/operations/cpu.h>
#include <rl_tools/nn/layers/dense/operations_cpu.h>

#include <rl_tools/nn_models/mlp/operations_cpu.h>


#include "furuta/furuta.h"

namespace rlt = rl_tools;
using namespace std;

using T = float;
using DEVICE = rlt::devices::DEVICE_FACTORY<>;
using TI = typename DEVICE::index_t;
auto rng = rlt::random::default_engine(typename DEVICE::SPEC::RANDOM{}, 1);
DEVICE device;

constexpr TI BATCH_SIZE = 5000;
constexpr TI INPUT_DIM_MLP = 2;
constexpr TI OUTPUT_DIM_MLP = 1;
constexpr TI NUM_LAYERS = 1;
constexpr TI HIDDEN_DIM = 32;
constexpr auto ACTIVATION_FUNCTION_MLP = rlt::nn::activation_functions::RELU;
constexpr auto OUTPUT_ACTIVATION_FUNCTION_MLP = rlt::nn::activation_functions::IDENTITY;
using N_ITERS=50;

using STRUCTURE_SPEC = rlt::nn_models::mlp::StructureSpecification<T, DEVICE::index_t, INPUT_DIM_MLP, OUTPUT_DIM_MLP, NUM_LAYERS, HIDDEN_DIM, ACTIVATION_FUNCTION_MLP, OUTPUT_ACTIVATION_FUNCTION_MLP, BATCH_SIZE>;

using OPTIMIZER_SPEC = rlt::nn::optimizers::adam::Specification<T, TI>;
using OPTIMIZER = rlt::nn::optimizers::Adam<OPTIMIZER_SPEC>;
using MODEL_SPEC = rlt::nn_models::mlp::AdamSpecification<STRUCTURE_SPEC>;
using MODEL_TYPE = rlt::nn_models::mlp::NeuralNetworkAdam<MODEL_SPEC>;

int main() {
OPTIMIZER optimizer;
MODEL_TYPE model;
typename MODEL_TYPE::Buffer<BATCH_SIZE> buffer;

rlt::malloc(device, model);
rlt::init_weights(device, model, rng); // recursively initializes all layers using kaiming initialization
rlt::zero_gradient(device, model); // recursively zeros all gradients in the layers
rlt::reset_optimizer_state(device, optimizer, model);
rlt::malloc(device, buffer);

rlt::MatrixDynamic<rlt::matrix::Specification<T, TI, BATCH_SIZE, INPUT_DIM_MLP>> input_mlp, d_input_mlp;
rlt::MatrixDynamic<rlt::matrix::Specification<T, TI, BATCH_SIZE, OUTPUT_DIM_MLP>> d_output_mlp;
rlt::malloc(device, input_mlp);
rlt::malloc(device, d_input_mlp);
rlt::malloc(device, d_output_mlp);

rlt::randn(device, input_mlp, rng);
rlt::forward(device, model, input_mlp);
T output_value = get(model.output_layer.output, 0, 0);

T target_output_value = 1;
T error = target_output_value - output_value;
rlt::set(d_output_mlp, 0, 0, -error);
rlt::backward(device, model, input_mlp, d_output_mlp, buffer);
rlt::step(device, optimizer, model);

rlt::forward(device, model, input_mlp);
auto start = std::chrono::high_resolution_clock::now();
for(TI i=0; i < N_ITERS; i++){
	//For loop fill the input batch with states, and actions
	//Alocate input state
	for(TI z = 0; x < BATCH_SIZE; z++){
		//add traj state
		rlt::forward(device, model, input_mlp);
		T action = get(model.output_layer.output, 0, 0);
		next_theta, next_theta_dot, done = pendulum(state,action);
		
	}
	//Compute rewards to go
	
	rlt::zero_gradient(device, model);
	T mse = 0;
	for(TI batch_i=0; batch_i < 32; batch_i++){
		rlt::randn(device, input_mlp, rng); //Provide input from pendulum
		rlt::forward(device, model, input_mlp);
		T output_value = get(model.output_layer.output, 0, 0);
		T target_output_value = rlt::max(device, input_mlp);
		T error = target_output_value - output_value;
	
		rlt::set(d_output_mlp, 0, 0, -error);
		rlt::backward(device, model, input_mlp, d_output_mlp, buffer);
		mse += error * error;
	}
	rlt::step(device, optimizer, model);
	if(i % 1000 == 0)
	std::cout << "Squared error: " << mse/32 << std::endl;
}
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration<double>(end - start);
std::cout << "Time to run inference: " << duration.count() << " seconds" << std::endl;

set(input_mlp, 0, 0, -0.1);
set(input_mlp, 0, 1, +0.5);
rlt::forward(device, model, input_mlp);
cout << rlt::get(model.output_layer.output, 0, 0);

cout << "kys" << endl;
}

