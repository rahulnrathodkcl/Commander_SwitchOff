#ifndef SMOTOR_R_h
#define SMOTOR_R_h

#include "S_EEPROM.h"
#include "Definitions.h"


class SMOTOR_R
{			
	S_EEPROM *eeprom1;
	const char MOTOR_PREVIOUSSTATE='L';
	const char MOTOR_HIGH='H';
	const char MOTOR_LOW='L';
	const char MOTOR_NORMAL='N';

	bool selfOperating;

	byte speedWaitTime;
	unsigned long speedWait;
	
	byte waitTime;
	unsigned long wait;
	unsigned long switchOffWaitTime;

	byte steps;
	bool backingOff;

	bool operationPerformed;
	bool operating;
	char currentOperation;
	float lastMotorPos;

	void (*finc)();
	void (*fdec)();
	void (*fMachineNotSwitchedOff)();
	void (*returnCommandStatus)(bool);

	
	void anotherConstructor(void (*inc)(),void (*dec)(),void (*MnotSwitchOff)(),void(*retStatus)(bool));
	
	bool operatingTimeOver();
	bool operateOnEvent();

	void signalOn();
	void signalOff();
	void turnOn();
	void turnOff();

	bool makeResponseElligible();
	void makeResponse();

	unsigned short int getAnalogInput();

	void stopBackOff();
	void stopOperating();

	#ifndef disable_debug
  		#ifdef software_SIM
		    HardwareSerial* _Serial;
		#else
		    SoftwareSerial* _Serial;
		#endif
	#endif

	public:

#ifndef disable_debug
	#ifdef software_SIM
		SMOTOR_R(HardwareSerial *serial,void (*inc)(),void (*dec)(),void (*MnotSwitchOff)(),void(*retStatus)(bool));
	#else
		SMOTOR_R(SoftwareSerial *serial,void (*inc)(),void (*dec)(),void (*MnotSwitchOff)(),void(*retStatus)(bool));
	#endif
#else
	SMOTOR_R(void (*inc)(),void (*dec)(),void (*MnotSwitchOff)(),void(*retStatus)(bool));
#endif

    void setEEPROM(S_EEPROM* e1);
    void increaseRPM(byte steps);
	void decreaseRPM(byte steps);
	void switchOff();

	void backOff();
	char checkCurrentOperation();
	char checkLimit();
	void update();
};
#endif
