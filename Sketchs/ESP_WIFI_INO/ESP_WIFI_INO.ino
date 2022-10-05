#include "ESP_WIFI.h"

wifi_msg mensaje;

//Variabless no se porque no las puedo declarar en el .h
//info del peer
esp_now_peer_info_t InfoDelPeer;
uint8_t MacAddress1[] = {0x24, 0x6F, 0x28, 0x48, 0xC9, 0x14};
uint8_t MacAddress2[] = {0x24, 0x6F, 0x28, 0x48, 0xC8, 0xB0};

void setup() {
  // put your setup code here, to run once:
  mensaje.a[0]='H';
  mensaje.b=1;
  mensaje.c=1;
  mensaje.d=true;

  check_Init_ESP_WIFI();
  Serial.begin(115200);
  InfoDelPeer = checkConnect_Send(InfoDelPeer,(uint8_t *) &MacAddress1); 
  esp_now_register_recv_cb(OnDataRecv);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hola Mi Mac es: "+GetLocalWifiMAC());
  delay(2000);
  SendData(MacAddress1,mensaje);
  
}

