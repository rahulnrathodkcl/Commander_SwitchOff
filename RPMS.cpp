#include "RPMS.h"

#ifndef disable_debug
  #ifdef software_SIM
    RPMS::RPMS(HardwareSerial *serial)
    {
      _NSerial = serial;
      _NSerial->begin(19200);
      anotherConstructor();
    }
  #else
    RPMS::RPMS(SoftwareSerial *serial)
    {
      _NSerial = serial;
      _NSerial->begin(19200);
      anotherConstructor();
    }
  #endif
#else
  RPMS::RPMS()
  {
    anotherConstructor();
  }
#endif

void RPMS::anotherConstructor()
{
    pinMode(PIN_RPMSEN,INPUT_PULLUP);

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
      if(tempRPM>150 && startCnt<250)
        startCnt++;
      else if(startCnt>=250)
        turnMachineOn();    
    }
   if(firedRPMEvent && tempRPM<(eeprom1->RPM))
    {
        HRPMCnt=0;
        firedRPMEvent=false;
    }    
    
    if(!firedRPMEvent && machineOn)
    {
        if(tempRPM>(eeprom1->RPM))
        {
          if(HRPMCnt<250)
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
  eeprom1->machineOn=true;
  MachineSwitchedOn();
}

void RPMS::turnMachineOff()
{
  startCnt=0;
  machineOn = false;
  firedRPMEvent=false;
  eeprom1->machineOn=false;
}

bool RPMS::getMachineStatus()
{
  return machineOn;
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
