//Program Details: 
// Machine Switch Off And Increased Decrease with RPM increased event and Switched Off Event
//Version 1 Dated 01062016

#include "SIM.h"
#include "RPMS.h"
#include <SoftwareSerial.h>

//#define USE_ALTERNATOR
//#define USE_SOLENOID
#define USE_MOTOR

const byte SEN_RPM=2;
const byte REL_ALTERNATOR=8;
const byte RX=4;
const byte TX=3;

SoftwareSerial s1(RX,TX);

SIM sim1(&Serial, &s1);
RPMS rpmSensor(SEN_RPM,REL_ALTERNATOR,&Serial);

void cannotTurnOffMachine()
{
      Serial.println("Cannot Turn Off");  
      sim1.registerEvent('Z', true, false); 
}

bool gotMachineOffCommand;
char lastImmediateEvent;

#ifdef USE_SOLENOID

  #include "SOLENOID.h"
  #define REL_SOLENOID 9
  SOLENOID solenoid1(REL_SOLENOID,cannotTurnOffMachine);
  
#else

  #ifdef USE_MOTOR
    #include "SMOTOR_R.h"
    #define REL_SLOW 9
    #define REL_FAST 10

    #define PWR_SIGM 25
    #define SEN_MPOS A0

    #define MinimumMotorVoltage 0.30
    #define MaximumMotorVoltage 1.70

    SMOTOR_R smotor1(REL_SLOW, REL_FAST, PWR_SIGM, SEN_MPOS, MinimumMotorVoltage, MaximumMotorVoltage, &Serial,registerRPMIncreased,registerRPMDecreased,cannotTurnOffMachine);

    char increaseRPM()
    {
      if (smotor1.increaseRPM())
      {
        return 'D';
      }
      else
      {
        Serial.println("LIMIT REACHED");
        return 'L';
      }
    }
    
    void registerRPMIncreased()
    {
      sim1.registerEvent('I', false, false);
    }
    
    char decreaseRPM()
    {
      if (smotor1.decreaseRPM())
      {
        return 'D';
      }
      else
      {
        Serial.println("LIMIT REACHED");
        return 'L';
      }
    }
    
    void registerRPMDecreased()
    {
      sim1.registerEvent('D', false, false);
    }
  #endif
#endif

void gotImmediateResponse(bool temp)
{
  if (temp && rpmSensor.machineOn)
  {
    Serial.print("Machine Switch Off :  ");
    Serial.println(lastImmediateEvent);
    stopMachine();
  }
  else if(!temp)
  {
    rpmSensor.discardRPMEvent();
  }
  lastImmediateEvent = 'N';
}

char stopMachine()
{
  Serial.println("STOP ");
  if (rpmSensor.getRPM() != 0 && rpmSensor.machineOn)
  {
      #ifdef USE_SOLENOID
        solenoid1.turnOnSolenoid();
        gotMachineOffCommand = true;
        Serial.println("DONE STOP ");
        return 'D';
      #else
        #ifdef USE_MOTOR
          if (smotor1.switchOff())
          {
            gotMachineOffCommand = true;
            Serial.println("DONE STOP ");
            return 'D';
          }
          else
          {
            Serial.println("LIMIT REACHED ");
            return 'L';
          }
        #endif 
      #endif
  }
  else
  {
    Serial.println("machine OFF ");
    return 'O';
  }
}

void rpmIncreased()   //triggers when machine's RPM get increased beyond limit 
{
    Serial.println("RPM INCREASED EVENT");  
    sim1.registerEvent('F', true, true);
    lastImmediateEvent = 'F';
}

void machineSwitchedOn()
{
    lastImmediateEvent='N';
    sim1.registerEvent('C', false, false);  
}

void machineSwitchedOff()
{
  if (gotMachineOffCommand)
  {
    #ifdef USE_SOLENOID
      solenoid1.turnOffSolenoid();
    #else
      #ifdef USE_MOTOR
        smotor1.backOff();
      #endif
    #endif
  }
  rpmSensor.discardRPMEvent();
  lastImmediateEvent='N';
  sim1.registerEvent('O', false, false);
}

/*ISR(TIMER3_OVF_vect)
{
  //TIMSK3&=~(1<<TOIE3);
  //TIMSK3 = (TIMSK3 & 0xFE);   //disable the Timer INterrupt
  //self1.NoRPM();

  //Serial.println("Got A Timer OverFLow Event..");
  
  //digitalWrite(LED, !digitalRead(LED));
  //  TCNT3 = 0x48E4;       //initialize the counter
}*/

void FIVR_RPM()
{
    rpmSensor.lastrise = rpmSensor.currentrise;
    rpmSensor.currentrise = millis();
    rpmSensor.gotTrigger = true;

  /*int x = digitalRead(SEN_RPM);
  if (x == 0)
  {
    self1.lastrise = self1.currentrise;
    self1.currentrise = millis();
    self1.gotTrigger = true;

    //TIMSK3 |= (1 << TOIE3); //enable timer 3 
    //TCNT3 = 0x48E4;   //intialize the counter for timer 3
    //Serial.println(TCNT3);
    //self1.setTimer();
  }*/
}

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  //Serial1.begin(4800);    //SIM
  if (!sim1.initialize())
  {
    Serial.println("Problem in initializing SIM");
  }
  sim1.setCallBackFunctions(gotImmediateResponse);
  sim1.setDTMFFunctions(increaseRPM,decreaseRPM,stopMachine);

  rpmSensor.setCallBackFunctions(rpmIncreased, machineSwitchedOff,machineSwitchedOn);
  gotMachineOffCommand = false;

  attachInterrupt(digitalPinToInterrupt(SEN_RPM), FIVR_RPM, RISING);
  //FIVR_RPM();
}

String str;

void loop() {
/*  if (Serial.available() > 0)
  {
    str = Serial.readStringUntil('\n');
    if (str == "S\r")
      startMachine();
    else if (str == "I\r")
      increaseRPM();
    //                    smotor1.increaseRPM();
    else if (str == "D\r")
      decreaseRPM();
    //smotor1.decreaseRPM();
    else if (str == "A\r")
      stopMachine();
    //smotor1.switchOff();
    else if(str=="R\r")
      Serial.println(self1.requestMotorReading());
  }
  */
  rpmSensor.update();
  sim1.update();
  #ifdef USE_SOLENOID
    solenoid1.update();
  #else 
    #ifdef USE_MOTOR
      smotor1.update();
    #endif
  #endif
}
