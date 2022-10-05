//////////////////////////////////////////////////
//////////////////////////////////////////////////
////////////// Autor:Ignacio Islas 	//////////////
//////////////		4/21/22			      //////////////
//////////////	  EcoControl		    //////////////
//////////////	  ESP_WIFI.cpp		 //////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////

//Include
#include "ESP_WIFI.h"

// callback cuando el dato es enviado
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//callback function cuando el dato es recivido
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) 
{
  wifi_message myData;
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myData.a);
  Serial.print("Int: ");
  Serial.println(myData.b);
  Serial.print("Float: ");
  Serial.println(myData.c);
  Serial.print("Bool: ");
  Serial.println(myData.d);
  Serial.println();
}

//Para obtener la MAC LOCAL
String GetLocalMAC()
{
  WiFi.mode(WIFI_MODE_STA);
  return (WiFi.macAddress());
}

//Chequeo de conexion
bool check_Init_ESP_WIFI()
{
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
  }
  return esp_now_init();
}

//Agrego peer y activo callbback
esp_now_peer_info_t checkConnect_Send(esp_now_peer_info_t peerInfo,uint8_t * broadcastAddress)
{
  // Luego del Init registro para env Cb
  esp_now_register_send_cb(OnDataSent);
  // registro peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // Agrego peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
  }
  return peerInfo;
}

bool SendData(uint8_t *broadcastAddress,wifi_message myData)
{
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) 
  {
    Serial.println("Sent with success");
  }
  else 
  {
    Serial.println("Error sending the data");
  }
}
