#include "TEMP.h"

TEMP::TEMP()
{
	wait=0;
	getTempWait=0;
	tempRequested=false;
	
	oneWire=new OneWire(PIN_TEMPSEN);
	sensors=new DallasTemperature(oneWire);

	sensors->begin();
	tempAlarmRaised=false;

	if(sensors->getDeviceCount()==0)
	{
  		sensorOn=false;
	}	
	else
	{
  		sensorOn=true;
  		sensors->getAddress(tempSensor, 0);	
	}
  	sensors->setWaitForConversion(false);
  	temperature=0;
}

void TEMP::setEEPROM(S_EEPROM *eeprom1)
{
	this->eeprom1=eeprom1;
}

float TEMP::retTemp()
{
	return temperature;
}

bool TEMP::tempElligible()
{
	return (sensorOn && !tempRequested && ((millis()-wait)>=TEMPWAITTIME));	
}

void TEMP::requestTemp()
{
	sensors->requestTemperatures();	
	getTempWait=millis();
	tempRequested=true;
}

bool TEMP::getTempElligible()
{
	return(sensorOn && tempRequested && (millis()-getTempWait)>=GETTEMPWAITTIME);
}

void TEMP::getTemp()
{
	temperature=sensors->getTempC(tempSensor);
	wait=millis();
	tempRequested=false;
}

bool TEMP::checkTempLimitReached()
{
	if(temperature>(float)(eeprom1->HIGHTEMP))
		return true;
	else
	{
		tempAlarmRaised=false;
		return false;
	}
}

void TEMP::operateOnEvent()
{
	if(!tempAlarmRaised)
	{
		triggerAlarm=true;
	}
}

void TEMP::init()
{
	if(sensorOn)
		requestTemp();
}

void TEMP::checkedAlarm(bool falseAlarm)
{
	if(triggerAlarm)
	{
			if(!falseAlarm)
				tempAlarmRaised=true;
			triggerAlarm=false;
	}
}

void TEMP::update()
{

	if(!eeprom1->machineOn)
		tempAlarmRaised=false;

	if(tempElligible())
		requestTemp();
	
	if(getTempElligible())
	{
		getTemp();
		if(checkTempLimitReached() && eeprom1->machineOn)
			operateOnEvent();
	}
}
