#ifndef S_EEPROM_h
#define S_EEPROM_h

#include <Arduino.h>
#include <String.h>
#include <EEPROM.h>

#define RPMAddress 0
#define numbersCountAddress 4
#define mobileNumberAddress 8

#define EEPROM_MIN_ADDR 0
#define EEPROM_MAX_ADDR 1023

class S_EEPROM
{
    void loadRPMSettings();
    void loadMotorSettings();
    void loadTempSettings();
    
    void loadNumbers();
    void loadAlterNumber();
    void loadAlterNumberSetting();
    void clearLoadedNumbers();

    void updateNumberChanges();
    bool write_StringEE(int Addr, String input);
    String read_StringEE(int Addr, int length);
    bool eeprom_read_string(int addr, char* buffer, int bufSize);
    bool eeprom_write_string(int addr, const char* str);
    bool eeprom_is_addr_ok(int addr);
    bool eeprom_write_bytes(int startAddr, const byte* array, int numBytes);

  public:
    byte numbersCount;
    bool machineOn;
    unsigned short int RPM;
    unsigned short int MOTORLOW;
    unsigned short int MOTORHIGH;
    unsigned short int HIGHTEMP;
    
    String primaryNumber;
    String secondary[4];

    String alterNumber;
    byte alterNumberSetting;
    byte alterNumberPresent;


    S_EEPROM();
    void saveHighRPMSettings(unsigned short int);
    void saveMotorLowSettings(unsigned short int);
    void saveMotorHighSettings(unsigned short int);
    void saveTempSettings(unsigned short int temp);

    byte checkExists(String number);
    void loadAllData();
    bool addNumber(String number);
    bool addAlternateNumber(String number);
    bool removeNumber(String number);
    void clearNumbers(bool admin);
};
#endif
