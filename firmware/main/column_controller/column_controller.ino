/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

// BOARD: Adafruit ESP32 Feather

#include "src/components/cart/cart.h"

#include "src/components/configuration/configuration.h"

#include "WiFi.h"
#include "WiFiUdp.h"
#include "AsyncTCP.h" // https://github.com/me-no-dev/AsyncTCP

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

volatile bool button0 = false;
volatile bool wifi_connected = false;
volatile bool client_connected = false;
IPAddress IP;
IPAddress ServerIP;
WiFiUDP udp;
AsyncClient client;

void onClientConnected(void* arg, AsyncClient* client){
  client_connected = true;
  DEBUG_PORT.println("Client connected to server");
}

void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          IP = WiFi.localIP();
          DEBUG_PORT.print("WiFi connected! IP address: ");
          DEBUG_PORT.println(IP);  
          udp.begin(IP, NETWORK_UDP_PORT);
          wifi_connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          DEBUG_PORT.println("WiFi lost connection");
          wifi_connected = false;
          // todo: handle lost connections properly (re-establish)
          break;
      default: break;
    }
}

void IRAM_ATTR button0ISR() {
  button0 = true;
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);

  pinMode(0, INPUT_PULLUP);
  attachInterrupt(0, button0ISR, RISING);

  WiFi.disconnect(true);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);

  client.onConnect(onClientConnected, NULL);
  while(!client_connected){
    if(wifi_connected){
      if(!udp.parsePacket()){ continue; }

      ServerIP = udp.remoteIP();
      uint16_t ServerPort = udp.remotePort();
      uint32_t len = udp.available();

      char payload[len+1];
      memset(payload, 0x00, len+1);
      udp.read(payload, len);
      udp.flush();
  
      DEBUG_PORT.print("UDP packet rx. len = ");
      DEBUG_PORT.print(len);
      DEBUG_PORT.print(", remoteIP = ");
      DEBUG_PORT.print(ServerIP);
      DEBUG_PORT.print(", remotePort = ");
      DEBUG_PORT.print(ServerPort);
      DEBUG_PORT.print(", payload = '");
      DEBUG_PORT.print(String(payload));
      DEBUG_PORT.print("'");
      DEBUG_PORT.println();

      if((ServerPort == NETWORK_UDP_PORT) && (String(payload) == String(NETWORK_PASSWORD))){
        client_connected = client.connect(ServerIP, NETWORK_TCP_PORT);
      }
    }
  }
  udp.stop();
}

void loop() {
  if(button0){
//    client.close(true);
//    client.stop();

    client.write("test message!");

    button0 = false;
  }
}
