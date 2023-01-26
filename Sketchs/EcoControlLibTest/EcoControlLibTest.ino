#include <Wire.h>
#include "./Libraries/pinDefines.cpp"
#include "./Libraries/WizNet_UDP_SR/WizNet.h"
#include "./Libraries/SDP810/sdpsensor.cpp"
#include "./Libraries/SDP810/i2chelper.cpp"
#include "./Libraries/ChipCap2/CFF_ChipCap2.cpp"
#include "./Libraries/ESP_DS18B20/DS18B20.cpp"
#include "./Libraries/ADS1219/ADS1219.cpp"
#include "./Libraries/EEPROM/AT24CM01.cpp"
#include "./Libraries/PCA9536/SparkFun_PCA9536_Arduino_Library.cpp"
#include <string.h>
#include <WiFi.h>


//defines begin

//#define MODOAP 1
#define MODOHTTPGETPOST 2
//#define MODOMQTT 3
//#define MODOSERVERCLIENTE 4
//defines ends

#ifdef MODOAP
  #include "./Libraries/AsyncTCP-master/src/AsyncTCP.h"
  #include "./Libraries/ESPAsyncWebServer-master/src/ESPAsyncWebServer.h"
  //Acces Point(wifi) config
  const char* ssid = "ESP32-AP";
  const char* password = "1234";
  #define PUERTO_WIFI_AP 80 
  AsyncWebServer server(PUERTO_WIFI_AP);
#endif
#ifdef MODOHTTPGETPOST
  #include <HTTPClient.h>
  const char* ssid = "DESARROLLO";
  const char* password = "RediDesa120#";
  //Your Domain name with URL path or IP address with path
  String serverName = "http://192.168.0.199:1880/data-get";
#endif
#ifdef MODOMQTT
  #include "./Libraries/pubsubclient-master/src/PubSubClient.cpp"
  //Para conectarse a un servidor wifi
  const char* ssid = "DESARROLLO";
  const char* password = "RediDesa120#";
  //IP de MQTT
  const char* mqtt_server = "192.168.0.167";
  WiFiClient espClient;
  PubSubClient client(espClient);
#endif
#ifdef MODOSERVERCLIENTE
  // Set web server port number to 80
  const char* ssid = "DESARROLLO";
  const char* password = "RediDesa120#";
    WiFiServer server(80);
  // Variable to store the HTTP request
  String header;
  // Current time
  unsigned long currentTime = millis();
  // Previous time
  unsigned long previousTime = 0; 
  // Define timeout time in milliseconds (example: 2000ms = 2s)
  const long timeoutTime = 2000;
#endif


long lastMsg = 0;
char msg[50];
int value = 0;

//HARDWARE Serial
HardwareSerial &usbC         = Serial;
HardwareSerial &rs485rs232   = Serial2;

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

//OneWire
DS18B20_devices ds18b20;

//BUS I2C
CFF_ChipCap2 cc2 = CFF_ChipCap2(ChipCap_1);
CFF_ChipCap2 cc3 = CFF_ChipCap2(ChipCap_2);
SDP8XXSensor sdp;
ADS1219 ads1(ADS_1);
ADS1219 ads2(ADS_2);
AT24CM01 EEPROM;
PCA9536 io;

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
  usbC.println("opto 1 Interrupt");
}

void IRAM_ATTR ISR_2() 
{
  usbC.println("opto 2 Interrupt");
}

void IRAM_ATTR ISR_3() 
{
  usbC.println("opto 3 Interrupt");
}

void IRAM_ATTR ISR_4() 
{
  usbC.println("opto 4 Interrupt");
}

void IRAM_ATTR ISR_5() 
{
  usbC.println("opto 5 Interrupt");
}
//Callbacks functions end

//functions def

