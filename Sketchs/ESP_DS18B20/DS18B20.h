//////////////////////////////////////////////////
//////////////////////////////////////////////////
////////////// Autor:Ignacio Islas 	//////////////
//////////////		  4/21/22			    //////////////
//////////////	    EcoControl		  //////////////
//////////////	   DS18B20.h		    //////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////

#ifndef DS18B20_h
#define DS18B20_h

//INCLUDES
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//Define
#define ONE_WIRE_BUS_PIN 4//DS18B20_PIN

//types
typedef DeviceAddress DevAddrMatriz[10];

//enum
enum Temp_Request_Status 
{   
  REQUESTING_TEMP, 
  REQUESTING_TEMP_DONE,
  REQUESTING_TEMP_ERROR
};

enum Search_ADRESS_DS18B20 
{   
  REQ_Devices, 
  REQ_Devices_Complete,
  REQ_Devices_Error
};

//variables 

class DS18B20_devices 
{
  private:
    OneWire oneWireSensores;
    DallasTemperature sensores;
    float TempCArray[10];
  public:
    uint8_t cantidad_Sensores;
    DeviceAddress sensoresArray[10];
    void _init_search(uint8_t BUS_PIN);
    Search_ADRESS_DS18B20 findAddrSensorsDS18B20();
    Temp_Request_Status getTemp(); 
    uint8_t printAdressAndTemp() ;
};

//Funciones

#endif