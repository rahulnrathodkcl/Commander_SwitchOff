#ifndef TEMP_h
#define TEMP_h
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Definitions.h"
#include "S_EEPROM.h"

class slaveSPI;
class TEMP
{	
	S_EEPROM *eeprom1;

	bool tempAlarmRaised;

	unsigned long wait;
	unsigned long getTempWait;
	bool tempRequested;

	OneWire *oneWire;
	DallasTemperature *sensors;
	DeviceAddress tempSensor;
	
	float temperature;
	bool sensorOn;

	bool tempElligible();
	void requestTemp();
	bool getTempElligible();
	void getTemp();
	void operateOnEvent();

	public:	
		bool triggerAlarm;

		TEMP();
		void setEEPROM(S_EEPROM *eeprom1);
		float retTemp();
		bool checkTempLimitReached();
		void checkedAlarm(bool falseAlarm=false);
		void init();
		void update();
};
#endif