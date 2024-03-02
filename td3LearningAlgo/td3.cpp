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
	int here = 1;
	while(!rlt::step(device, ts)){
		using CONFIG = LOOP_CONFIG;
		using namespace rlt;
		using TI = typename CONFIG::TI;
		using PARAMETERS = typename CONFIG::EVALUATION_PARAMETERS;
		using STATE = rl::loop::steps::evaluation::State<CONFIG>;
		if constexpr(PARAMETERS::DETERMINISTIC_EVALUATION == true){

		    TI evaluation_index = ts.step / PARAMETERS::EVALUATION_INTERVAL;
		    if(ts.step % PARAMETERS::EVALUATION_INTERVAL == 0 && evaluation_index < PARAMETERS::N_EVALUATIONS){
			auto result = evaluate(device, ts.env_eval, ts.ui, get_actor(ts), rl::utils::evaluation::Specification<PARAMETERS::NUM_EVALUATION_EPISODES, CONFIG::EVALUATION_PARAMETERS::EPISODE_STEP_LIMIT>(), ts.observations_mean, ts.observations_std, ts.actor_deterministic_evaluation_buffers, ts.rng_eval, false);
			log(device, device.logger, "Step: ", ts.step, "/", CONFIG::CORE_PARAMETERS::STEP_LIMIT, " Mean return: ", result.returns_mean);
			add_scalar(device, device.logger, "evaluation/return/mean", result.returns_mean);
			add_scalar(device, device.logger, "evaluation/return/std", result.returns_std);
			ts.evaluation_results[evaluation_index] = result;
		    }
		}
		here = step(device, static_cast<typename STATE::NEXT&>(ts));
	if(ts.step == 5000){
	  std::cout << "steppin yourself > callbacks 'n' hooks: " << ts.step << std::endl;
	}
	}
	rlt::free(device,ts);
	rlt::free(device);



	return 0;
}