void setup_wifi() 
  {
    delay(10);
    // We start by connecting to a WiFi network
    usbC.println();
    usbC.print("Connecting to ");
    usbC.println(ssid);
    WiFi.begin(ssid, password);
    /*if(WiFi.status() != WL_CONNECTED)
    {
      IPAddress  staticIP(192, 168, 0, 31);
      IPAddress  gateway(192, 168, 0, 101);
      IPAddress  subnet(255, 255, 255, 0);
      IPAddress  dns(208, 67, 220, 220);
      WiFi.config(staticIP, gateway, subnet, dns, dns);
      usbC.println("Configuration failed... trying with dynamic IP");
    }*/
    uint8_t count=0;
    while (WiFi.status() != WL_CONNECTED & count<=150) 
    {
      delay(500);
      usbC.print(".");
      count++;
    }
    if(WiFi.status() != WL_CONNECTED)
    {
      usbC.println("Configuration failed");
      usbC.println("Restarting ESP32");
      ESP.restart();
    } 
    else
    {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }
  }

#ifdef MODOHTTPGETPOST
void postHTTPstring(String dataSend)
{
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED)
  {
   
    WiFiClient client;
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // Data to send with HTTP POST
    //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";           
    // Send HTTP POST request
    //int httpResponseCode = http.POST(httpRequestData);
     
    // If you need an HTTP request with a content type: application/json, use the following:
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"value\":\""+dataSend+"\"}");

    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain");
    //int httpResponseCode = http.POST("Hello, World!");
    usbC.println("------------------------------------------------------------------------");
    usbC.println("Envio de Datos Get");
    usbC.print("HTTP Response code: ");
    usbC.println(httpResponseCode);
    rs485rs232.println("------------------------------------------------------------------------");
    rs485rs232.println("Envio de Datos Get");
    rs485rs232.print("HTTP Response code: ");
    rs485rs232.println(httpResponseCode);

    // Free resources
    http.end();
  }
  else 
  {
    usbC.println("WiFi Disconnected");
    setup_wifi();
  }
}
#endif

