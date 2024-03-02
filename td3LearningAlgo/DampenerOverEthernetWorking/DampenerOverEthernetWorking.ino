#include "/home/dax/workingLibAndCPP/defines.h"
#include <EthernetUdp.h>
#include "/home/dax/workingLibAndCPP/constants.h"
#include "FastAccelStepper.h"

EthernetUDP Udp;
IPAddress host(10,0,0,1);

const int stepPinStepper 17
const int dirPinStepper 6
int encoderPin1 = 2;
int encoderPin2 = 3;//standard pin 
volatile long lastEncoded = 0;
volatile long encoderValue = 0;
volatile long lastencoderValue = 0;
volatile long lastMSB = 0;

FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;

bool print = 0;

int encPos[hysteresis];
int i = 0;
float encVel;

int stepsPerEP = 0;
int U = 0;

int pauseAmount = 0; //Min 1
const float amountPausesFL = evalPeriod * TICKS_PER_MS /65535;
const int amountPauses = max(1, amountPausesFL) + 1;
int pauses[amountPauses];
int pauseTracker = 0;

int microsAtLastEval = 0;
int microsAtLastSample = 0;

static struct stepper_command_s cmd;
typedef struct {
  int encoderValue;
  float encVel;
} dati;
dati datum;

void setup()
{

  for (int j = 0; j < 0; j++){
    encPos[i] = 0;
  }
  pinMode(encoderPin1, INPUT); 
  pinMode(encoderPin2, INPUT);
  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on
  attachInterrupt(encoderPin1, updateEncoder, CHANGE); 
  attachInterrupt(encoderPin2, updateEncoder, CHANGE);

  engine.init();
  stepper = engine.stepperConnectToPin(stepPinStepper);
  if (stepper) {
    stepper->setDirectionPin(dirPinStepper);
  }

  cmd.count_up = true;
  encoderValue = 0;

  SerialDebug.begin(115200);


  while (!Serial && millis() < 5000);


  #define USE_THIS_SS_PIN   10    // For other boards


  #if defined(BOARD_NAME)
    ETG_LOGWARN3(F("Board :"), BOARD_NAME, F(", setCsPin:"), USE_THIS_SS_PIN);
  #else
    ETG_LOGWARN1(F("Unknown board setCsPin:"), USE_THIS_SS_PIN);
  #endif

    // For other boards, to change if necessary
    Ethernet.init (10);


    uint16_t index = millis() % NUMBER_OF_MAC;
    
    Ethernet.begin(mac[index],ip,subnet);
    while(Ethernet.linkStatus() != LinkON){
      Serial.println("Ethernet not connected");
    }
    SerialDebug.print(F("Using mac index = "));
    SerialDebug.println(index);

    Serial.print("Connected! IP address: ");
    Serial.println(Ethernet.localIP());

    SerialDebug.print(F("Speed: "));
    SerialDebug.print(Ethernet.speedReport());
    SerialDebug.print(F(", Duplex: "));
    SerialDebug.print(Ethernet.duplexReport());
    
    SerialDebug.print(F(", Link status: "));
    SerialDebug.println(Ethernet.linkReport());
  

    int tempRes = Udp.begin(8081);
    Serial.print("Value for UDP beginning: ");
    Serial.println(tempRes);
}

