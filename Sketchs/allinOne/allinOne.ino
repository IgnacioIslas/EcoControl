#include <Wire.h>
#include "pinDefines.h"
#include "defines.h"
#include "sdpsensor.h"
#include "CFF_ChipCap2.h"
#include "DS18B20.h"
#include "ADS1219.h"
#include "AT24CM01.h"


#define rst 14
#define drdy 15
#define INDEX_SIZE 6


// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

//OneWire
DS18B20_devices ds18b20;
//BUS I2C
CFF_ChipCap2 cc2 = CFF_ChipCap2(0x28);
CFF_ChipCap2 cc3 = CFF_ChipCap2(0x22);
SDP8XXSensor sdp;
ADS1219 ads(drdy,0x40);
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

//Para testear la EEPROM
char array[INDEX_SIZE] = "12345";    // arrays values to be stored
int ibuff = 2839;                    // int value to be stored
float fbuff = -245.2049;             // float value to be stored

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
  Serial.print("\nStart UDPSendReceive on "); Serial.print(BOARD_NAME);
  Serial.print(F(" with ")); Serial.println(SHIELD_TYPE); 
  Serial.println(ETHERNET_GENERIC_VERSION);
  
  #if (USING_SPI2)
    #if defined(CUR_PIN_MISO)
      ETG_LOGWARN(F("Default SPI pinout:"));
      ETG_LOGWARN1(F("MOSI:"), CUR_PIN_MOSI);
      ETG_LOGWARN1(F("MISO:"), CUR_PIN_MISO);
      ETG_LOGWARN1(F("SCK:"),  CUR_PIN_SCK);
      ETG_LOGWARN1(F("SS:"),   CUR_PIN_SS);
      ETG_LOGWARN(F("========================="));
    #endif
  #else
    ETG_LOGWARN(F("Default SPI pinout:"));
    ETG_LOGWARN1(F("MOSI:"), MOSI);
    ETG_LOGWARN1(F("MISO:"), MISO);
    ETG_LOGWARN1(F("SCK:"),  SCK);
    ETG_LOGWARN1(F("SS:"),   SS);
    ETG_LOGWARN(F("========================="));
  #endif

  #if defined(ESP32)

    // You can use Ethernet.init(pin) to configure the CS pin
    //Ethernet.init(10);  // Most Arduino shields
    //Ethernet.init(5);   // MKR ETH shield
    //Ethernet.init(0);   // Teensy 2.0
    //Ethernet.init(20);  // Teensy++ 2.0
    //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
    //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

    #ifndef USE_THIS_SS_PIN
      #define USE_THIS_SS_PIN   5   //22    // For ESP32
    #endif

    ETG_LOGWARN1(F("ESP32 setCsPin:"), USE_THIS_SS_PIN);

    // Must use library patch for Ethernet, EthernetLarge libraries
    // ESP32 => GPIO2,4,5,13,15,21,22 OK with Ethernet, Ethernet2, EthernetLarge
    // ESP32 => GPIO2,4,5,15,21,22 OK with Ethernet3

    //Ethernet.setCsPin (USE_THIS_SS_PIN);
    Ethernet.init (USE_THIS_SS_PIN);
    

    // start the ethernet connection and the server:
    // Use DHCP dynamic IP and random mac
    uint16_t index = millis() % NUMBER_OF_MAC;
    // Use Static IP
    //Ethernet.begin(mac[index], ip);
    Ethernet.begin(mac[index]);

    //SPIClass SPI2(HSPI);
    //Ethernet.begin(mac[index], &SPI2);

    // Just info to know how to connect correctly
    // To change for other SPI
  #endif
  #if defined(CUR_PIN_MISO)
    ETG_LOGWARN(F("Currently Used SPI pinout:"));
    ETG_LOGWARN1(F("MOSI:"), CUR_PIN_MOSI);
    ETG_LOGWARN1(F("MISO:"), CUR_PIN_MISO);
    ETG_LOGWARN1(F("SCK:"),  CUR_PIN_SCK);
    ETG_LOGWARN1(F("SS:"),   CUR_PIN_SS);
    ETG_LOGWARN(F("========================="));
  #else
    ETG_LOGWARN(F("Currently Used SPI pinout:"));
    ETG_LOGWARN1(F("MOSI:"), MOSI);
    ETG_LOGWARN1(F("MISO:"), MISO);
    ETG_LOGWARN1(F("SCK:"),  SCK);
    ETG_LOGWARN1(F("SS:"),   SS);
    ETG_LOGWARN(F("========================="));
  #endif

  Serial.print(F("Using mac index = "));
  Serial.println(index);

  Serial.print(F("Connected! IP address: "));
  Serial.println(Ethernet.localIP());

  if ( (Ethernet.getChip() == w5500) || (Ethernet.getAltChip() == w5100s) )
  {
    Serial.print(F("Speed: "));    Serial.print(Ethernet.speedReport());
    Serial.print(F(", Duplex: ")); Serial.print(Ethernet.duplexReport());
    Serial.print(F(", Link status: ")); Serial.println(Ethernet.linkReport());
  }

  Serial.println(F("\nStarting connection to server..."));
  // if you get a connection, report back via serial:
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
    sprintf(ReplyBuffer,"ACK"); 
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
}

