#ifndef DEF_h
#define DEF_h

#include <SoftwareSerial.h>
#include <HardwareSerial.h>
//#define disable_debug

#define CHK_BATTERY
// SELF
#ifdef disable_debug
	#undef software_SIM
#endif

#define PIN_SLOW A4
#define PIN_FAST A3

#define PIN_BUZZER 3
#define PIN_SIMSLEEP 2
#define PIN_RPMSEN 3

#define PIN_BATTERYPWR 8
#define PIN_BATTERYSEN A2

//SEC_LIMIT || DIESEL
#define PIN_SECLIMITPWR 0
#define PIN_SECLIMITSIG A5

// SMOTOR
#define PIN_SMOTORPWR A4
#define PIN_SMOTORSIG A3

// TEMP
#define PIN_TEMPSEN 4
#define TEMPWAITTIME 30000L
#define GETTEMPWAITTIME 1500

// EEPROM
#define RPMAddress 0
#define numbersCountAddress 4
#define mobileNumberAddress 8
#define highTempAddress 90
#define motorHighAddress 94
#define motorLowAddress 98
// #define forceStartAddress 102
#define alterNumberPresentAddress 106
#define alterNumberSettingAddress 110
#define alterNumberAddress 114

#define EEPROM_MIN_ADDR 0
#define EEPROM_MAX_ADDR 1023

#endif