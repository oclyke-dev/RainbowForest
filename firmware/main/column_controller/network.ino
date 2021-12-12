/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "Arduino.h"

IPAddress host_ip = IPAddress(0,0,0,0);

#ifdef PRODUCTION
  #define HOST host_ip
  #define PORT 5678
  #define MDNS_HOSTNAME "rainbow-forest-pi"
#else
  #define HOST DEV_HOST
  #define PORT DEV_PORT
  #define MDNS_HOSTNAME DEV_MDNS_HOSTNAME
#endif // PRODUCTION

void sendUDP(const uint8_t* msg, size_t len) {
  udp.beginPacket(HOST, PORT);
  udp.write(msg, len);
  udp.endPacket();
}

void sendUDP_str(const char* msg) {
  sendUDP((uint8_t*)msg, strlen(msg));
}

void init_network ( void ){
  WiFi.onEvent(wifi_event_handler);
#ifdef NETWORK_PASSWORD
  WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);
#else
  WiFi.begin(NETWORK_SSID);
#endif

  udp.begin(PORT);
}

void wifi_event_handler(WiFiEvent_t event){
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP: {
      DEBUG_PORT.print("WiFi got ip: ");
      DEBUG_PORT.print(WiFi.localIP());
      DEBUG_PORT.println();

      const char* host_fmt = "rf-col-%d";
      size_t required = 1 + snprintf(NULL, 0, host_fmt, column_number);
      char host[required] = {'\0'};
      snprintf(host, required, host_fmt, column_number);
      if (!MDNS.begin(host)) {
        Serial.println("Error setting up MDNS responder!");
      }

      host_ip = MDNS.queryHost(MDNS_HOSTNAME);
      DEBUG_PORT.print("mDNS got host ip: ");
      DEBUG_PORT.print(host_ip);
      DEBUG_PORT.println();

#ifdef PRODUCTION
      Serial.printf("PROD MODE - connecting to mDNS resolved host '%s' (%u.%u.%u.%u)\n", MDNS_HOSTNAME, (host_ip & 0xFF), ((host_ip>>8) & 0xFF), ((host_ip>>16) & 0xFF), ((host_ip>>24) & 0xFF));
#else
      Serial.printf("DEV MODE - connecting directly to DEV_HOST: %s\n", HOST);
#endif // PRODUCTION
    }
    break;
    
  case SYSTEM_EVENT_STA_DISCONNECTED:
    DEBUG_PRINTF(("WiFi disconnected\n"));
    clearColors();
// #ifdef NETWORK_PASSWORD
//     WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);
// #else
//     WiFi.begin(NETWORK_SSID);
// #endif
    ESP.restart();
    break;
    
  default:
    DEBUG_PRINTF(("[WiFi] event: %d\n", event));
    break;
  }
}
