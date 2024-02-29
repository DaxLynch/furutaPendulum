#include "math.h"
#ifndef TICKS_PER_S
#define TICKS_PER_S 21000000
#endif

const float totalGain = 1;
const float Kc = -1.5;
const float Td = 0;

float minVel = 100;//encoderPulses/second 2400 is 1 rp
const int hysteresis = 3;
const int hysteresisCutoff = 40;
const int evalPeriod = 25; //Time in ms per PID loop
const int changePeriod = 1; //Should be in ms
const int TICKS_PER_MS = TICKS_PER_S / 1000;
const int EPTPMS = evalPeriod * TICKS_PER_MS;

const float proportionalTimeAdjustment = sqrt(1000.0 / evalPeriod);

const int samplingPeriod = 10;
const int microsteppingPulses = 6400;
const int encoderPulses = 2400;
