//Program Details: 
// Machine Switch Off And Increased Decrease with RPM increased event and Switched Off Event
//Version 2 Dated 22072016

//#include <avr/sleep.h>
#include "SIM.h"
#include "RPMS.h"
#include "SMOTOR_R.h"
#include "S_EEPROM.h"
#include "BATTERY.h"
#include "TEMP.h"
#include "Definitions.h"

#define SIM_SLEEP_PIN 8
S_EEPROM eeprom1;

bool gotMachineOffCommand;
char lastImmediateEvent;
bool inform;
bool sensorInit=false;
bool initialized=false;

bool securityAlarmed=false;
bool gotCannotTurnOffMachineEvent=false;
bool gotRPMIncreasedEvent=false;
bool gotSwitchedOffEvent=false;
bool gotStartedEvent=false;

void registerRPMIncreased();
void registerRPMDecreased();
void cannotTurnOffMachine();
void motorCommandStatus(bool);
void informSIM(char);
void triggerStopMachine(bool b=false);

#ifndef disable_debug
  SoftwareSerial s1(5,6);
  #ifdef software_SIM
    SIM sim1(&Serial,&s1);
    RPMS rpmSensor1(&Serial);
    SMOTOR_R smotor1(&Serial,registerRPMIncreased, registerRPMDecreased,cannotTurnOffMachine,motorCommandStatus);
    BATTERY batteryLevel(&Serial);
    TEMP temp1;
    HardwareSerial* USART1=&Serial;
  #else
    SIM sim1(&s1,&Serial);
    RPMS rpmSensor1(&s1);
    SMOTOR_R smotor1(&s1,registerRPMIncreased, registerRPMDecreased,cannotTurnOffMachine,motorCommandStatus);
    BATTERY batteryLevel(&s1);
    TEMP temp1;
    SoftwareSerial* USART1=&s1;
  #endif
#else
  SIM sim1(&Serial);
  RPMS rpmSensor1(&s1);
  BATTERY batteryLevel;
  SMOTOR_R smotor1(registerRPMIncreased, registerRPMDecreased,cannotTurnOffMachine,motorCommandStatus);
  TEMP temp1;
#endif  

void gotImmediateResponse(bool temp)
{
  if (temp && eeprom1.machineOn)
  {
    #ifndef disable_debug
        Serial.print("M Off I Event:");
        Serial.println(lastImmediateEvent);
    #endif
    triggerStopMachine();
  }
  lastImmediateEvent = 'N';
}

void cannotTurnOffMachine()
{
  #ifndef disable_debug
    USART1->println("Cannot Off");  
  #endif
  gotCannotTurnOffMachineEvent=true;
}

void reportCannotTurnOffMachine()
{
  if(sim1.registerEvent('Z', true, false))
    gotCannotTurnOffMachineEvent=false;
}

void lowBattery()
{
  #ifndef disable_debug
    Serial.println("Low Battery");
  #endif
  
  if(sim1.registerEvent('B',true,false))
  {
    batteryLevel.checkedAlarm();
  }
}

unsigned short int getRPM()
{
  return rpmSensor1.getRPM();
}

void reportSecurityBreach()
{
  if(sim1.registerEvent('Y',true,false))
  {
    USART1->print("SEC");
    USART1->println(" Event");
    securityAlarmed=false;
  }
}

void maxTempLimitReached()
{
  if(eeprom1.machineOn)
  {
    if(sim1.registerEvent('T', true,true))
    {
      lastImmediateEvent = 'T';
      temp1.checkedAlarm();
    }
  }
  else
    temp1.checkedAlarm(true);
}

void triggerIncreaseRPM(byte steps=1)
{
  #ifndef disable_debug
    USART1->print("INC");
    USART1->println("RPM");
  #endif  
  smotor1.increaseRPM(steps);
}

void triggerDecreaseRPM(byte steps=1)
{
  #ifndef disable_debug
    USART1->print("DEC");
    USART1->println("RPM");
  #endif  
  smotor1.decreaseRPM(steps);
}

void triggerStopMachine(bool b)
{
  inform=b;
  #ifndef disable_debug
    USART1->println("STOP");
  #endif
  if(eeprom1.machineOn)
  {
    smotor1.switchOff();
  }
  else
  {
  #ifndef disable_debug
    USART1->println("OFF ");
  #endif  
    if(inform)
      informSIM('O');
  }
}

void informSIM(char b)
{
    sim1.speedMotorStatus(b);
    inform=false;
}

void motorCommandStatus(bool motorStatus)   //inc dec stopMachine command Status
{    
      if(smotor1.checkCurrentOperation()=='S')
      {
        if(motorStatus)
          gotMachineOffCommand=true;

        if(!inform)
          return;
      }

    if(motorStatus)
      informSIM('D');
    else
      informSIM('L');
}



void registerRPMIncreased()
{
  //sim1.registerEvent('I', false, false);
}

void registerRPMDecreased()
{
//  sim1.registerEvent('D', false, false);
}


