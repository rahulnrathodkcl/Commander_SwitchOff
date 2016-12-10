#ifndef BATTERY_h

#define BATTERY_h
#include <Arduino.h>

#define disable_debug

class BATTERY
{
	byte SEN_PIN;
	byte PWR_PIN;
	byte batteryLevel;
	bool motorOff;
	unsigned long lastCheck;
	bool alarmed;
	#ifndef disable_debug
	HardwareSerial *_Serial;
	#endif

	byte getBatteryLevel();
	char (*fMotorStatus)();
	void (*fLowBattery)();
	bool checkSufficientLevel();

	public:
	BATTERY(byte,byte,void (*lowBattery)(),char (*motorStatus)(),HardwareSerial *serial);
	void update();
	void checkInitialBatteryLevel();
};
#endif