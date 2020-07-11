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

void handleClientDisconnect(void* args, AsyncClient* client){
  DEBUG_PORT.print("Client 0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.println(" disconnected!");
  while(!client->freeable()){};
  client->free();
}

void handleClientAck(void* args, AsyncClient* client, size_t len, uint32_t time){
  DEBUG_PORT.print("Client 0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.print(" ACKed! len = ");
  DEBUG_PORT.print(len);
  DEBUG_PORT.print(", time = ");
  DEBUG_PORT.print(time);
  DEBUG_PORT.println();
}

void handleClientError(void* args, AsyncClient* client, int8_t error){
  DEBUG_PORT.print("Client 0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.print(" error! error = ");
  DEBUG_PORT.print(error);
  DEBUG_PORT.println();
}

void handleClientPacket(void* args, AsyncClient* client, struct pbuf *pb){
  DEBUG_PORT.print("0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.print(" received a packet. pb* = 0x");
  DEBUG_PORT.print((uint32_t)pb, HEX);
  DEBUG_PORT.print(", len = ");
  DEBUG_PORT.print(pb->len);
  DEBUG_PORT.print(", tot_len = ");
  DEBUG_PORT.print(pb->tot_len);
  DEBUG_PORT.println();
}

void handleClientTimeout(void* args, AsyncClient* client, uint32_t time){
  DEBUG_PORT.print("Client 0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.print(" timeout! time = ");
  DEBUG_PORT.print(time);
  DEBUG_PORT.println();
}

void handleClientConnected(void* args, AsyncClient* client){
  DEBUG_PORT.print("Client 0x");
  DEBUG_PORT.print((uint32_t)client, HEX);
  DEBUG_PORT.println(" connected!");
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
      client.close(true);
      client.stop();
      ESP.restart();  // restart to reconnect
      break;
    default: 
      DEBUG_PORT.print("Unhandled WiFi Event: ");
      DEBUG_PORT.println(event);
      break;
  }
}

void IRAM_ATTR button0ISR() {
  button0 = true;
}

void connectToServer( void ){
  WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);
  
  while(!wifi_connected){
    static uint32_t connect_wifi = 0;
    if(millis() >= connect_wifi){
      DEBUG_PORT.print("Waiting to connect to SSID: ");
      DEBUG_PORT.println(NETWORK_SSID);
      connect_wifi = millis() + 2000;
    }
  }

  while(!client_connected){
    while(!udp.parsePacket()){
      static uint32_t debug_print_packet = 0;
      if(millis() >= debug_print_packet){
        DEBUG_PORT.println("Waiting to for UDP packet.");
        debug_print_packet = millis() + 500;
      }
    }
    
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
  udp.stop();
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);

  pinMode(0, INPUT_PULLUP);
  attachInterrupt(0, button0ISR, RISING);

  DEBUG_PORT.print("Column Controller Base Client = 0x");
  DEBUG_PORT.print((uint32_t)&client, HEX);
  DEBUG_PORT.println();
  client.onConnect(handleClientConnected, NULL);      //connected
  client.onDisconnect(handleClientDisconnect, NULL);  //disconnected
  client.onAck(handleClientAck, NULL);                //ack received
  client.onError(handleClientError, NULL);            //unsuccessful connect or error
  client.onPacket(handleClientPacket, NULL);          //data received
  client.onTimeout(handleClientTimeout, NULL);        //ack timeout
  
  WiFi.onEvent(WiFiEvent);
  connectToServer();
}

void loop() {
  if(button0){
    client.write("test message!");
    button0 = false;
  }
}
