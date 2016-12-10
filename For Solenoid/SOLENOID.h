#ifndef SOLENOID_h
#define SOLENOID_h
#include <Arduino.h>

class SOLENOID
{

	byte REL_SOLENOID;
	bool operating;
	long int tempTime;
	int operateTime;
	void (*cannotTurnOffMachine)();


	bool timeOut();
	
	public:
		SOLENOID(byte,void(*func1)());		
		void turnOnSolenoid();
		void turnOffSolenoid();
		void update();
};

#endif 