#define STAFF_ROWS 7
#define STAFF_COLS 16
#define STAFF_VALS 5

typedef struct _packet_t {
  uint8_t s = 0;
  uint8_t c = 0;
  uint8_t r = 0;
  uint8_t v = 0;
  uint8_t e = 0;
  uint8_t d = 0;
} packet_t;

packet_t packet;

void init_packet( void ){
  packet.s = 0x20;
  packet.e = 0x21;
}

void random_packet( void ){
  uint8_t sum = 0;

  packet.c = random(0, STAFF_COLS);
  packet.r = random(0, STAFF_ROWS);
  packet.v = random(0, STAFF_VALS);

  sum += packet.s;
  sum += packet.c;
  sum += packet.r;
  sum += packet.v;
  sum += packet.e;

  packet.d = sum;
}

void send_packet( void ){
  uint8_t* buff = (uint8_t*)(&packet);
  for( uint8_t index = 0; index < (sizeof(packet_t)/sizeof(uint8_t)); index++ ){
    Serial1.write(buff[index]);
    
    Serial.print("0x");
    if(buff[index] < 0x10){
      Serial.print("0");
    }
    Serial.print(buff[index], HEX);
    Serial.println(" ");
  }
}

void setup() {
  init_packet();

  Serial.begin(115200);
  Serial1.begin(115200);
}

void loop() {
  random_packet();
  send_packet();
  delay(2000);
  Serial.println("\n\n");
}
