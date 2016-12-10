#ifndef SMOTOR_R_h
#define SMOTOR_R_h
#include <Arduino.h>

#define disable_debug

class SMOTOR_R
{			
	byte REL_SLOW;
	byte REL_FAST;
	byte PWR_SEN;
	byte SEN_PIN;

	bool selfOperating;

	float minVoltage;
	float maxVoltage;

	byte speedWaitTime;
	unsigned long speedWait;
	
	byte waitTime;
	unsigned long wait;
	unsigned long switchOffWaitTime;

	bool backingOff;

	bool operationPerformed;
	bool operating;
	char currentOperation;
	float lastMotorPos;

	void (*finc)();
	void (*fdec)();
	void (*fMachineNotSwitchedOff)();
	
	void stopOperating();
	bool checkStopOperatingElligible();
	
	bool stopBackingOffElligible();
	void stopBackOff();
	
	bool makeResponseElligible();
	void makeResponse();

	float getAnalogInput();
	#ifndef disable_debug
	HardwareSerial *_Serial;
	#endif

	public:

	SMOTOR_R(byte SLOWPIN,byte FASTPIN,byte PWRPIN,byte SEN,float MINVOLT,float MAXVOLT,HardwareSerial *serial,void (*inc)(),void (*dec)(), void (*MNotSwitchOff)());
	bool increaseRPM();
	bool decreaseRPM();
	bool switchOff();
	void backOff();
	char checkCurrentOperation();
	char checkLimit();
	void update();
};
#endif