#ifdef MODOMQTT
  void callback(char* topic, byte* message, unsigned int length) 
  {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;
    
    for (int i = 0; i < length; i++) 
    {
      Serial.print((char)message[i]);
      messageTemp += (char)message[i];
    }
    Serial.println(" El mensaje recibido es:");
    Serial.println(messageTemp);
  }

  void reconnect() 
  {
    // Loop until we're reconnected
    while (!client.connected()) 
    {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (client.connect("ESP32Client")) 
      {
        Serial.println("connected");
        // Subscribe
        client.subscribe("esp32/output");
      } 
      else 
      {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }
#endif

void ethernetSetup() 
{
  usbC.print("\nStart UDPSendReceive on "); 
  usbC.print(BOARD_NAME);
  usbC.print(F(" with ")); 
  usbC.println(SHIELD_TYPE); 
  usbC.println(ETHERNET_GENERIC_VERSION);

  #if defined(ESP32)

    #ifndef USE_THIS_SS_PIN
      #define USE_THIS_SS_PIN   5   //22    // For ESP32
    #endif

    ETG_LOGWARN1(F("ESP32 setCsPin:"), USE_THIS_SS_PIN);
    Ethernet.init (USE_THIS_SS_PIN);
    // start the ethernet connection and the server:
    // Use DHCP dynamic IP and random mac
    uint16_t index = millis() % NUMBER_OF_MAC;

    
    if (Ethernet.begin(mac[index],10000) == 0) {
      Serial.println("Failed to obtaining an IP address using DHCP");
      // Use Static IP
      //Ethernet.begin(mac, ip, dns, gateway, subnet); 
      byte ip_static[]       = {192, 168, 0, 184};
      IPAddress ip = ip_static;
      byte dns[]      = {208, 67, 220, 220};
      byte gateway[]  = {192, 168, 0, 101};
      //Serial.println("Intento con IP estatica");
      //Ethernet.begin(mac[index],ip,dns,gateway);
    }
  #endif
 
  usbC.print(F("Using mac index = "));
  usbC.println(index);
  usbC.print(F("Connected! IP address: "));
  usbC.println(Ethernet.localIP());
  if ( (Ethernet.getChip() == w5500) || (Ethernet.getAltChip() == w5100s) )
  {
    usbC.print(F("Speed: "));    
    usbC.print(Ethernet.speedReport());
    usbC.print(F(", Duplex: ")); 
    usbC.print(Ethernet.duplexReport());
    usbC.print(F(", Link status: ")); 
    usbC.println(Ethernet.linkReport());
  }
  usbC.println(F("\nStarting connection to server..."));
  Udp.begin(localPort);
  usbC.print(F("Listening on port "));
  usbC.println(localPort);
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
    usbC.print(F("Received packet of size "));
    usbC.println(packetSize);
    usbC.print(F("From "));
    IPAddress remoteIp = Udp.remoteIP();
    usbC.print(remoteIp);
    usbC.print(F(", port "));
    usbC.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    
    if (len > 0)
    {
      packetBuffer[len] = 0;
    }

    usbC.println(F("Contents:"));
    usbC.println(packetBuffer);
    // send a reply, to the IP address and port that sent us the packet we received
    char ReplyBuffer[] = "ACK";
    sendUDPmessage(ReplyBuffer);  
  }
}

String readADS() 
{
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
  /*usbC.println("Differential result 0-1:");
  usbC.println(ads.readDifferential_0_1()*5/pow(2,23));
  delay(1000);
  usbC.println("Differential result 1-2:");
  usbC.println(ads.readDifferential_1_2()*5/pow(2,23));
  delay(1000);
  usbC.println("Differential result 2-3:");
  usbC.println(ads.readDifferential_2_3()*5/pow(2,23));
  delay(1000);
  usbC.print(".........................................\n");*/
  sprintf(Buffer1,".........................................\n ADC1\n S0= %.4f\n S1= %.4f\n S2= %.4f\n S3= %.4f \n",val1,val2,val3,val4);
  
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
  //BUS I2C Init
  Wire.begin();  
  
  //ADS INIT
  adsInit();  
  
  //USB UART INIT
  usbC.begin(115200);
  
  //Configuro pin de habilitacion para RS485/422 y seteo 
  rs485rs232.begin(115200);
  pinMode(U2TxEN, OUTPUT);
  digitalWrite(U2TxEN,HIGH);
  delay(500);
  
  #ifdef MODOAP
  //Inicio AP-WiFI
  WiFi.softAP(ssid);
  IPAddress IP = WiFi.softAPIP();
  usbC.print("AP IP address: ");
  usbC.println(IP);
  server.on("/chipcap", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/plain", readCHIPCAP2().c_str());
  });
  server.begin();
#else 
    //Conexion Wifi
    setup_wifi();
  #ifdef MODOSERVERCLIENTE
    server.begin();
  #endif
#endif

#ifdef MODOMQTT 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
#endif

  //Ethernet Setup
  ethernetSetup();
  delay(1000); // let serial console settle
  
  //DS18B20
  ds18b20.findAddrSensorsDS18B20();
  delay(1000);
  
  //ChipCap Sensors begin
  cc2.begin();
  cc3.begin();
  delay(1000);
  
  //Eprom Init
  EEPROM.begin();
  delay(500);

  //Sensor de PResion Init
  if (sdp.init() == 0) 
  {
      usbC.print("\nSDP INIT OK\n");
      rs485rs232.print("\nSDP INIT OK\n");
  } else {
      usbC.print("\nSDP INIT FAIL\n");
      rs485rs232.print("\nSDP INIT FAIL\n");
  }
  delay(1000);

  //Inicializo el PA9536 (salidas led)
  if (io.begin() == false)
  {
    Serial.println("PCA9536 not detected. Please check wiring. Freezing...");
    while (1);
  }
  io.pinMode(0, OUTPUT);
  delay(1000);
  io.pinMode(1, OUTPUT);
  delay(1000);
  io.pinMode(2, INPUT);
  delay(1000);
  io.pinMode(3, INPUT);
  delay(1000);

  //Habilito Interrupciones de entradas de OPTO
  Opto1.enableInterrupt(ISR_1, CHANGE);
  Opto2.enableInterrupt(ISR_2, CHANGE);
  Opto3.enableInterrupt(ISR_3, CHANGE);
  Opto4.enableInterrupt(ISR_4, CHANGE);
  Opto5.enableInterrupt(ISR_5, CHANGE);
  delay(500);

  //Escribo la memoria EEPROM
  //EEPROM_write();
}

