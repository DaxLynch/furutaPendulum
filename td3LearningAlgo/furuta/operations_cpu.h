typedef struct dataToBeSent {
    int encPos;
    float encVel;
} dati;
dati datum;

namespace rlt = rl_tools;
template <typename T>
T clip(T x, T min, T max){
    x = x < min ? min : (x > max ? max : x);
    return x;
}
template <typename DEVICE, typename T>
T f_mod_python(const DEVICE& dev, T a, T b){
    return a - b * rlt::math::floor(dev, a / b);
}

template <typename DEVICE, typename T>
T angle_normalize(const DEVICE& dev, T x){
    return f_mod_python(dev, (x + rlt::math::PI<T>), (2 * rlt::math::PI<T>)) - rlt::math::PI<T>;
}

namespace rl_tools{
	int totalCounts = 0;
	int inited = 0;
	int U = 0;
	int result = 0;
    	int sockfd;
	sockaddr_in addr;
	sockaddr_in send_addr;
    template<typename DEVICE, typename SPEC>
    static void malloc(DEVICE& device, const MyFuruta<SPEC>& env){
	std::cout << "We in dis bithc" ;
        if (inited == 0){
		inited = 1;
		  sockfd = socket(AF_INET, SOCK_DGRAM, 0);		
		  if (sockfd == -1) {
		      perror("socket failed");
		  }

		  // Bind the socket to a local address.
		  addr.sin_family = AF_INET;
		  addr.sin_port = htons(8081);
		  addr.sin_addr.s_addr = INADDR_ANY;
		  if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) == -1) {
		      perror("bind failed");
		      close(sockfd);
		  }

		  send_addr.sin_family = AF_INET;
		  send_addr.sin_port = htons(8081);
		  const char* addie = "10.0.0.2";
		  send_addr.sin_addr.s_addr = inet_addr(addie);


        }
    }
    template<typename DEVICE, typename SPEC>
    static void free(DEVICE& device, const MyFuruta<SPEC>& env){
        std::cout << std::endl << "freed the envo" << std::endl;
    } 
    template<typename DEVICE, typename SPEC>
    static void init(DEVICE& device, const MyFuruta<SPEC>& env){
	std::cout << "Initting the critics" << std::endl;
	//This gets called by each critic I think. 

}
    template<typename DEVICE, typename SPEC, typename RNG>
    static void sample_initial_state(DEVICE& device, const MyFuruta<SPEC>& env, typename MyFuruta<SPEC>::State& state, RNG& rng){
	//std::cout << "Sampling Initial State" << std::endl;
//	totalCounts++;
        state.theta     = random::uniform_real_distribution(typename DEVICE::SPEC::RANDOM(), SPEC::PARAMETERS::INITIAL_STATE_MIN_ANGLE, SPEC::PARAMETERS::INITIAL_STATE_MAX_ANGLE, rng);
        state.theta_dot = random::uniform_real_distribution(typename DEVICE::SPEC::RANDOM(), SPEC::PARAMETERS::INITIAL_STATE_MIN_SPEED, SPEC::PARAMETERS::INITIAL_STATE_MAX_SPEED, rng);
    }
    template<typename DEVICE, typename SPEC>
    static void initial_state(DEVICE& device, const MyFuruta<SPEC>& env, typename MyFuruta<SPEC>::State& state){
        state.theta = -rlt::math::PI<typename SPEC::T>;
        state.theta_dot = 0;
    }
    template<typename DEVICE, typename SPEC, typename ACTION_SPEC, typename RNG>
    typename SPEC::T step(DEVICE& device, const MyFuruta<SPEC>& env, const typename MyFuruta<SPEC>::State& state, const Matrix<ACTION_SPEC>& action, typename MyFuruta<SPEC>::State& next_state, RNG& rng) {
        static_assert(ACTION_SPEC::ROWS == 1);
        static_assert(ACTION_SPEC::COLS == 1);
        typedef typename SPEC::T T;
        typedef typename SPEC::PARAMETERS PARAMS;
	//What if the state included the time, and the did the action, waited until eval was over,
        T u_normalised = get(action, 0, 0); //Action not based on current state 
   
//---
        T newthdot = state.theta_dot + (3 * g / (2 * l) * rlt::math::sin(device.math, state.theta) + 3.0 / (m * l * l) * u) * dt;
        newthdot = clip(newthdot, -PARAMS::MAX_SPEED, PARAMS::MAX_SPEED);
        T newth = state.theta + newthdot * dt;
//-- All of this is the is wait eval period and return angle, but in my case it actually changes state, in this case it doesnt.
        next_state.theta = newth;
        next_state.theta_dot = newthdot;
        return SPEC::PARAMETERS::DT;
    }
    template<typename DEVICE, typename SPEC, typename ACTION_SPEC, typename RNG>
    static typename SPEC::T reward(DEVICE& device, const MyFuruta<SPEC>& env, const typename MyFuruta<SPEC>::State& state, const Matrix<ACTION_SPEC>& action, const typename MyFuruta<SPEC>::State& next_state, RNG& rng){
        typedef typename SPEC::T T;
        T angle_norm = angle_normalize(device.math, state.theta);
        T u_normalised = get(action, 0, 0);
        T u = SPEC::PARAMETERS::MAX_TORQUE * u_normalised;
        T costs = angle_norm * angle_norm + 0.1 * state.theta_dot * state.theta_dot + 0.001 * (u * u);
        return -costs;
    }

    template<typename DEVICE, typename SPEC, typename OBS_SPEC, typename RNG>
    static void observe(DEVICE& device, const MyFuruta<SPEC>& env, const typename MyFuruta<SPEC>::State& state, Matrix<OBS_SPEC>& observation, RNG& rng){
        static_assert(OBS_SPEC::ROWS == 1);
        static_assert(OBS_SPEC::COLS == 3);
        typedef typename SPEC::T T;
        set(observation, 0, 0, rlt::math::cos(device.math, state.theta));
        set(observation, 0, 1, rlt::math::sin(device.math, state.theta));
        set(observation, 0, 2, state.theta_dot);
    }
    template<typename DEVICE, typename SPEC, typename RNG>
    RL_TOOLS_FUNCTION_PLACEMENT static bool terminated(DEVICE& device, const MyFuruta<SPEC>& env, const typename MyFuruta<SPEC>::State state, RNG& rng){
        using T = typename SPEC::T;
        return false;
    }
}
