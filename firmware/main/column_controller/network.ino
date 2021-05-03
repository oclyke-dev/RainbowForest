/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "Arduino.h"

IPAddress host_ip = IPAddress(0,0,0,0);

#ifdef PRODUCTION
  #define HOST host_ip
  #define PORT 81
  #define MDNS_HOSTNAME "rainbow-forest-pi"
#else
  #define HOST DEV_HOST
  #define PORT DEV_PORT
  #define MDNS_HOSTNAME DEV_MDNS_HOSTNAME
#endif // PRODUCTION

#define PROTOCOL "rf"

// api base url
const char* api = "/rainbow-forest/api/v1";

// private endpoint
const char* private_end = "/private";

void init_network ( void ){
  WiFi.onEvent(wifi_event_handler);
#ifdef NETWORK_PASSWORD
  WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);
#else
  WiFi.begin(NETWORK_SSID);
#endif

  private_ws.setExtraHeaders(PRIVATE_AUTH_HEADER);
  private_ws.onEvent(private_ws_event_handler);
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

      // construct the desired endpoint
      const size_t route_len = (strlen(api) + strlen(private_end)) + 1;
      char route[route_len] = {'\0'};
      strcat(route, api);
      strcat(route, private_end);

#ifdef PRODUCTION
      Serial.printf("PROD MODE - connecting to mDNS resolved host '%s' (%u.%u.%u.%u)\n", MDNS_HOSTNAME, (host_ip & 0xFF), ((host_ip>>8) & 0xFF), ((host_ip>>16) & 0xFF), ((host_ip>>24) & 0xFF));
#else
      Serial.printf("DEV MODE - connecting directly to DEV_HOST: %s\n", HOST);
#endif // PRODUCTION

      // connect to the endpoint
      private_ws.begin(HOST, PORT, route, PROTOCOL);
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

void private_ws_event_handler(WStype_t type, uint8_t * payload, size_t length) {
  static size_t disconnected_count = 0;
  switch(type) {
    case WStype_DISCONNECTED:
      DEBUG_PORT.printf("[WSc] Disconnected!\n");
      disconnected_count += 1;
      clearColors();
      if(disconnected_count > 10){
        ESP.restart();
      }
      break;
      
    case WStype_CONNECTED:
      DEBUG_PORT.printf("[WSc] Connected to url: %s\n",  payload);
      requestColumn();
      break;
      
    case WStype_TEXT:
      parseUpdate((const char*)payload);
      // send message to server
      // webSocket.sendTXT("message here");
      break;
      
    case WStype_BIN:
      DEBUG_PRINTF(("[WSc] get binary length: %u\n", length));
      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
      
    case WStype_ERROR:
      DEBUG_PRINTF(("[WSc] error: %s\n",  payload));
      break;
    
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      DEBUG_PRINTF(("[WSc] event: %d\n",  type));
      break;
  }
}