void rpmIncreased()
{
  #ifndef disable_debug
    USART1->print("RPM ");
    USART1->println("INC EVENT");  
  #endif  
    gotRPMIncreasedEvent=true;
}

void reportRPMIncreasedEvent()
{
  if(sim1.registerEvent('F', true, true))
  {
    lastImmediateEvent = 'F';
    gotRPMIncreasedEvent=false;
  }
}

void machineSwitchedOn()
{
  //lastImmediateEvent='N';
  gotStartedEvent=true;
}

void reportStartEvent()
{
  if(sim1.registerEvent('C', true, false))
    gotStartedEvent=false;
}

void machineSwitchedOff()
{
  if (gotMachineOffCommand)
  {
    smotor1.backOff();
  }  
  gotMachineOffCommand=false;
  gotSwitchedOffEvent=true; 
}

void reportSwitchOffEvent()
{
  if(sim1.registerEvent('O', true, false))
  {
    lastImmediateEvent='N';
    gotSwitchedOffEvent=false; 
  }  
}

void playBuzzer(byte times,byte d)
{
  int temp=d*100;
  for(byte i=0;i<times;i++)
  {
    digitalWrite(PIN_BUZZER,HIGH);
    delay(temp);
    digitalWrite(PIN_BUZZER,LOW);
    if(times>1)
      delay(temp);
  }
}

void printNumbers()
{
  #ifndef disable_debug
    Serial.print("RPM LIMIT:");
    Serial.println(eeprom1.RPM);
    if (eeprom1.numbersCount > 0)
    {
      Serial.println("Numbers:");
    
      Serial.print("1:");
      Serial.print(eeprom1.primaryNumber);
      Serial.print("\t");
      Serial.println(eeprom1.primaryNumber.length());
  
        for (int i = 0; i < eeprom1.numbersCount - 1; i++)
        {
          Serial.print(i + 2);
          Serial.print(":");
          Serial.print(eeprom1.secondary[i]);
          Serial.print("\t");
          Serial.println(eeprom1.secondary[i].length());
        }
    }
  #endif
}

ISR(BADISR_vect)
{
    #ifndef disable_debug
      USART1->println("!!!");
      USART1->println(MCUSR);
    #endif  
}

ISR(PCINT2_vect)
{
    #ifndef disable_debug
      s1.gotInterrupt();
    #endif
}

void FIVR_RPM()
{    
      rpmSensor1.lastrise = rpmSensor1.currentrise;
      rpmSensor1.currentrise = millis();
      rpmSensor1.gotTrigger = true;
}

void setup() {
  Serial.begin(19200);
  #ifndef disable_debug
    USART1->begin(19200);
  #endif

  eeprom1.loadAllData();
  printNumbers();

  sim1.setCallBackFunctions(gotImmediateResponse,getRPM);
  sim1.setDTMFFunctions(triggerIncreaseRPM,triggerDecreaseRPM,triggerStopMachine);
  
  rpmSensor1.setCallBackFunctions(rpmIncreased, machineSwitchedOff,machineSwitchedOn);

  sim1.setEEPROM(&eeprom1);
  rpmSensor1.setEEPROM(&eeprom1);
  smotor1.setEEPROM(&eeprom1);

  attachInterrupt(digitalPinToInterrupt(PIN_RPMSEN), FIVR_RPM, RISING);
  gotMachineOffCommand = false;
 
}

String str;
void loop() {

  if(gotCannotTurnOffMachineEvent)
    reportCannotTurnOffMachine();

  if(gotRPMIncreasedEvent)
    reportRPMIncreasedEvent();

  if(gotSwitchedOffEvent)
    reportSwitchOffEvent();

  if(gotStartedEvent)
    reportStartEvent();

  //if(securityAlarmed)
  //  reportSecurityBreach();

  if(!initialized)
  {
    if(!sensorInit && millis()>=3000)
    {
      batteryLevel.checkInitialBatteryLevel();
      temp1.init();

      sensorInit=true;
    }
    if(millis()>=5000)
    {
      if (!sim1.initialize())
      {
        #ifndef disable_debug
          USART1->println("NOT INIT SIM");
        #endif
          playBuzzer(5,3);
      }
      else
        playBuzzer(2,3);
      initialized = true;  
    }
    return;
  }

  #ifndef disable_debug 
  if (USART1->available() > 0)
  {
    str = USART1->readStringUntil('\n');
    if (str == "I\r")
      triggerIncreaseRPM(1);
    else if (str == "D\r")
      triggerDecreaseRPM(1);
    else if (str == "A\r")
      triggerStopMachine();
    else
      sim1.operateOnMsg(str,false);
  }
  #endif

    rpmSensor1.update();
    sim1.update();
    smotor1.update();
    temp1.update();
    batteryLevel.update();  

    if(temp1.triggerAlarm)
      maxTempLimitReached();

    if(batteryLevel.triggerAlarm)
      lowBattery();
}