#include "BATTERY.h"

#ifndef disable_debug
  #ifdef software_SIM
    BATTERY::BATTERY(HardwareSerial *serial)
    {
      _Serial = serial;
      _Serial->begin(19200);
      anotherConstructor();
    }
  #else
    BATTERY::BATTERY(SoftwareSerial *serial)
    {
      _Serial = serial;
      _Serial->begin(19200);
      anotherConstructor();
    }
  #endif
#else
	BATTERY::BATTERY()
	{
		anotherConstructor();
	}
#endif


void BATTERY::anotherConstructor()
{
	pinMode(PIN_BATTERYPWR,OUTPUT);
	alarmed=false;
	lastCheck=0;
}

bool BATTERY::checkSufficientLevel()
{	
	if(batteryLevel<=2 || batteryLevel>80)
		return false;
	else if(batteryLevel>3 && alarmed==true)
	{
			alarmed=false;
			return true;
	}
}

byte BATTERY::getBatteryLevel()
{
	float temp;
	unsigned int t;

	digitalWrite(PIN_BATTERYPWR,HIGH);
	analogRead(PIN_BATTERYSEN);
	t=millis();
	while(millis()-t<2)
	{}
	temp=analogRead(PIN_BATTERYSEN);

	digitalWrite(PIN_BATTERYPWR,LOW);
	temp=temp*5.0/1024.0;
	temp=temp*4.030;
	temp=temp-11.5;
	#ifndef disable_debug
		_Serial->print("Reading For Processing:");
		_Serial->println(temp);
		_Serial->println((byte)(temp*10));
	#endif
	temp=temp*10;
	lastCheck=millis();
	return (byte)temp;
}

void BATTERY::checkInitialBatteryLevel()
{
	checkBatteryLevel();
}

void BATTERY::checkBatteryLevel()
{
	batteryLevel=getBatteryLevel();
	if(!checkSufficientLevel() && !alarmed)
	{
			triggerAlarm=true;
	}
}

void BATTERY::checkedAlarm(bool falseAlarm)
{
	if(triggerAlarm)
	{
		if(!falseAlarm)
			alarmed=true;
		triggerAlarm=false;
	}
}

void BATTERY::update()
{
	if(millis()-lastCheck>180000L)
		checkInitialBatteryLevel();
}