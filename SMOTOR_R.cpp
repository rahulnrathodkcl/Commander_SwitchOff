#include <Arduino.h>
#include "SMOTOR_R.h"

#ifndef disable_debug
	#ifdef software_SIM
		SMOTOR_R::SMOTOR_R(HardwareSerial *serial,void (*inc)(),void (*dec)(),void (*MnotSwitchOff)(),void(*retStatus)(bool))
		{
			_Serial=serial;
			anotherConstructor(inc,dec,MnotSwitchOff,retStatus);
		}
	#else
		SMOTOR_R::SMOTOR_R(SoftwareSerial *serial,void (*inc)(),void (*dec)(),void (*MnotSwitchOff)(),void(*retStatus)(bool))
		{
			_Serial=serial;
			anotherConstructor(inc,dec,MnotSwitchOff,retStatus);
		}
	#endif

#else
	SMOTOR_R::SMOTOR_R(void (*inc)(),void (*dec)(),void (*MnotSwitchOff)(),void(*retStatus)(bool))
	{
		anotherConstructor(inc,dec,MnotSwitchOff,retStatus);
	}

#endif

void SMOTOR_R::anotherConstructor(void (*inc)(),void (*dec)(),void (*MnotSwitchOff)(),void(*retStatus)(bool))
{
	pinMode(PIN_SMOTORPWR,OUTPUT);

	pinMode(PIN_SLOW,OUTPUT);
	pinMode(PIN_FAST,OUTPUT);

	digitalWrite(PIN_SLOW,LOW);
	digitalWrite(PIN_FAST,LOW);
	
	waitTime=250;//x100
	switchOffWaitTime=600;
	speedWaitTime=6;
	currentOperation='N';

	finc=*inc;
	fdec=*dec;
	fMachineNotSwitchedOff=*MnotSwitchOff;
	returnCommandStatus=*retStatus;

	operating=false;
	backingOff=false;
	operationPerformed=false;
}

bool SMOTOR_R::operatingTimeOver()
{
	if(operating)
	{
		if(currentOperation!='S' && millis()-speedWait>=((speedWaitTime*steps)*100))
			{
				#ifndef disable_debug
					_Serial->println("TIME LIMIT");
				#endif
					return true;
			}
	}
	return false;
}

bool SMOTOR_R::operateOnEvent()
{
	if(operating)
	{
		char r=checkLimit();
		if(backingOff)
		{
			if(currentOperation=='B' && (r==MOTOR_PREVIOUSSTATE || r==MOTOR_LOW))
				return true;
		}
		
		if((r==MOTOR_LOW && currentOperation=='I') || (r==MOTOR_HIGH && (currentOperation=='D' || currentOperation=='S')))
			return true;
	}	
	return false;
}

void SMOTOR_R::increaseRPM(byte steps)
{
	if(!operating && checkLimit()!=MOTOR_LOW)
	{
		this->steps=steps;
		currentOperation='I';
		turnOn();
		digitalWrite(PIN_FAST,LOW);
		digitalWrite(PIN_SLOW,HIGH);
		speedWait=millis();
		returnCommandStatus(true);
	}
	else
	{
		#ifndef disable_debug
			_Serial->println("Motor limit");
		#endif
		returnCommandStatus(false);
	}
}

void SMOTOR_R::setEEPROM(S_EEPROM* e1)
{
  eeprom1=e1;
}

void SMOTOR_R::decreaseRPM(byte steps)
{	
	if(!operating && checkLimit()!=MOTOR_HIGH)
	{	
		this->steps=steps;
		currentOperation='D';
		turnOn();
		digitalWrite(PIN_SLOW,LOW);
		digitalWrite(PIN_FAST,HIGH);
		speedWait=millis();
		returnCommandStatus(true);
	}
	else
	{
		#ifndef disable_debug
			_Serial->println("Motor limit");
		#endif
		returnCommandStatus(false);
	}
}

void SMOTOR_R::switchOff()
{
	if(!operating && checkLimit()!=MOTOR_HIGH)
	{
		currentOperation='S';
		turnOn();
		lastMotorPos=getAnalogInput();
		digitalWrite(PIN_SLOW,LOW);
		digitalWrite(PIN_FAST,HIGH);
		returnCommandStatus(true);
	}
	else
	{	
		#ifndef disable_debug
		_Serial->println("Motor limit");
		#endif
		returnCommandStatus(false);
	}
}


void SMOTOR_R::stopBackOff()
{
	digitalWrite(PIN_SLOW,HIGH);
	digitalWrite(PIN_FAST,HIGH);
	turnOff();
}

void SMOTOR_R::stopOperating()
{
	digitalWrite(PIN_SLOW,HIGH);
	digitalWrite(PIN_FAST,HIGH);
	turnOff();
	wait=millis();
	operationPerformed=true;
}

void SMOTOR_R::backOff()
{
	operationPerformed=false;
	currentOperation='B';
	turnOn();
	backingOff=true;
	digitalWrite(PIN_SLOW,HIGH);
	digitalWrite(PIN_FAST,LOW);
}

bool SMOTOR_R::makeResponseElligible()
{
	if(operationPerformed)
	{
		if(currentOperation=='S')
		{
			return ((millis()-wait)>=(switchOffWaitTime*100));
		}
		else
			return ((millis()-wait)>=(waitTime*100));	
	}
}

void SMOTOR_R::makeResponse()
{
	if(currentOperation=='I')
	{
		finc();
	}
	else if(currentOperation=='D')
	{
		fdec();
	}
	else if(currentOperation=='S')
	{
		fMachineNotSwitchedOff();
	}
	operationPerformed=false;
}

char SMOTOR_R::checkCurrentOperation()
{
	if(operating)
	{
		return currentOperation;
	}
	else return 'N';
}

void SMOTOR_R::signalOn()
{
	digitalWrite(PIN_SMOTORPWR,HIGH);	
}

void SMOTOR_R::signalOff()
{
	digitalWrite(PIN_SMOTORPWR,LOW);	
}

void SMOTOR_R::turnOn()
{
	operating=true;
	signalOn();
}

void SMOTOR_R::turnOff()
{
	operating=false;
	backingOff=false;
	currentOperation='N';
	signalOff();
}

unsigned short int SMOTOR_R::getAnalogInput()
{
	unsigned long temp;
	signalOn();
	analogRead(PIN_SMOTORSIG);
	temp=millis();
	while(millis()-temp<2)
	{}
	temp=analogRead(PIN_SMOTORSIG);
	signalOff();
	return (unsigned short int)temp;
}

char SMOTOR_R::checkLimit()
{	
	unsigned short int temp=getAnalogInput();
	if(temp<=(eeprom1->MOTORLOW))
		return MOTOR_LOW;
	else if(temp>=(eeprom1->MOTORHIGH))
		return MOTOR_HIGH; 
	else 
	{
		if(operating && backingOff)
		{
			if(temp<=lastMotorPos)
				return MOTOR_PREVIOUSSTATE;
		}
		return MOTOR_NORMAL;
	}
}

void SMOTOR_R::update()
{
		// if(checkStopOperatingElligible())
			// stopOperating();

		if(operatingTimeOver() || operateOnEvent())
		{
			if(backingOff)	stopBackOff();
			else 			stopOperating();
		}	

		if(makeResponseElligible())
			makeResponse();

		// if(stopBackingOffElligible())
			// stopBackOff();
}