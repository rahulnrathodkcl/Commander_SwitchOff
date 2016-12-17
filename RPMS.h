#ifndef RPMS_h
#define RPMS_h

#include "S_EEPROM.h"
#include "Definitions.h"

class RPMS
{
	private:
	volatile double RPM;
 
	void anotherConstructor();
    bool firedRPMEvent;
	byte startCnt;
	byte HRPMCnt;
	
	#ifndef disable_debug
  		#ifdef software_SIM
		    HardwareSerial* _NSerial;
		#else
		    SoftwareSerial* _NSerial;
		#endif
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


	#ifndef disable_debug
  		#ifdef software_SIM
			RPMS(HardwareSerial *s);
		#else
			RPMS(SoftwareSerial *s);
		#endif
	#else
		RPMS();
	#endif

    void setEEPROM(S_EEPROM* e1);
	void IVR_RPM();
	void setCallBackFunctions(void (*funcRPMChange)(),void (*machineSwitchOff)(),void (*machineTurnedOn)());
	double getRPM();
	void update();
};
#endif
