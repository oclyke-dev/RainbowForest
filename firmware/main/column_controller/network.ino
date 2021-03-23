/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#ifdef PRODUCTION
  #define HOST "oclyke.dev"
  #define PORT 443
  #define USE_SSL               // <-- when in production mode we use SSL (the nginx ingress handles decryption before messages hit the api server)
#else
  #define HOST DEV_HOST
  #define PORT DEV_PORT
#endif // PRODUCTION

#define PROTOCOL "rf"

// api base url
const char* api = "/rainbow-forest/api/v1";

// private endpoint
const char* private_end = "/private";

void init_network ( void ){
  WiFi.onEvent(wifi_event_handler);
  WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);

  private_ws.setExtraHeaders(PRIVATE_AUTH_HEADER);
  private_ws.onEvent(private_ws_event_handler);
}

void wifi_event_handler(WiFiEvent_t event){
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP: {
      DEBUG_PORT.print("WiFi got ip: ");
      DEBUG_PORT.print(WiFi.localIP());
      DEBUG_PORT.println();

      // construct the desired endpoint
      const size_t route_len = (strlen(api) + strlen(private_end)) + 1;
      char route[route_len] = {'\0'};
      strcat(route, api);
      strcat(route, private_end);

      // connect to the endpoint
#ifdef USE_SSL
      private_ws.beginSSL(HOST, PORT, route, "", PROTOCOL);
#else
      private_ws.begin(HOST, PORT, route, PROTOCOL);
#endif // USE_SSL
    }
    break;
    
  case SYSTEM_EVENT_STA_DISCONNECTED:
    DEBUG_PRINTF(("WiFi disconnected\n"));
    WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);
    break;
    
  default:
    DEBUG_PRINTF(("[WiFi] event: %d\n", event));
    break;
  }
}

void private_ws_event_handler(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      DEBUG_PRINTF(("[WSc] Disconnected!\n"));
      break;
      
    case WStype_CONNECTED:
      DEBUG_PRINTF(("[WSc] Connected to url: %s\n",  payload));
      requestColumn();
      break;
      
    case WStype_TEXT:
      DEBUG_PRINTF(("[WSc] get text: %s\n", payload));
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
