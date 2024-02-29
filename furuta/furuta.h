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
