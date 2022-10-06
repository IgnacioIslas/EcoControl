#include "DS18B20.h"
#include "pinDefines.h"

//Dispositivos DS18B20
DS18B20_devices ds18b20;

void setup()
{
  Serial.begin(115200);
  ds18b20.findAddrSensorsDS18B20();
};
void loop()
{
  //ds18b20.getTemp();
  delay(1000);
  //ds18b20.printAdressAndTemp();
};