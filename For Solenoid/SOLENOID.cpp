#include "SOLENOID.h"
#include <Arduino.h>

SOLENOID::SOLENOID(byte VREL_SOLENOID,void (*machineNotOff)())
{
	
	REL_SOLENOID=VREL_SOLENOID;
	pinMode(REL_SOLENOID,OUTPUT);
	digitalWrite(REL_SOLENOID,HIGH);

	operating=false;
	tempTime=0;
	operateTime=600;
	cannotTurnOffMachine = *machineNotOff;
}

bool SOLENOID::timeOut()
{
	return(operating && (millis()-tempTime)>=(operateTime*100));
}

void SOLENOID::turnOnSolenoid()
{
	digitalWrite(REL_SOLENOID,LOW);
	tempTime=millis();
	operating=true;
}

void SOLENOID::turnOffSolenoid()
{
	digitalWrite(REL_SOLENOID,HIGH);
	operating=false;
}

void SOLENOID::update()
{
	if(timeOut())
	{
		turnOffSolenoid();
		cannotTurnOffMachine();
	}
}