#include "DS18B20.h"

DS18B20_devices::DS18B20_devices() 
{
	OneWire oneWireSens(ONE_WIRE_DS18B20_PIN);
	DallasTemperature sensors(&oneWireSensores);
	this->oneWireSensores = oneWireSens;
	this->sensores = sensors;
}

DS18B20_devices::DS18B20_devices(uint8_t BUS_PIN) 
{
	OneWire oneWireSens(BUS_PIN);
	DallasTemperature sensors(&oneWireSensores);
	this->oneWireSensores = oneWireSens;
	this->sensores = sensors;
}

Search_ADRESS_DS18B20 DS18B20_devices::findAddrSensorsDS18B20() 
{	
	byte addr[8];
	DeviceAddress sensorAux;
	DevAddrMatriz sensorNumber;
	uint8_t i;
	uint8_t jmax=0;
	
	while (this->oneWireSensores.search(addr)) 
	{
		memcpy(sensorAux, addr, 8);
		memcpy(sensorNumber[jmax] , sensorAux, 8);
		memcpy(this->sensoresArray[], sensorNumber, 8);
    for (int i=0;i<8;i++)
    	{
			  Serial.print("0x");
        Serial.print(this->sensoresArray[jmax][i],HEX);
			  Serial.print(" ");
    	}
    Serial.println("");
		jmax=jmax+1;
	}
	this->cantidad_Sensores=jmax;
	this->oneWireSensores.reset_search();
	return REQ_Devices_Complete;  
}

Temp_Request_Status DS18B20_devices::getTemp() 
{	
	byte n;
	Temp_Request_Status status=REQUESTING_TEMP;
	this->sensores.requestTemperatures(); // Send the command to get temperatures
	for(uint8_t n=0; n<this->cantidad_Sensores; n++)
	{
		this->TempCArray[n]=this->sensores.getTempC(this->sensoresArray[n]);
    Serial.println(TempCArray[n]);
	}
	return REQUESTING_TEMP_DONE;
}

uint8_t DS18B20_devices::printAdressAndTemp() 
{	
	for (int j=0;j<this->cantidad_Sensores;j++)
  	{
		  Serial.print(j);
		  Serial.print(" ");
    	for (int i=0;i<8;i++)
    	{
			  Serial.print("0x");
        Serial.print(this->sensoresArray[j][i],HEX);
			  Serial.print(" ");
    	}
		  Serial.print(this->sensores.getTempC(this->sensoresArray[j]));
		  Serial.println("");
		  delay(1000);
  	}
	return 0;
}