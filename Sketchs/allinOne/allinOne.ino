#include <Wire.h>
#include "pinDefines.h"
#include "WizNet.h"
#include "sdpsensor.h"
#include "CFF_ChipCap2.h"
#include "DS18B20.h"
#include "ADS1219.h"
#include "AT24CM01.h"

#include <string.h>


#define rst 14

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

//OneWire
DS18B20_devices ds18b20;

//BUS I2C
CFF_ChipCap2 cc2 = CFF_ChipCap2(0x28);
CFF_ChipCap2 cc3 = CFF_ChipCap2(0x22);
SDP8XXSensor sdp;
ADS1219 ads1(0x40);
ADS1219 ads2(0x41);
AT24CM01 EEPROM;

//Reles
Reless Rele1 = Reless(Rele1OUT);
Reless Rele2 = Reless(Rele2OUT);
Reless Rele3 = Reless(Rele3OUT);
Reless Rele4 = Reless(Rele4OUT);
Reless Rele5 = Reless(Rele5OUT);

//Optos
Opto Opto1 = Opto(OPTO_1_PIN);
Opto Opto2 = Opto(OPTO_2_PIN);
Opto Opto3 = Opto(OPTO_3_PIN);
Opto Opto4 = Opto(OPTO_4_PIN);
Opto Opto5 = Opto(OPTO_5_PIN);


//Modulo ETHERNET
unsigned int localPort = 1883;    //10002;  // local port to listen on
char packetBuffer[255];          // buffer to hold incoming packet
char ReplyBuffer[] = "ACK";      // a string to send back

//Callbacks functions Opto begin
void IRAM_ATTR ISR_1() 
{
  Serial.println("opto 1 Interrupt");
}

void IRAM_ATTR ISR_2() 
{
  Serial.println("opto 2 Interrupt");
}

void IRAM_ATTR ISR_3() 
{
  Serial.println("opto 3 Interrupt");
}

void IRAM_ATTR ISR_4() 
{
  Serial.println("opto 4 Interrupt");
}

void IRAM_ATTR ISR_5() 
{
  Serial.println("opto 5 Interrupt");
}
//Callbacks functions end


void ethernetSetup() 
{
  Serial.print("\nStart UDPSendReceive on "); 
  Serial.print(BOARD_NAME);
  Serial.print(F(" with ")); 
  Serial.println(SHIELD_TYPE); 
  Serial.println(ETHERNET_GENERIC_VERSION);

  #if defined(ESP32)

    #ifndef USE_THIS_SS_PIN
      #define USE_THIS_SS_PIN   5   //22    // For ESP32
    #endif

    ETG_LOGWARN1(F("ESP32 setCsPin:"), USE_THIS_SS_PIN);
    Ethernet.init (USE_THIS_SS_PIN);
    // start the ethernet connection and the server:
    // Use DHCP dynamic IP and random mac
    uint16_t index = millis() % NUMBER_OF_MAC;
    // Use Static IP
    //Ethernet.begin(mac[index], ip);
    Ethernet.begin(mac[index]);
  #endif
 
  Serial.print(F("Using mac index = "));
  Serial.println(index);
  Serial.print(F("Connected! IP address: "));
  Serial.println(Ethernet.localIP());
  if ( (Ethernet.getChip() == w5500) || (Ethernet.getAltChip() == w5100s) )
  {
    Serial.print(F("Speed: "));    
    Serial.print(Ethernet.speedReport());
    Serial.print(F(", Duplex: ")); 
    Serial.print(Ethernet.duplexReport());
    Serial.print(F(", Link status: ")); 
    Serial.println(Ethernet.linkReport());
  }
  Serial.println(F("\nStarting connection to server..."));
  Udp.begin(localPort);
  Serial.print(F("Listening on port "));
  Serial.println(localPort);
}

void sendUDPmessage(char* mensajee)
{
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write(mensajee);
  Udp.endPacket();
}

void readpacket()
{
    // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Serial.print(F("Received packet of size "));
    Serial.println(packetSize);
    Serial.print(F("From "));
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(F(", port "));
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    
    if (len > 0)
    {
      packetBuffer[len] = 0;
    }

    Serial.println(F("Contents:"));
    Serial.println(packetBuffer);
    // send a reply, to the IP address and port that sent us the packet we received
    char ReplyBuffer[] = "ACK";
    sendUDPmessage(ReplyBuffer);  
  }
}

String readADS() {
  char Buffer1[300];
  //Leo adc 1
  float val1 = (float) ads1.readSingleEnded(0)*5/pow(2,23);
  delay(100);
  float val2 = (float) ads1.readSingleEnded(1)*5/pow(2,23);
  delay(100);
  float val3 = (float) ads1.readSingleEnded(2)*5/pow(2,23);
  delay(100);
  float val4 = (float) ads1.readSingleEnded(3)*5/pow(2,23);
  delay(100);
  /*Serial.println("Differential result 0-1:");
  Serial.println(ads.readDifferential_0_1()*5/pow(2,23));
  delay(1000);
  Serial.println("Differential result 1-2:");
  Serial.println(ads.readDifferential_1_2()*5/pow(2,23));
  delay(1000);
  Serial.println("Differential result 2-3:");
  Serial.println(ads.readDifferential_2_3()*5/pow(2,23));
  delay(1000);
  Serial.print(".........................................\n");*/
  sprintf(Buffer1,".........................................\n ADC1\nS0= %.4f\n S1= %.4f\n S2= %.4f\n S3= %.4f \n",val1,val2,val3,val4);
  
  char Buffer2[300];
  //Leo adC2
  val1 = (float) ads2.readSingleEnded(0)*5/pow(2,23);
  delay(100);
  val2 = (float) ads2.readSingleEnded(1)*5/pow(2,23);
  delay(100);
  val3 = (float) ads2.readSingleEnded(2)*5/pow(2,23);
  delay(100);
  val4 = (float) ads2.readSingleEnded(3)*5/pow(2,23);
  delay(100);
  sprintf(Buffer2,".........................................\n ADC2\n S0= %.4f\n S1= %.4f\n S2= %.4f\n S3= %.4f \n",val1,val2,val3,val4);
  strcat(Buffer1,Buffer2);
  return Buffer1;
}

