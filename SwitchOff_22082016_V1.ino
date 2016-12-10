//Program Details: 
// Machine Switch Off And Increased Decrease with RPM increased event and Switched Off Event
//Version 2 Dated 22072016

//#include <avr/sleep.h>
#include "SIM.h"
#include "RPMS.h"
#include "SMOTOR_R.h"
#include "S_EEPROM.h"
#include <SoftwareSerial.h>
#define CHK_BATTERY

#define disable_debug
//#define USE_ALTERNATOR

#define SEN_RPM 2
#define REL_SLOW 3
#define REL_FAST 4
#define PWR_SIGM 9
#define SEN_MPOS A0

#define SIM_SLEEP_PIN 8

#define MinimumMotorVoltage 0.30
#define MaximumMotorVoltage 1.70
#define RPMLimit 500

bool triedInit=false;
  const byte RX=5;
  const byte TX=6;
  SoftwareSerial s1(RX,TX);

S_EEPROM eeprom1;


#ifdef disable_debug
  SIM sim1(&s1,SIM_SLEEP_PIN);
#else
  SIM sim1(&Serial,&s1,SIM_SLEEP_PIN);
#endif 

bool gotMachineOffCommand;
char lastImmediateEvent;

#ifdef USE_ALTERNATOR
  const byte REL_ALTERNATOR=10;
    RPMS rpmSensor(SEN_RPM,REL_ALTERNATOR,RPMLimit,&Serial);
#else
  RPMS rpmSensor(SEN_RPM,RPMLimit,&Serial);
#endif

void registerRPMIncreased()
{
  //sim1.registerEvent('I', false, false);
}

void cannotTurnOffMachine()
{
      #ifndef disable_debug
      Serial.println("Cannot Turn Off");  
      #endif
      sim1.registerEvent('Z', true, false); 
}

SMOTOR_R smotor1(REL_SLOW, REL_FAST, PWR_SIGM, SEN_MPOS, MinimumMotorVoltage, MaximumMotorVoltage, &Serial,registerRPMIncreased,registerRPMDecreased,cannotTurnOffMachine);

#ifdef CHK_BATTERY
  
  #include "BATTERY.h"

  #define PWR_BTRY 13
  #define SEN_BTRY A1

  void lowBattery()
  {
    #ifndef disable_debug
      Serial.println("Low Battery");
    #endif
    sim1.registerEvent('B',true,false);
  }
  
  char checkMotorOperation()
  {
    return smotor1.checkCurrentOperation();
  }
  BATTERY batteryLevel(PWR_BTRY,SEN_BTRY,lowBattery,checkMotorOperation,&Serial);
#endif

char increaseRPM()
{
  if (smotor1.increaseRPM())
  {
    return 'D';
  }
  else
  {
    #ifndef disable_debug
    Serial.println("LIMIT REACHED");
    #endif
    return 'L';
  }
}

char decreaseRPM()
{
  if (smotor1.decreaseRPM())
  {
    return 'D';
  }
  else
  {
    #ifndef disable_debug
    Serial.println("LIMIT REACHED");
    #endif
    return 'L';
  }
}

void registerRPMDecreased()
{
//  sim1.registerEvent('D', false, false);
}


char stopMachine()
{
  #ifndef disable_debug
  Serial.println("STOP ");
  #endif
  if (rpmSensor.getRPM() != 0 && rpmSensor.machineOn)
  {
          if (smotor1.switchOff())
          {
            gotMachineOffCommand = true;
            #ifndef disable_debug
            Serial.println("STOPPED");
            #endif
            return 'D';
          }
          else
          {
            #ifndef disable_debug
            Serial.println("LIMIT REACHED ");
            #endif
            return 'L';
          }
  }
  else
  {
    #ifndef disable_debug
    Serial.println(" OFF ");
    #endif
    return 'O';
  }
}

void gotImmediateResponse(bool temp)
{
	if (temp && rpmSensor.machineOn)
	{
#ifndef disable_debug
		Serial.print("Machine Off :  ");
		Serial.println(lastImmediateEvent);
#endif

		stopMachine();
	}
	else if (!temp)
	{
		rpmSensor.discardRPMEvent();
	}
	lastImmediateEvent = 'N';
}


void rpmIncreased()   //triggers when machine's RPM get increased beyond limit 
{
    #ifndef disable_debug
    Serial.println("RPM EVENT");
    #endif
    sim1.registerEvent('F', true, true);
    lastImmediateEvent = 'F';
    //stopMachine();
    
/*    Serial.println("RPM INCREASED EVENT");  
    sim1.registerEvent('F', true, true);
    lastImmediateEvent = 'F';
*/
}

