#include "BATTERY.h"

BATTERY::BATTERY(byte BPWR_PIN,byte BSEN_PIN,void (*lbattery)(),char (*fmotor)(),HardwareSerial *serial)
{
	PWR_PIN=BPWR_PIN;
	SEN_PIN=BSEN_PIN;
	pinMode(PWR_PIN,OUTPUT);
	alarmed=false;
	lastCheck=0;
	fLowBattery=*lbattery;
	fMotorStatus=*fmotor;

	#ifndef disable_debug
		_Serial=serial;		
	#endif
}

bool BATTERY::checkSufficientLevel()
{	
	if(fMotorStatus()=='N')
	{
			if(batteryLevel<=2 || batteryLevel>80)
				return false;
			else if(batteryLevel>3 && alarmed==true)
				alarmed=false;
		return true;
	}
	return false;
}

byte BATTERY::getBatteryLevel()
{
	float temp;
	unsigned int t;

	digitalWrite(PWR_PIN,HIGH);
	analogRead(SEN_PIN);
	t=millis();
	while(millis()-t<2)
	{}
	temp=analogRead(SEN_PIN);

	digitalWrite(PWR_PIN,LOW);
	temp=temp*5.0/1024.0;
	temp=temp*4.030;
	temp=temp-11.5;
	#ifndef disable_debug
		_Serial->print("Reading For Processing:");
		_Serial->println(temp);
		_Serial->println((byte)(temp*10));
	#endif
//	if(temp<-1)
//	{
//		return 0xFF;
//	}
	temp=temp*10;
	lastCheck=millis();
	return (byte)temp;
}

void BATTERY::checkInitialBatteryLevel()
{
	batteryLevel=getBatteryLevel();
	if(!checkSufficientLevel() && alarmed==false)
	{
			#ifndef disable_debug
			_Serial->print("Battery is Low");
			#endif
			fLowBattery();
			alarmed=true;
	}
}

void BATTERY::update()
{
	if(millis()-lastCheck>180000L)
		checkInitialBatteryLevel();
}