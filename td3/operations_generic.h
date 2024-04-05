#include <rl_tools/version.h>
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_RL_ALGORITHMS_TD3_LOOP_CORE_OPERATIONS_GENERIC_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_RL_ALGORITHMS_TD3_LOOP_CORE_OPERATIONS_GENERIC_H

#include <rl_tools/rl/algorithms/td3/operations_generic.h>
#include <rl_tools/rl/components/off_policy_runner/operations_generic.h>
#include <rl_tools/rl/utils/evaluation.h>

#include <rl_tools/rl/algorithms/td3/loop/core/config.h>

RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools{
    template <typename DEVICE, typename T_CONFIG>
    void malloc(DEVICE& device, rl::algorithms::td3::loop::core::State<T_CONFIG>& ts){
        malloc(device, ts.actor_critic);
        malloc(device, ts.off_policy_runner);
        malloc(device, ts.critic_batch);
        malloc(device, ts.critic_training_buffers);
        malloc(device, ts.critic_buffers[0]);
        malloc(device, ts.critic_buffers[1]);
        malloc(device, ts.actor_batch);
        malloc(device, ts.actor_training_buffers);
        malloc(device, ts.actor_buffers_eval);
        malloc(device, ts.actor_buffers[0]);
        malloc(device, ts.actor_buffers[1]);
        malloc(device, ts.observations_mean);
        malloc(device, ts.observations_std);
        malloc(device, ts.actor_deterministic_evaluation_buffers);
        for(auto& env: ts.envs){
            rl_tools::malloc(device, env);
        }
    }
    template <typename DEVICE, typename T_CONFIG>
    void init(DEVICE& device, rl::algorithms::td3::loop::core::State<T_CONFIG>& ts, typename T_CONFIG::TI seed = 0){
        using CONFIG = T_CONFIG;
        using T = typename CONFIG::T;

        ts.rng = random::default_engine(typename DEVICE::SPEC::RANDOM{}, seed);

        init(device, ts.actor_critic, ts.rng);

        for(auto& env: ts.envs){
            rl_tools::init(device, env);
        }
        init(device, ts.off_policy_runner, ts.envs);
        set_all(device, ts.observations_mean, 0);
        set_all(device, ts.observations_std, 1);
        ts.off_policy_runner.parameters = rl::components::off_policy_runner::default_parameters<T>;
        init(device);
        init(device, device.logger);
        ts.step = 0;
    }

    template <typename DEVICE, typename T_CONFIG>
    void free(DEVICE& device, rl::algorithms::td3::loop::core::State<T_CONFIG>& ts){
        free(device, ts.critic_batch);
        free(device, ts.critic_training_buffers);
        free(device, ts.actor_batch);
        free(device, ts.actor_training_buffers);
        free(device, ts.off_policy_runner);
        free(device, ts.actor_critic);
        free(device, ts.observations_mean);
        free(device, ts.observations_std);
        for(auto& env: ts.envs){
            rl_tools::free(device, env);
        }
    }

    template <typename DEVICE, typename T_CONFIG>
    bool step(DEVICE& device, rl::algorithms::td3::loop::core::State<T_CONFIG>& ts){
        using CONFIG = T_CONFIG;
	using T = typename CONFIG::T;
        using TI = typename CONFIG::TI;
	using ENVIRONMENT = typename CONFIG::ENVIRONMENT;	

	using namespace std;
	static int count = 0;
        set_step(device, device.logger, ts.step);
        
	bool finished = false;
	
	static typename ENVIRONMENT::State state; 
	static typename ENVIRONMENT::State next_state; 
        int allocated = 0;
	int first = 1;
	static MatrixDynamic<matrix::Specification<T, TI, 1, ENVIRONMENT::ACTION_DIM * 1>> action_full;
        static MatrixDynamic<matrix::Specification<T, TI, 1, ENVIRONMENT::OBSERVATION_DIM>> next_observation, next_observation_normalized, next_observation_privileged;
        static MatrixDynamic<matrix::Specification<T, TI, 1, ENVIRONMENT::OBSERVATION_DIM>> observation, observation_normalized,  observation_privileged;;
	if (allocated == 0){
		malloc(device, next_observation);
        	malloc(device, next_observation_normalized);
        	malloc(device, next_observation_privileged);
 
		malloc(device, observation);
        	malloc(device, observation_normalized);
        	malloc(device, observation_privileged);
		allocated = 1;
	}
        malloc(device, action_full);

	
	//for(int z = 1; z < 2; z++){
		if (first == 1){
			sample_initial_state(device, ts.envs[0], state, ts.rng);
			next_state = state;
			observe(device, ts.envs[0], state, next_observation, ts.rng);
 	        	normalize(device, ts.observations_mean, ts.observations_std, next_observation, next_observation_normalized);
			first = 0;

		}

		state = next_state;
		copy(device, device, next_observation, observation);
		copy(device, device, next_observation_normalized, observation_normalized);

		evaluate(device, ts.actor_critic.actor, observation_normalized, action_full, ts.actor_buffers_eval);
		
		auto action = view(device, action_full, matrix::ViewSpec<1, ENVIRONMENT::ACTION_DIM>{});
		step(device, ts.envs[0], state, action, next_state, ts.rng);
		
		observe(device, ts.envs[0], next_state, next_observation, ts.rng);
 	        normalize(device, ts.observations_mean, ts.observations_std, next_observation, next_observation_normalized);

		T rew = reward(device, ts.envs[0], state, action_full, next_state, ts.rng);
		if (false){
			cout << "State: " << state.theta << " "  << state.theta_dot << " " << endl;
			cout << "Next State: " << next_state.theta << " "  << next_state.theta_dot << " " << endl;
			cout << "Obs: ";
			print(device, observation); 
			cout << "Normalized Observation: ";
			print(device,observation_normalized);
			cout << "Next Obs: ";
			print(device, next_observation); 
			cout << "Next Normalized Observation: ";
			print(device,next_observation_normalized); 
			cout << "Action taken";
			print(device,action);
			cout << "Reward: " << rew;
			
			cout << "____________________________________________________" << endl;
		}
		add(device, ts.off_policy_runner.replay_buffers[0], state, observation, observation_privileged, action, rew, next_state, next_observation, next_observation_privileged, false, 0); 	
//	}
	
        //step(device, ts.off_policy_runner, ts.actor_critic.actor, ts.actor_buffers_eval, ts.rng); //fill devices policy buffer with many elements.
        if(ts.step > CONFIG::CORE_PARAMETERS::N_WARMUP_STEPS){
            if(ts.step % CONFIG::CORE_PARAMETERS::TD3_PARAMETERS::CRITIC_TRAINING_INTERVAL == 0){
                for(int critic_i = 0; critic_i < 2; critic_i++){
                    gather_batch(device, ts.off_policy_runner, ts.critic_batch, ts.rng);
                    target_action_noise(device, ts.actor_critic, ts.critic_training_buffers.target_next_action_noise, ts.rng);
                    train_critic(device, ts.actor_critic, critic_i == 0 ? ts.actor_critic.critic_1 : ts.actor_critic.critic_2, ts.critic_batch, ts.actor_critic.critic_optimizers[critic_i], ts.actor_buffers[critic_i], ts.critic_buffers[critic_i], ts.critic_training_buffers);
                }
            }
            if(ts.step % CONFIG::CORE_PARAMETERS::TD3_PARAMETERS::ACTOR_TRAINING_INTERVAL == 0){
                gather_batch(device, ts.off_policy_runner, ts.actor_batch, ts.rng);
                train_actor(device, ts.actor_critic, ts.actor_batch, ts.actor_critic.actor_optimizer, ts.actor_buffers[0], ts.critic_buffers[0], ts.actor_training_buffers);
            }
            if(ts.step % CONFIG::CORE_PARAMETERS::TD3_PARAMETERS::CRITIC_TARGET_UPDATE_INTERVAL == 0){
                update_critic_targets(device, ts.actor_critic);
            }
            if(ts.step % CONFIG::CORE_PARAMETERS::TD3_PARAMETERS::ACTOR_TARGET_UPDATE_INTERVAL == 0){
                update_actor_target(device, ts.actor_critic);
            }
        }
        ts.step++;
        if(ts.step > CONFIG::CORE_PARAMETERS::STEP_LIMIT){
            return true;
        }
        else{
            return finished;
        }
    }
}
RL_TOOLS_NAMESPACE_WRAPPER_END


#endif
