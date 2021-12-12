#include <WiFi.h>
#include <WiFiUdp.h>

#include "credentials.h"

#define PORT 5678
IPAddress server_address = IPAddress(10,0,0,92);

#define RX_BUF_SIZE 512
char udp_rx_buf[RX_BUF_SIZE];

WiFiUDP udp;

// temporary simulation of column notes
const size_t columns = 16;  // how many columns in the staff
const size_t notes = 7;     // how many notes there are in the column
const size_t values = 5;    // how many valid data values there are
uint8_t data[columns][notes] = {0};

void setup() {
  Serial.begin(115200);

  WiFi.onEvent(WiFiEvent);
  WiFi.begin(SSID, PASSWORD);

  udp.begin(PORT);
}

void loop() {
  udp.parsePacket();

  uint32_t now = millis();

  static uint32_t send_update = 0;
  if(send_update <= now){
    send_update = now + 1000;

    size_t col = random(columns);
    size_t note = random(notes);
    uint8_t val = random(values);
    
    const char* fmt = "{\"timestamp\": \"%d\", \"id\": {\"name\": \"column\", \"extra\": \"%d\"}, \"update\": {\"entries\": [{\"column\": %d, \"row\": %d, \"entry\": {\"note\": %d}}]}}";
    size_t required = 1 + snprintf(NULL, 0, fmt, now, col, col, note, val);
    char buf[required] = {'\0'};
    snprintf(buf, required, fmt, now, col, col, note, val);

    udp.beginPacket(server_address, PORT);
    udp.write((uint8_t*)buf, required);
    udp.endPacket();
    Serial.println("transmitting!");
  }

  size_t rx_bytes = udp.read(udp_rx_buf, RX_BUF_SIZE);
  if(rx_bytes){
    Serial.print("received:");
    Serial.println(udp_rx_buf);
  }
  
}

void WiFiEvent(WiFiEvent_t event){
  switch(event) {
  case SYSTEM_EVENT_STA_GOT_IP:
    Serial.print("WiFi got ip: ");
    Serial.println(WiFi.localIP());
    break;
    
  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("WiFi disconnected");
    WiFi.begin(SSID, PASSWORD);
    break;
    
  default:
    Serial.printf("[WiFi] event: %d\n", event);
    break;
  }
}