void loop() 
{
  //RS485/232
  rs485rs232.print("\nRS485/422 esta funcionando\n");
  
  //Read Ethernet packet
  readpacket();
  
  #ifdef MODOHTTPGETPOST
    postHTTPstring("HTTP GET ACTIVO");
  #endif


#ifdef MODOMQTT
  //wifi Conection handler
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
#endif

#ifdef MODOSERVERCLIENTE
  WiFiClient client = server.available(); 
  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {

            } else if (header.indexOf("GET /26/off") >= 0) {


            } else if (header.indexOf("GET /27/on") >= 0) {


            } else if (header.indexOf("GET /27/off") >= 0) 
            {


            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>GPIO 26 - State </p>");
            // If the output26State is off, it displays the ON button       
            client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>GPIO 27 - State </p>");
            // If the output27State is off, it displays the ON button       

            client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
#endif

  //CHIPACAP2
  readCHIPCAP2().toCharArray(ReplyBuffer, 300);
  usbC.print(ReplyBuffer);
  delay(200);
  rs485rs232.print(ReplyBuffer);
  delay(200);
  sendUDPmessage(ReplyBuffer);
  delay(200);
  #ifndef MODOAP
    #ifdef MODOHTTPGETPOST
      postHTTPstring(String(cc2.temperatureC));
    #endif
    #ifdef MODOMQTT
      client.publish("esp32/chipcap2", String(cc2.temperatureC));
    #endif
  #endif

  //SDP Pressure
  usbC.print(readPressure());
  delay(200);
  readPressure().toCharArray(ReplyBuffer, 300);
  sendUDPmessage(ReplyBuffer);
  delay(200);
  rs485rs232.print(ReplyBuffer);
  delay(200);
  #ifndef MODOAP
    #ifdef MODOHTTPGETPOST
      postHTTPstring(String(sdp.getDifferentialPressure()));
    #endif
    #ifdef MODOMQTT
      client.publish("esp32/SDP", sdp.getDifferentialPressure());
    #endif    
  #endif

  //DS18b20
  ds18b20.printAdressAndTemp();
  sprintf(ReplyBuffer,"--------------------------------\nTemp1: %.2f\nTemp 2: %.2f\n --------------------------------\n",ds18b20.TempCArray[0],ds18b20.TempCArray[1]);
  sendUDPmessage(ReplyBuffer);
  delay(200);
  #ifndef MODOAP
    #ifdef MODOHTTPGETPOST
      postHTTPstring(String(ds18b20.TempCArray[0]));
    #endif
    #ifdef MODOMQTT
      client.publish("esp32/DS18B20", String(ds18b20.TempCArray[0]));
    #endif
  #endif

  //EEPROM Read
  EEPROM_read().toCharArray(ReplyBuffer, 300);
  usbC.print(ReplyBuffer);
  delay(200);
  rs485rs232.print(ReplyBuffer);
  delay(200);
  sendUDPmessage(ReplyBuffer);
  delay(200);
  #ifndef MODOAP
    #ifdef MODOHTTPGETPOST
      //postHTTPstring(ReplyBuffer);
    #endif
    #ifdef MODOMQTT
      client.publish("esp32/EEPROM", ReplyBuffer);
    #endif   
  #endif

  //ADS value Read
  readADS().toCharArray(ReplyBuffer, 300);
  usbC.print(ReplyBuffer);
  delay(200);
  sendUDPmessage(ReplyBuffer);
  delay(200);
  rs485rs232.print(ReplyBuffer);
  delay(200);
  #ifndef MODOAP
    #ifdef MODOHTTPGETPOST
      postHTTPstring(String(ads1.readSingleEnded(0)*5/pow(2,23)));
    #endif
    #ifdef MODOMQTT
      client.publish("esp32/ADS", String(ads1.readSingleEnded(0)*5/pow(2,23)));
    #endif  
  #endif

  //LEDS on
  io.write(0, LOW);
  delay(500);
  io.write(1, LOW);
  delay(500);

  //Reles toggle
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

  //LEDS ON
  io.write(0, HIGH);
  delay(500);
  io.write(1, HIGH);
  delay(500);

  //OPTOS
  usbC.print("OPTO1 : ");
  usbC.println(Opto1.ReadOpto());
  usbC.print("OPTO2 : ");
  usbC.println(Opto2.ReadOpto());
  usbC.print("OPTO3 : ");
  usbC.println(Opto3.ReadOpto());  
  usbC.print("OPTO4 : ");
  usbC.println(Opto4.ReadOpto());
  usbC.print("OPTO5 : ");
  usbC.println(Opto5.ReadOpto());
}