void adsInit()
{
  //adc1
  ads1.begin();
  ads1.setVoltageReference(REF_EXTERNAL);
  ads1.setConversionMode(SINGLE_SHOT);
  ads1.setDataRate(20);
  ads1.setGain(ONE);
  //adc 2
  ads2.begin();
  ads2.setVoltageReference(REF_EXTERNAL);
  ads2.setConversionMode(SINGLE_SHOT);
  ads2.setDataRate(20);
  ads2.setGain(ONE);
}

String readCHIPCAP2()
{
  char stringCHIPCAP2[300];
  cc2.readSensor();
  delay(100);
  cc3.readSensor();
  delay(100);
  sprintf(stringCHIPCAP2,"--------------------------------\nCC2 sensor\nHumidity: %.2f\nTemperature C: %.2f\n --------------------------------\nCC3 sensor\nHumidity: %.2f\nTemperature C: %.2f\n",cc2.humidity,cc2.temperatureC,cc3.humidity,cc3.temperatureC);
  String stringCHCP(stringCHIPCAP2);
  return stringCHCP;
}

String readPressure() 
{
  char stringPreassure[300];
  int ret = sdp.readSample();
  if (ret == 0) 
  {
      sprintf(stringPreassure,"--------------------------------\nDifferential pressure:  %.2f Pa | Temperature: %.2f C\n.........................................\n",sdp.getDifferentialPressure(),sdp.getTemperature());
      return stringPreassure;
  } 
  else 
  {
      sprintf(stringPreassure,"--------------------------------\nError in readSample()/r/n");
      return stringPreassure;
  }
}

void EEPROM_write() 
{
  char chBuff[6] = "HOLAA";    // arrays values to be stored
  int intBuff = 2111;                    // int value to be stored
  float floatBuff = 310.279;             // float value to be stored
  //Guardo Variables
  //Int
  EEPROM.writeInt(0, intBuff);                            // WriteVartype(eeprom address, value)
  //Float
  EEPROM.writeFloat(10, floatBuff);
  //char Array
  EEPROM.writeCharArray(20, chBuff, 6-1);        // EEPROM.writeArrayType(address, array, arraysize)
}

String EEPROM_read() 
{
  char stringArray[300];
  #define INDEX_SIZE 6
  //Para testear la EEPROM
  char array[INDEX_SIZE] = "HOLAA";    // arrays values to be stored
  int ibuff = 2111;                    // int value to be stored
  float fbuff = -310.2799;             // float value to be stored

  EEPROM.begin();

  //Leo Variables
  int numeroEntero  = EEPROM.readInt(0);
  float numeroFloat = EEPROM.readFloat(10);
  EEPROM.readCharArray(20, array, 6-1);         // ReadArrayType(address, textarraybuffer, textsize)
  sprintf(stringArray,"\n\n .........................................\n\n ...........Reading Variables.............\nLeo Int: %d\nLeo Float: %.4f\nLeo charArray: %s\n",numeroEntero,numeroFloat,array);
  return stringArray;
}


void setup() 
{
  Wire.begin();  
  adsInit();  
  Serial.begin(115200);
  delay(500);
  ethernetSetup();
  delay(1000); // let serial console settle
  ds18b20.findAddrSensorsDS18B20();
  delay(1000);
  cc2.begin();
  cc3.begin();
  delay(1000);
  EEPROM.begin();
  delay(500);
  if (sdp.init() == 0) 
  {
      Serial.print("\nSDP INIT OK\n");
  } else {
      Serial.print("\nSDP INIT FAIL\n");
  }
  delay(1000);
  Opto1.enableInterrupt(ISR_1, CHANGE);
  Opto2.enableInterrupt(ISR_2, CHANGE);
  Opto3.enableInterrupt(ISR_3, CHANGE);
  Opto4.enableInterrupt(ISR_4, CHANGE);
  Opto5.enableInterrupt(ISR_5, CHANGE);
  delay(500);
  EEPROM_write();
}

void loop() 
{
  readpacket();
  Serial.print(readCHIPCAP2());
  //sendUDPmessage(ReplyBuffer);
  Serial.print(readPressure());
  //sendUDPmessage(ReplyBuffer);
  ds18b20.printAdressAndTemp();
  sprintf(ReplyBuffer,"--------------------------------\nTemp1: %.2f\nTemp 2: %.2f\n --------------------------------\n",ds18b20.TempCArray[0],ds18b20.TempCArray[1]);
  //sendUDPmessage(ReplyBuffer);
  Serial.print(EEPROM_read());
  Serial.print(readADS());
  //sendUDPmessage(ReplyBuffer);

  Rele1.toggle();
  delay(1000);
  Rele2.toggle();
  delay(1000);
  Rele3.toggle();
  delay(1000);
  Rele4.toggle();
  delay(1000);
  Rele5.toggle();
  delay(1000);
  Serial.print("OPTO1 : ");
  Serial.println(Opto1.ReadOpto());
  Serial.print("OPTO2 : ");
  Serial.println(Opto2.ReadOpto());
  Serial.print("OPTO3 : ");
  Serial.println(Opto3.ReadOpto());  
  Serial.print("OPTO4 : ");
  Serial.println(Opto4.ReadOpto());
  Serial.print("OPTO5 : ");
  Serial.println(Opto5.ReadOpto());
}
