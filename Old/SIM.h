//Version 2 Dated 29052016
#ifndef SIM_h
#define SIM_h

#include <Arduino.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>

#define disable_debug

class SIM
{
  private:
    
    byte soundPlayNumber;
    byte soundWaitTime; //x100 = mSec
    byte soundPlayedNumber;
    byte SLEEP_PIN;
    unsigned long soundWait;
    bool bplaySound;
    char playFile;

    byte callCutWaitTime;  //x100 = mSec
    unsigned long callCutWait;

    bool commandsAccepted;
    byte acceptCommandsTime;
    unsigned long tempAcceptCommandTime;

    char currentStatus;
    char currentCallStatus;

    byte nr;
    char responseSetting;
    bool gotSettingTone;
    bool callAccepted;

    int responseWaitTime;
    unsigned long responseWait;
    bool makeResponse;
    char actionType;

    bool freezeIncomingCalls;

    bool immediateEvent;
    bool sendImmediateResponse;
    char (*f1)();
    char (*f2)();
    char (*f3)();
    /*char (*f4)();
    void (*f5)();*/
    void (*immediateFeedback)(bool);


    void anotherConstructor(byte);
    bool isNumber(String &str);
    bool checkNumber(String);
    
    void acceptCommands();
    void rejectCommands();
    void sendCommand(char cmd,bool newline);
    void sendCommand(String cmd,bool newline);

    bool sendBlockingATCommand(String);
    String readString();
    bool matchString(String, String);
    bool stringContains(String &sstr, String mstr, int sstart, int sstop);
    bool isRinging(String);
    bool isDTMF(String &str);
    bool isCut(String);
    bool isSoundStop(String);
    char callState(String);
    void makeCall();
    void endCall();
    void acceptCall();
    void sendSMS();
    void operateDTMF(String str);
    void operateRing();
    bool playSoundElligible();
    void triggerPlaySound();
    void playSoundAgain(String);
    void playSound(char c,bool x=false);
    void stopSound();
    bool callTimerExpire();
    bool responseActionElligible();
    void makeResponseAction();
    void sendImmediateFeedback(bool);
    bool rejectCommandsElligible();
    void checkNetwork(String str);
    void networkCounterMeasures();

    #ifndef disable_debug
        HardwareSerial* _NSerial;
        SoftwareSerial* _SSerial;
    #else
        SoftwareSerial* _SSerial;
    #endif
    void debugPrint(void *a);

  public:
    bool initialized;

    #ifndef disable_debug
        SIM(HardwareSerial* serial, SoftwareSerial* serial1,byte SLEEP_PIN);
    #else
        SIM(SoftwareSerial* serial,byte SLEEP_PIN);    
    #endif
    
    bool initialize();
    void registerEvent(char eventType, bool immediate,bool getResponse);
    void setDTMFFunctions(char (*p1)(),char (*p2)(),char (*p3)());
    void setCallBackFunctions(void (*ImdEvent)(bool));
    void update();
};
#endif