void loop()
{

if (  (int(micros()) - microsAtLastSample) > (samplingPeriod * 1000) ){ //Sampling period for pos and vel
  microsAtLastSample = micros();
  encPos[i] = encoderValue;
  encVel = float(encPos[i] - encPos[(i + hysteresis - 1)%hysteresis])/(float(samplingPeriod)/1000.0);
  if (encoderValue >= encoderPulses/2){
    encoderValue = -1200 + (encoderValue % (encoderPulses/2));
  } 
  if (encoderValue <= -encoderPulses/2){
    encoderValue = 1200 + (encoderValue % (encoderPulses/2));
  }
  encPos[i] = encoderValue;
  datum.encoderValue = encPos[i];
  datum.encVel = encVel;
  i = (i +1) % hysteresis;
}

int newEntriesAmount = min(32 - stepper->queueEntries(), stepsPerEP * pauseAmount - pauseTracker);
for(int j = 0; j < newEntriesAmount; j++){ // Make sure there are no queue elements to add
  //Serial.println("Here");
  int res = 0;
  cmd.ticks = pauses[pauseTracker];
  if (pauseTracker){
    cmd.steps = 1;
  } else { //Your on a 0
    if(pauseAmount == 1){
      cmd.steps = min(stepsPerEP, 255);
      stepsPerEP -= cmd.steps;
    } else {
      cmd.steps = 1;
      stepsPerEP--;
    }
    
  }
  pauseTracker = (pauseTracker + 1) % pauseAmount;
  res = stepper->addQueueEntry(&cmd);
  if (res != 0){
    Serial.print(res);
    Serial.println(" : Queue Entry Error");
    Serial.print(cmd.steps);
    Serial.println(" : steps");
    Serial.print(cmd.ticks);
    Serial.println(" : ticks");
  }
}

if( (int(micros()) - microsAtLastEval) > ( (evalPeriod - changePeriod)*1000)  ){
  //Serial.print("Ticks in Queue: "); Serial.println(stepper->ticksInQueue()); //should be 1 or 0 entries or approx 2 ms in queue
  if(false){
    Serial.print(stepper->queueEntries());
    Serial.println(": Queue entries. This should only be 1 or 0;");
    Serial.print("Ticks in Queue: "); Serial.println(stepper->ticksInQueue());
    Serial.println( (int(micros()) - microsAtLastEval)  ); 
    Serial.print("Ticks in Queue: ");
    Serial.println(stepper->ticksInQueue());//*21 + (int(micros()) - microsAtLastEval));
    Serial.println( (int(micros()) - microsAtLastEval)  ); 
  }
  if (U < 0){
    cmd.count_up = true;
  } else {
    cmd.count_up = false;
  }
  if ((U < 1) && (U > -1)){
    stepsPerEP = 0;
    pauseAmount = 1;
    pauseTracker = 0;
  } else {
    int nomTicks = abs(EPTPMS/U);
    
    if (nomTicks < 4200){
      pauses[0] = 4200;
      pauseAmount = 1;
      pauseTracker = 0;
      stepsPerEP = EPTPMS/4200;
    } else if (nomTicks < 65536){
      pauses[0] = nomTicks;
      pauseAmount = 1;
      pauseTracker = 0;
      stepsPerEP = abs(U);
    } else if (nomTicks <= EPTPMS){    
      int rem = nomTicks % 65536;
      if (rem < 4200){
        pauses[0] = 4200;
        pauseAmount =  nomTicks/65536 +1;
        pauseTracker = 0;
        stepsPerEP = abs(U);
        for(int j = 0; j < nomTicks/65536; j++){
          pauses[j + 1] = 65535;
        }
        
        pauses[1] -= (4200 - rem - 1);
        
      } else {
        pauses[0] = rem;
        pauseAmount =  nomTicks/65536 +1;
        pauseTracker = 0;
        stepsPerEP = abs(U);
        for(int j = 0; j < nomTicks/65536; j++){
          pauses[j + 1] = 65535;
        }
      }  
    } else {
      stepsPerEP = 0;
      pauseAmount = 1;
      pauseTracker = 0;
    }
    
    if(print){
      Serial.print(nomTicks);
      Serial.println(": nomTicks");
      Serial.print(pauseAmount);
      Serial.println(": pauseAmount");
      Serial.print(stepsPerEP);
      Serial.println(": stepsPerEP");
      for(int i = 0; i < pauseAmount + 1; i++){
          Serial.print(pauses[i]);
          Serial.println(" : pauses");
      }
    }
  }
  
  microsAtLastEval = microsAtLastEval + (evalPeriod * 1000);
  //Serial.print("micros at last eval: "); Serial.println(microsAtLastEval);
}
int packetSize = Udp.parsePacket();
if(packetSize){
  char tempBuf[32];
  Udp.read((char*)&U, packetSize);
}

Udp.beginPacket(host,8081);
Udp.write((byte*)&datum,sizeof(dati));
Udp.endPacket();


}
void updateEncoder(){

  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time
  
}
