
#include <WiFi.h>
#include <WebSocketsClient.h>   // WebSockets by Markus Sattler (2.x.x for ESP)

//#define SSID "daihinmin"
//#define PASSWORD "anseladams"
#define SSID "galvanize members"
#define PASSWORD "ourmembersrock!"

//#define PRODUCTION // comment out to use development settings

#ifdef PRODUCTION
  #define HOST "oclyke.dev"
  #define PORT 443
  #define USE_SSL                                       // <-- when in production mode we use SSL (the nginx ingress handles decryption before messages hit the api server)
#else
  #define HOST "10.9.12.89"                             // <-- set this to the ip address of the machine running the api server
  #define PORT 81
#endif // PRODUCTION

#define PROTOCOL "rf"

// api base url
const char* api = "/rainbow-forest/api/v1";

// private endpoint
const char* private_end = "/private";
const char* private_auth_header = "RF-Auth: find-your-muse";
WebSocketsClient private_ws;


// temporary simulation of column notes
const size_t columns = 16;  // how many columns in the staff
const size_t notes = 7;     // how many notes there are in the column
const size_t values = 5;    // how many valid data values there are
uint8_t data[columns][notes] = {0};

void setup() {
  Serial.begin(115200);

  WiFi.onEvent(WiFiEvent);
  WiFi.begin(SSID, PASSWORD);

  private_ws.setExtraHeaders(private_auth_header);
  private_ws.onEvent(private_ws_event_handler);
}

void loop() {
  private_ws.loop();

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
    
    private_ws.sendTXT(buf);
  }
  
}

void WiFiEvent(WiFiEvent_t event){
  switch(event) {
  case SYSTEM_EVENT_STA_GOT_IP: {
    Serial.print("WiFi got ip: ");
    Serial.println(WiFi.localIP());

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
    Serial.println("WiFi disconnected");
    WiFi.begin(SSID, PASSWORD);
    break;
    
  default:
    Serial.printf("[WiFi] event: %d\n", event);
    break;
  }
}

void private_ws_event_handler(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
      
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to url: %s\n",  payload);
      private_ws.sendTXT("{\"timestamp\": \"\", \"id\": {\"name\": \"column\", \"extra\": \"0\"}, \"request\": {\"columns\": [{\"column\": 0}]}}");
      break;
      
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      // send message to server
      // webSocket.sendTXT("message here");
      break;
      
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
      
    case WStype_ERROR:
      Serial.printf("[WSc] error: %s\n",  payload);
      break;
    
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      Serial.printf("[WSc] event: %d\n",  type);
      break;
  }
}