void readADS() {
  float val1 = (float) ads.readSingleEnded(0)*5/pow(2,23);
  delay(500);
  float val2 = (float) ads.readSingleEnded(1)*5/pow(2,23);
  delay(500);
  float val3 = (float) ads.readSingleEnded(2)*5/pow(2,23);
  delay(500);
  float val4 = (float) ads.readSingleEnded(3)*5/pow(2,23);
  delay(500);
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
  sprintf(ReplyBuffer,".........................................\n S0= %.4f\n S1= %.4f\n S2= %.4f\n S3= %.4f \n",val1,val2,val3,val4);
  Serial.println(ReplyBuffer);
}

void adsInit()
{
  ads.begin();
  ads.setVoltageReference(REF_EXTERNAL);
  ads.setConversionMode(SINGLE_SHOT);
  ads.setDataRate(20);
  ads.setGain(ONE);
}

void readCHIPCAP2()
{
  cc2.readSensor();
  delay(100);
  cc3.readSensor();
  sprintf(ReplyBuffer,"--------------------------------\nCC2 sensor\nHumidity: %.2f\nTemperature C: %.2f\n --------------------------------\nCC3 sensor\nHumidity: %.2f\nTemperature C: %.2f\n",cc2.humidity,cc2.temperatureC,cc3.humidity,cc3.temperatureC);
  Serial.print(ReplyBuffer);
}

void readPressure() 
{
  int ret = sdp.readSample();
  if (ret == 0) {
      sprintf(ReplyBuffer,"--------------------------------\nDifferential pressure:  %.2f Pa | Temperature: %.2f C\n.........................................\n",sdp.getDifferentialPressure(),sdp.getTemperature());
      Serial.print(ReplyBuffer);
  } else {
      Serial.print("Error in readSample(), ret = ");
      Serial.println(ret);
  }
  delay(500);
}

void EEPROM_test() {
  EEPROM.begin();
  Serial.print("\n\n .........................................");
  Serial.print("\n\n ...........Reading Variables.............");

  /* int and float values ----------------------------------------------- */
  Serial.print("\n\nRecalling Int\n");
  //EEPROM.writeInt(0, ibuff);                            // WriteVartype(eeprom address, value)
  Serial.print(EEPROM.readInt(0));                      // EEPROM.readVarType(eeprom address)
                                                    
  Serial.print("\nRecalling Float\n");
  //EEPROM.writeFloat(10, fbuff);
  Serial.print(EEPROM.readFloat(10), 7);                // EEPROM.readVarType(eeprom address), print with 7 decimal place resolution

  /* Array valuess ---------------------------------- */
  Serial.print("\nRecalling Array\n");
  //EEPROM.writeCharArray(20, array, INDEX_SIZE-1);        // EEPROM.writeArrayType(address, array, arraysize)  
  EEPROM.readCharArray(20, array, INDEX_SIZE-1);         // ReadArrayType(address, textarraybuffer, textsize)
  Serial.print(array);
  Serial.print("\n\n .........................................");
}


void setup() {
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
  if (sdp.init() == 0) 
  {
      Serial.print("\nSDP INIT OK\n");
  } else {
      Serial.print("\nSDP INIT FAIL\n");
  }
  EEPROM_test(); //Para testear la EEPROM
  delay(1000);
  Opto1.enableInterrupt(ISR_1, CHANGE);
  Opto2.enableInterrupt(ISR_2, CHANGE);
  Opto3.enableInterrupt(ISR_3, CHANGE);
  Opto4.enableInterrupt(ISR_4, CHANGE);
  Opto5.enableInterrupt(ISR_5, CHANGE);
}

void loop() 
{
  readpacket();
  readCHIPCAP2();
  sendUDPmessage(ReplyBuffer);
  readPressure();
  sendUDPmessage(ReplyBuffer);
  ds18b20.printAdressAndTemp();
  ds18b20.getTemp();
  sprintf(ReplyBuffer,"--------------------------------\nTemp1: %.2f\nTemp 2: %.2f\n --------------------------------\n",ds18b20.TempCArray[0],ds18b20.TempCArray[1]);
  sendUDPmessage(ReplyBuffer);
  readADS();
  sendUDPmessage(ReplyBuffer);

  Rele1.toggle();
  delay(500);
  Rele2.toggle();
  delay(500);
  Rele3.toggle();
  delay(500);
  Rele4.toggle();
  delay(500);
  Rele5.toggle();
  delay(4000);
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