void machineSwitchedOn()
{
    lastImmediateEvent='N';
    sim1.registerEvent('C', false, false);  
}

void machineSwitchedOff()
{
  if (gotMachineOffCommand)
  {
      #ifndef disable_debug
      Serial.println("Off Command. Motor BackOff");
      #endif
      smotor1.backOff();
  }
  rpmSensor.discardRPMEvent();

  lastImmediateEvent='N';

/*  if(lastImmediateEvent!='N')
  {
    sim1.registerEvent('F', true, false);
  }
  else*/
    sim1.registerEvent('O', false, false);
}

/*ISR(TIMER3_OVF_vect)
{
  //TIMSK3&=~(1<<TOIE3);
  //TIMSK3 = (TIMSK3 & 0xFE);   //disable the Timer INterrupt
  //self1.NoRPM();

  //Serial.println("Got A Timer OverFLow Event..");
  
  //digitalWrite(LED, !digitalRead(LED));
  //  TCNT3 = 0x48E4;       //initialize the counter
}*/

void printNumbers()
{
  #ifndef disable_debug
    Serial.print("RPM LIMIT:");
    Serial.println(eeprom1.RPM);
    if (eeprom1.numbersCount > 0)
    {
      Serial.println("Numbers:");
    
      Serial.print("1:");
      Serial.print(eeprom1.primaryNumber);
      Serial.print("\t");
      Serial.println(eeprom1.primaryNumber.length());
  
        for (int i = 0; i < eeprom1.numbersCount - 1; i++)
        {
          Serial.print(i + 2);
          Serial.print(":");
          Serial.print(eeprom1.secondary[i]);
          Serial.print("\t");
          Serial.println(eeprom1.secondary[i].length());
        }
    }
  #endif
}

void FIVR_RPM()
{
    if(digitalRead(SEN_RPM)==LOW)
    {
      rpmSensor.lastrise = rpmSensor.currentrise;
      rpmSensor.currentrise = millis();
      rpmSensor.gotTrigger = true;
    }

  /*int x = digitalRead(SEN_RPM);
  if (x == 0)
  {
    self1.lastrise = self1.currentrise;
    self1.currentrise = millis();
    self1.gotTrigger = true;

    //TIMSK3 |= (1 << TOIE3); //enable timer 3 
    //TCNT3 = 0x48E4;   //intialize the counter for timer 3
    //Serial.println(TCNT3);
    //self1.setTimer();
  }*/
}

void setup() {
  // put your setup code here, to run once:
  //Serial1.begin(4800);    //SIM
  #ifndef disable_debug
  Serial.begin(19200);
  #endif

  eeprom1.loadAllData();
  printNumbers();

  sim1.setCallBackFunctions(gotImmediateResponse);
  sim1.setDTMFFunctions(increaseRPM,decreaseRPM,stopMachine);
  
  rpmSensor.setCallBackFunctions(rpmIncreased, machineSwitchedOff,machineSwitchedOn);

  sim1.setEEPROM(&eeprom1);
  rpmSensor.setEEPROM(&eeprom1);

  gotMachineOffCommand = false;
 
//FIVR_RPM();
//    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
//    sleep_enable(); 
}

String str;

void loop() {

  #ifndef disable_debug  
  if (Serial.available() > 0)
  {
    str = Serial.readStringUntil('\n');
    if (str == "I\r")
      increaseRPM();
    //                    smotor1.increaseRPM();
    else if (str == "D\r")
      decreaseRPM();
    //smotor1.decreaseRPM();
    else if (str == "A\r")
      stopMachine();
  }
  #endif

  if(!triedInit && millis()>5000)
  {  
    unsigned long t=millis();
    if (!sim1.initialize())
    {
      #ifndef disable_debug
      Serial.println("Problem in SIM");
      #endif
      digitalWrite(REL_SLOW,LOW);
      digitalWrite(REL_FAST,LOW);
      delay(1000);
      digitalWrite(REL_SLOW,HIGH);
      digitalWrite(REL_FAST,HIGH);
    }
    else
    {
      digitalWrite(REL_SLOW,LOW);
      digitalWrite(REL_FAST,LOW);
      delay(2000);
      digitalWrite(REL_SLOW,HIGH);
      digitalWrite(REL_FAST,HIGH);
    }
    triedInit=true;

    attachInterrupt(digitalPinToInterrupt(SEN_RPM), FIVR_RPM, CHANGE);

    #ifdef CHK_BATTERY
      batteryLevel.checkInitialBatteryLevel();
    #endif 
  }
  else
  {
    rpmSensor.update();
    sim1.update();
    smotor1.update();
    batteryLevel.update();  
  }
}