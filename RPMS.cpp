#include "RPMS.h"
#include <Arduino.h>

#ifdef USE_ALTERNATOR
  RPMS::RPMS(byte SRPM, byte RELALTERNATOR,int VRPMLimit, HardwareSerial *serial)
  {

    REL_ALTERNATOR=RELALTERNATOR;
    pinMode(REL_ALTERNATOR,OUTPUT);
    digitalWrite(REL_ALTERNATOR,HIGH);

    anotherConstructor(SRPM,VRPMLimit)
    #ifndef disable_debug
      _NSerial = serial;
      _NSerial->begin(19200);
    #endif
  }

#else

  RPMS::RPMS(byte SRPM,int VRPMLimit, HardwareSerial *serial)
  {
    anotherConstructor(SRPM,VRPMLimit);
 
    #ifndef disable_debug
      _NSerial = serial;
      _NSerial->begin(19200);
    #endif
  }
#endif

void RPMS::anotherConstructor(byte SRPM, int VRPMLimit)
{
    SEN_RPM = SRPM;
    RPMLimit=VRPMLimit;
    pinMode(SEN_RPM,INPUT);

    turnMachineOff(); 
    RPM = 0;
    HRPMCnt=0;
    lastrise = 0;
    currentrise = 0;
    gotTrigger = false;
    firedRPMEvent=false;  
}

void RPMS::setEEPROM(S_EEPROM* e1)
{
  eeprom1=e1;
}

void RPMS::IVR_RPM()
{
  double tempRPM = 0;
  double crise,lrise;
  noInterrupts();
  crise=currentrise;
  lrise=lastrise;
  interrupts();  

  if (crise != 0 && lrise != 0 && crise!=lrise)
  {
    RPM = 60000.0 / (crise - lrise);
    tempRPM = RPM;
    
    if(getMachineStatus()==false)
    {
      if(tempRPM>150 && startCnt<10)
        startCnt++;
      else if(startCnt>=10)
        turnMachineOn();    
    }
    /*if(tempRPM==0)
    {
      turnMachineOn();    
    }*/

    if(!firedRPMEvent && machineOn)
    {
        if(tempRPM>(eeprom1->RPM))
        {
          if(HRPMCnt<10)
            HRPMCnt++;
          else
          {
            firedRPMEvent=true;
            RPMLimitReached();
            HRPMCnt=0;
          }          
        }
        else
        {
          HRPMCnt=0;
        }
    }
    #ifndef disable_debug
    _NSerial->println(tempRPM);
    #endif
  }
}

void RPMS::checkNoRPM()
{
  unsigned long temp,cTime;
  double tempRPM;

  if(gotTrigger)
    return;
  else
  {
    noInterrupts();
    temp = currentrise;
    interrupts();
    tempRPM = RPM;
    cTime = millis();
  }

  if (temp!=0 && (cTime - temp)>= 3000)
  {
    if (tempRPM != 0)
    {
      #ifndef disable_debug
      _NSerial->println("SW OFF");
      #endif
      RPM = 0;

      if(getMachineStatus())
      {
        MachineSwitchedOff();
      }
      turnMachineOff();
    }
  }
}

void RPMS::setCallBackFunctions(void(*funcRPMChange)(), void(*funcMachineOff)(),void(*funcMachineOn)())
{
  RPMLimitReached = *funcRPMChange;
  MachineSwitchedOff = *funcMachineOff;
  MachineSwitchedOn = *funcMachineOn;
}

double RPMS::getRPM()
{
  double tempRPM;
  noInterrupts();
  tempRPM = RPM;
  interrupts();
  return tempRPM;
}

void RPMS::turnMachineOn()
{
  startCnt=0;
  machineOn = true;
  MachineSwitchedOn();
  #ifdef USE_ALTERNATOR
  digitalWrite(REL_ALTERNATOR,LOW);
  #endif
}

void RPMS::turnMachineOff()
{
  startCnt=0;
  machineOn = false;
  #ifdef USE_ALTERNATOR
  digitalWrite(REL_ALTERNATOR,HIGH);
  #endif
}

bool RPMS::getMachineStatus()
{
  return machineOn;
}

void RPMS::discardRPMEvent()
{
  firedRPMEvent=false;
}

void RPMS::update()
{
  if (gotTrigger)
  {
    IVR_RPM();
    gotTrigger = false;
  }
  checkNoRPM();
}
