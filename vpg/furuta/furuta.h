#pragma once
#include <cstdlib>
#include <ctime>
#include <rl_tools/operations/cpu_mux.h>
#include <rl_tools/nn/operations_cpu_mux.h>

#include <rl_tools/operations/cpu.h>
#include <rl_tools/nn/layers/dense/operations_cpu.h>

#include <rl_tools/nn_models/mlp/operations_cpu.h>

namespace rlt = rl_tools;
using T = float;
using DEVICE = rlt::devices::DEVICE_FACTORY<>;
using TI = typename DEVICE::index_t;

constexpr TI BATCH_SIZE = 5000;
constexpr TI INPUT_DIM_MLP = 2;
constexpr TI OUTPUT_DIM_MLP = 1;
constexpr TI NUM_LAYERS = 2;
constexpr TI HIDDEN_DIM = 32;
constexpr auto ACTIVATION_FUNCTION_MLP = rlt::nn::activation_functions::RELU;
constexpr auto OUTPUT_ACTIVATION_FUNCTION_MLP = rlt::nn::activation_functions::IDENTITY;
constexpr TI N_ITERS=50;

using STRUCTURE_SPEC = rlt::nn_models::mlp::StructureSpecification<T, DEVICE::index_t, INPUT_DIM_MLP, OUTPUT_DIM_MLP, NUM_LAYERS, HIDDEN_DIM, ACTIVATION_FUNCTION_MLP, OUTPUT_ACTIVATION_FUNCTION_MLP, 1>; //Final 1 refers to batch size

using OPTIMIZER_SPEC = rlt::nn::optimizers::adam::Specification<T, TI>;
using OPTIMIZER = rlt::nn::optimizers::Adam<OPTIMIZER_SPEC>;
using MODEL_SPEC = rlt::nn_models::mlp::AdamSpecification<STRUCTURE_SPEC>;
using MODEL_TYPE = rlt::nn_models::mlp::NeuralNetworkAdam<MODEL_SPEC>;

typedef struct{
	T theta;
	T theta_dot;
} STATE_VECTOR;

typedef struct{
	T next_theta;
	T next_theta_dot;
	TI done;
	T reward;
} RETURN_VECTOR;

RETURN_VECTOR pendulum(STATE_VECTOR state, T action);
STATE_VECTOR pendulumInitialState();

namespace rlt = rl_tools;
template <typename T>
struct MyFurutaParameters {
    constexpr static T G = 10;
    constexpr static T MAX_SPEED = 8;
    constexpr static T MAX_TORQUE = 2;
    constexpr static T DT = 0.05;
    constexpr static T M = 1;
    constexpr static T L = 1;
    constexpr static T INITIAL_STATE_MIN_ANGLE = -rlt::math::PI<T>;
    constexpr static T INITIAL_STATE_MAX_ANGLE = rlt::math::PI<T>;
    constexpr static T INITIAL_STATE_MIN_SPEED = -1;
    constexpr static T INITIAL_STATE_MAX_SPEED = 1;
};

template <typename T_T, typename T_TI, typename T_PARAMETERS = MyFurutaParameters<T_T>>
struct MyFurutaSpecification{
    using T = T_T;
    using TI = T_TI;
    using PARAMETERS = T_PARAMETERS;
};

template <typename T, typename TI>
struct MyFurutaState{
    static constexpr TI DIM = 2;
    T theta;
    T theta_dot;
};

template <typename T_SPEC>
struct MyFuruta{
    using SPEC = T_SPEC;
    using T = typename SPEC::T;
    using TI = typename SPEC::TI;
    using State = MyFurutaState<T, TI>;
    static constexpr TI OBSERVATION_DIM = 3;
    static constexpr TI OBSERVATION_DIM_PRIVILEGED = OBSERVATION_DIM;
    static constexpr TI ACTION_DIM = 1;
};
