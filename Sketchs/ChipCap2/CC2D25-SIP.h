//////////////////////////////////////////////////
//////////////////////////////////////////////////
////////////// Autor:Ignacio Islas 	//////////////
//////////////		4/21/22			//////////////
//////////////	  EcoControl		//////////////
//////////////	  CC2D25-SIP.h		//////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////

#ifndef CC2D25-SIP_h
#define CC2D25-SIP_h

#include <Wire.h>

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#define CFF_CHIPCAP2_DEFAULT_ADDR 0x28
#define CCF_CHIPCAP2_STATUS_VALID 0
#define CCF_CHIPCAP2_STATUS_STALE 1
#define CCF_CHIPCAP2_STATUS_COMMANDMODE 2
#define CCF_CHIPCAP2_COMMAND_BUSY 0
#define CCF_CHIPCAP2_COMMAND_SUCCESS 1
#define CCF_CHIPCAP2_COMMAND_ERROR 2
#define CCF_CHIPCAP2_EEPROM_ERROR_CORRECTED 2
#define CCF_CHIPCAP2_EEPROM_ERROR_UNCORRECTED 4
#define CCF_CHIPCAP2_EEPROM_ERROR_RAM 8
#define CCF_CHIPCAP2_EEPROM_ERROR_CONFIG 16
#define CCF_CHIPCAP2_READY_PIN 2
#define CCF_CHIPCAP2_ALARMLOW_PIN 3
#define CCF_CHIPCAP2_ALARMHIGH_PIN 4


////////////////////////////////////////
// ChipCap2 Digital I2C Class
////////////////////////////////////////
class CFF_ChipCap2
{
    public:
        CFF_ChipCap2(uint8_t addr = CFF_CHIPCAP2_DEFAULT_ADDR);
        void configPowerPin(uint8_t pwrpin = NULL);
        void configReadyPin(uint8_t readypin = CCF_CHIPCAP2_READY_PIN);
        void configAlarmLowPin(uint8_t alowpin = CCF_CHIPCAP2_ALARMLOW_PIN);
        void configAlarmHighPin(uint8_t ahighpin = CCF_CHIPCAP2_ALARMHIGH_PIN);
		void power(uint8_t onOff);
        void startCommandMode(void);
        void startNormalMode(void);
        void begin(void);
        void readSensor(void);
        bool dataReady(void);
        bool checkAlarmLow(void);
        void setAlarmLowVal(int16_t alarmlowon);
        int16_t getAlarmLowVal(void);
        bool checkAlarmHigh(void);
        void setAlarmHighVal(int16_t alarmhighon);
        int16_t getAlarmHighVal(void);
        uint8_t status;
        uint8_t ready;
        float humidity;
        float temperatureC;
        float temperatureF;
		void CFF_ChipCap2::ChangeAddr(uint8_t NEW_I2C_ADDRESS);
        
    private:
        uint8_t _i2caddr;
        uint8_t _pwrpin;
        uint8_t _readypin;
        uint8_t _alowpin;
        uint8_t _ahighpin;

};