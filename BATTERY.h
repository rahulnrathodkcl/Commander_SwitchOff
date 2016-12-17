#ifndef BATTERY_h

#define BATTERY_h
#include "Definitions.h"

class BATTERY
{
	byte batteryLevel;
	unsigned long lastCheck;
	bool alarmed;

	#ifndef disable_debug
  		#ifdef software_SIM
		    HardwareSerial* _Serial;
		#else
		    SoftwareSerial* _Serial;
		#endif
	#endif

	void anotherConstructor();
	void operateOnQuery();
	bool checkSufficientLevel();
	byte getBatteryLevel();
	void checkBatteryLevel();

	public:
			#ifndef disable_debug
  		#ifdef software_SIM
			BATTERY(HardwareSerial *s);
		#else
			BATTERY(SoftwareSerial *s);
		#endif
	#else
		BATTERY();
	#endif
	bool triggerAlarm;
	void checkInitialBatteryLevel();
	void checkedAlarm(bool falseAlarm=false);
	void update();
};
#endif