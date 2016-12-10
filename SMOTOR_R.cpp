#include "SMOTOR_R.h"

SMOTOR_R::SMOTOR_R(byte SLOWPIN,byte FASTPIN,byte PWRPIN,byte SENPIN,float minVOLT,float maxVOLT,HardwareSerial *serial,void (*inc)(),void (*dec)(),void (*MnotSwitchOff)())
{
	REL_SLOW=SLOWPIN;
	REL_FAST=FASTPIN;
	PWR_SEN=PWRPIN;
	SEN_PIN=SENPIN;
	minVoltage=minVOLT;
	maxVoltage=maxVOLT;

	pinMode(REL_SLOW,OUTPUT);
	pinMode(REL_FAST,OUTPUT);
	pinMode(PWR_SEN,OUTPUT);

	digitalWrite(REL_SLOW,HIGH);
	digitalWrite(REL_FAST,HIGH);
	//pinMode(SEN_PIN,INPUT);
	
	waitTime=250;//x100
	switchOffWaitTime=600;
	speedWaitTime=6;
	currentOperation='N';

	finc=*inc;
	fdec=*dec;
	fMachineNotSwitchedOff=*MnotSwitchOff;

	operating=false;
	backingOff=false;
	operationPerformed=false;
	#ifndef disable_debug
	_Serial=serial;
	#endif
}

bool SMOTOR_R::increaseRPM()
{
	if(checkLimit()!='F')
	{
		if(operating)
			return false;
		currentOperation='I';
		digitalWrite(PWR_SEN,HIGH);
		digitalWrite(REL_FAST,LOW);
		digitalWrite(REL_SLOW,HIGH);
		speedWait=millis();
		operating=true;	
		return true;
	}
	else
	{
		#ifndef disable_debug
		_Serial->println("Motor limit");
		#endif
		return false;	
	}
}

bool SMOTOR_R::decreaseRPM()
{	
	if(checkLimit()!='S')
	{	
		if(operating)
			return false;
		currentOperation='D';
		digitalWrite(PWR_SEN,HIGH);
		digitalWrite(REL_SLOW,LOW);
		digitalWrite(REL_FAST,HIGH);
		speedWait=millis();
		operating=true;
		return true;
	}
	else
	{
		#ifndef disable_debug
		_Serial->println("Motor limit");
		#endif
		return false;
	}
}

bool SMOTOR_R::switchOff()
{
	if(checkLimit()!='S')
	{
		if(operating)
			return false;
		currentOperation='S';
		lastMotorPos=getAnalogInput();
		digitalWrite(PWR_SEN,HIGH);
		digitalWrite(REL_SLOW,LOW);
		digitalWrite(REL_FAST,HIGH);
		operating=true;	
		return true;
	}
	else
		return false;
}

void SMOTOR_R::stopOperating()
{
	digitalWrite(PWR_SEN,LOW);
	digitalWrite(REL_SLOW,HIGH);
	digitalWrite(REL_FAST,HIGH);
	operating=false;
	wait=millis();
	operationPerformed=true;
}

bool SMOTOR_R::checkStopOperatingElligible()
{
	if(operating)
	{
		char r=checkLimit();
		#ifndef disable_debug
		_Serial->println(r);
		#endif
		if((r=='S' && currentOperation=='D') || (r=='F' && currentOperation=='I') || (r=='S' && currentOperation=='S'))
		{
			#ifndef disable_debug
			_Serial->println("Motor Limit");
			#endif
			return true;
		}
		else
		{
			if(currentOperation!='S' && millis()-speedWait>=(speedWaitTime*100))
			{
				#ifndef disable_debug
				_Serial->println("TIME LIMIT");	
				#endif
				return true;
			}
		}
	}	
	return false;
}

bool SMOTOR_R::stopBackingOffElligible()
{
	if(selfOperating)
		return true;

	if(backingOff)
	{
		float temp=getAnalogInput();
		if(temp<=minVoltage || temp<=lastMotorPos)
			return true;
		else
			return false;
	}	
	return false;
}

void SMOTOR_R::stopBackOff()
{
	currentOperation='N';
	digitalWrite(PWR_SEN,LOW);
	digitalWrite(REL_SLOW,HIGH);
	digitalWrite(REL_FAST,HIGH);
	backingOff=false;

}

void SMOTOR_R::backOff()
{
	currentOperation='B';
	operationPerformed=false;
	digitalWrite(PWR_SEN,HIGH);
	digitalWrite(REL_SLOW,HIGH);
	digitalWrite(REL_FAST,LOW);
	backingOff=true;
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
	if(backingOff)
		return 'B';
	else return 'N';
}

float SMOTOR_R::getAnalogInput()
{
	unsigned long temp;
	float t2;
	temp=0;

	digitalWrite(PWR_SEN,HIGH);
	analogRead(SEN_PIN);
	for (int i=0;i<30;i++)
	{
		temp+=analogRead(SEN_PIN);
	}

	t2=temp/6144.0;		//t2=temp/30*5/1024;
	return t2;
}

char SMOTOR_R::checkLimit()
{	
	float temp=getAnalogInput();
	#ifndef disable_debug
	_Serial->println(temp);
	#endif
	if(temp<=minVoltage)
		return 'F';
	else if(temp>=maxVoltage)
		return 'S'; 
	else 
		return 'N';
}

void SMOTOR_R::update()
{
		if(checkStopOperatingElligible())
			stopOperating();

		if(makeResponseElligible())
			makeResponse();

		if(stopBackingOffElligible())
			stopBackOff();
}