//////////////////////////////////////////////////
//////////////////////////////////////////////////
////////////// Autor:Ignacio Islas 	//////////////
//////////////		  4/21/22			    //////////////
//////////////	    EcoControl		  //////////////
//////////////	   ESP_WIFI.h		    //////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////

#ifndef ESP_WIFI_h
#define ESP_WIFI_h

//Include
#include <Arduino.h>
#include <esp_now.h>
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

struct wifi_msg 
{
  char a[32];
  int b;
  float c;
  bool d;
};

typedef struct wifi_msg wifi_message;

//Funciones
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) ;
bool check_Init_ESP_WIFI();
bool SendData(uint8_t *broadcastAddress,wifi_message myData);
esp_now_peer_info_t checkConnect_Send(esp_now_peer_info_t,uint8_t * broadcastAddress);
String GetLocalWifiMAC();

#endif