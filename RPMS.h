#ifndef RPMS_h
#define RPMS_h

#include <Arduino.h>
#include "S_EEPROM.h"
#define disable_debug

class RPMS
{
	private:
	volatile double RPM;
 
	void anotherConstructor(byte,int);
	byte SEN_RPM;
	int RPMLimit;
    bool firedRPMEvent;
	byte startCnt;
	byte HRPMCnt;

	#ifndef disable_debug
	HardwareSerial* _NSerial;
	#endif

	void (*RPMLimitReached)();
	void (*MachineSwitchedOff)();
	void (*MachineSwitchedOn)();

	void checkNoRPM();
	void turnMachineOn();
	void turnMachineOff();
	bool getMachineStatus();
	
	public:

    S_EEPROM* eeprom1;
	bool gotTrigger;
	volatile bool machineOn;
  	volatile unsigned long lastrise;
  	volatile unsigned long currentrise;

  	#ifdef USE_ALTERNATOR
		byte REL_ALTERNATOR;
		RPMS(byte,byte,int,HardwareSerial *s);

  	#else
  		RPMS(byte,int,HardwareSerial *s);
  	#endif
    
    void setEEPROM(S_EEPROM* e1);
	void IVR_RPM();
	void setCallBackFunctions(void (*funcRPMChange)(),void (*machineSwitchOff)(),void (*machineTurnedOn)());
	double getRPM();
	void discardRPMEvent();
	void update();
};
#endif
