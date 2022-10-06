#include <Arduino.h>

#include <Wire.h> 
#include "CC2D25-SIP.h"

CFF_ChipCap2 cc2 = CFF_ChipCap2(0x28);
CFF_ChipCap2 cc3 = CFF_ChipCap2(0x22);

byte scanI2CBus() {
  Wire.begin();
  Serial.println("\nI2C Scanner");
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  Wire.endTransmission();
  delay(5000);
  return address;
}

void setup() 
{
  Serial.begin(115200);
  
  scanI2CBus();
  //cc2.Init_begin();
  //cc3.Init_begin();
}

void loop()
{

  
  //cc2.ChangeAddr(0x22);
  cc2.startNormalMode();
  cc3.startNormalMode();
  delay(100);
  while (1)
  {
    if (cc2.dataReady() == true)
    {
      cc2.readSensor();
      Serial.println("0x28");
      Serial.print("Humidity: ");
      Serial.print(cc2.humidity);
      Serial.print("\n");
  
      Serial.print("Temperature C: ");
      Serial.print(cc2.temperatureC);
      Serial.print("\n");
    }
    if (cc3.dataReady() == true)
    {
      cc3.readSensor();
      Serial.println("0x22");
      Serial.print("Humidity: ");
      Serial.print(cc3.humidity);
      Serial.print("\n");
  
      Serial.print("Temperature C: ");
      Serial.print(cc3.temperatureC);
      Serial.print("\n");
    }
    
    delay(5000);
  }
}