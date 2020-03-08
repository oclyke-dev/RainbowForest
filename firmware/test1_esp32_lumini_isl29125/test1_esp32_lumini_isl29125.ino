
#include <math.h>
#include <Wire.h>
#include <FastLED.h>
#include "SparkFunISL29125.h"

#define DISPLAY_TITLES 0    // 1 to print field names to serial
#define DISPLAY_RGB 0           // 1 to print r g b values to serial
#define DISPLAY_HUE 0           // 1 to print computed h value to serial
#define DISPLAY_COLOR 1         // 1 to print nearest color to serial

// Define a type to hold detectable colors
typedef struct _color_t {
  float hue;
  const char* name;
}color_t;

// Here's the list of colors that the system can name on its own (you may add your own based on hue angle [0.0, 360] )
color_t colors[] = {
  {0.0, "Red"},
  {60.0, "Yellow"},
  {120.0, "Green"},
  {180.0, "Cyan"},
  {240.0, "Blue"},
  {300.0, "Magenta"},
};


// Color sensor
SFE_ISL29125 RGB_sensor;
unsigned int red = 0;
unsigned int green = 0;
unsigned int blue = 0;

// The LuMini rings need two data pins connected
#define DATA_PIN 16
#define CLOCK_PIN 17
#define NUM_LEDS 20 //1 Inch
CRGB ring[NUM_LEDS];

// Manual detection trigger
#define TRIGGER_PIN 0
volatile bool isr_fired = false;
volatile uint32_t debounce_time = 0;


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  // Initialize the ISL29125 with simple configuration so it starts sampling
  if (RGB_sensor.init())
  {
    Serial.println("Sensor Initialization Successful\n\r");
  }
  
  LEDS.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(ring, NUM_LEDS);
  LEDS.setBrightness(128);

  // set up the trigger pin
  pinMode(TRIGGER_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), startReading, RISING);

  Serial.print("Press button 0 to begin sampling colors!");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(isr_fired){
    printResults();
    isr_fired = false;
  }
  
}



void completeReading( void * parameter )
{
  // sensor continuously runs ADC at ~ 10 hz so to be sure wait 0.2 seconds before reading
  delay(200);

  delay(300); // delay to combat voltage sag from turning on all the leds...
              // i've experimentally determined that while there is no LED brightness that completely 
              // eliminates noise in detected color there is a minimum total delay between turning on
              // the leds and taking a sample that gets darn close. Its approx 500 ms total (including
              // time dedicated to letting the sensor read)

              // the final product may as well turn on all the leds, wait half a second, and then sample
              // all of the color sensors rapidly. 

  const uint8_t num_samples = 1;

  red = 0;
  blue = 0;
  green = 0;
  
  for( uint8_t indi = 0; indi < num_samples; indi++ ){
      red += (RGB_sensor.readRed() / num_samples);
      green += (RGB_sensor.readGreen() / num_samples);
      blue += (RGB_sensor.readBlue() / num_samples);
  }


  // Turn the ring to off
  for (int i = 0; i < NUM_LEDS; i++) {
    ring[i] = CRGB(0, 0, 0);
  }
  FastLED.show();

  isr_fired = true;

  delay(200);     // simulate a user pressing the button 200 ms later
  startReading(); // auto-start a new reading for conveneience
  
  vTaskDelete( NULL );
}

void startReading( void ){

  if(millis() <= debounce_time){ 
    return; 
  }

  debounce_time = millis() + 50;

//  // Turn the ring to white
//  for (int i = 0; i < NUM_LEDS; i++) {
  for (int i = 0; i < 1; i++) { // turn on only one led to be closer to the final design
    ring[i] = CRGB(200, 200, 255);  // adjust the balance of white here... blue seems to need some help (higher Vf than other leds..)
  }
  FastLED.show();

  // Start a new task to read the sensor (since it needs to wait for a sample)
  xTaskCreate(  completeReading,          /* Task function. */
                "completeReading",        /* String with name of task. */
                1000,                    /* Stack size in bytes. */
                NULL,                     /* Parameter passed as input of the task */
                1,                        /* Priority of the task. */
                NULL                      /* Task handle. */
             );
}

float max3( float Rp, float Gp, float Bp, uint8_t* index ){
  // ghetto way to find maximum of three (not tested well or even well-thought-through)...
  float Cmax = 0.0;
   if(Rp >= Gp){
      if(Rp >= Bp){
        Cmax = Rp;
        *index = 0;
      }
    }
    if(Gp >= Bp){
      if(Gp >= Rp){
        Cmax = Gp;
        *index = 1;
      }
    }
    if(Bp >= Gp){
      if(Bp >= Rp){
        Cmax = Bp;
        *index = 2;
      }
    }
    return Cmax;
}

float min3( float Rp, float Gp, float Bp, uint8_t* index ){
  // ghetto way to find maximum of three (not tested well or even well-thought-through)...
  float Cmin = 0.0;
   if(Rp <= Gp){
      if(Rp <= Bp){
        Cmin = Rp;
        *index = 0;
      }
    }
    if(Gp <= Bp){
      if(Gp <= Rp){
        Cmin = Gp;
        *index = 1;
      }
    }
    if(Bp <= Gp){
      if(Bp <= Rp){
        Cmin = Bp;
        *index = 2;
      }
    }
    return Cmin;
}

void printResults( void ){
    if(isr_fired){
    float Rp = (float)red/255;
    float Gp = (float)green/255;
    float Bp = (float)blue/255;

    uint8_t max_ind = 0;
    uint8_t min_ind = 0;
    float Cmax = max3(Rp, Gp, Bp, &max_ind);
    float Cmin = min3(Rp, Gp, Bp, &min_ind);
    float delta = Cmax - Cmin;
    
    float hue = 0.0;
    if(Cmax == 0.0){
      hue = 0.0;
    }else{
      switch(max_ind){
        case 0: hue = 60 * (fmod(((Gp-Bp)/delta), 6.0)); break;
        case 1: hue = 60 * (((Bp-Rp)/delta) + 2); break;
        case 2: hue = 60 * (((Rp-Gp)/delta) + 4); break;
        default: break;
      }
    }

    // search list of colors for the closest one (todo: when overall levels are low just say nothing is there)
    const char* identified_color = NULL;
    float prev_diff = 360.0; // start with a high (impossibly so) diff
    float diff = 0.0;
    uint8_t num_colors = sizeof(colors)/sizeof(color_t);
    for(uint8_t indi = 0; indi < num_colors; indi++){
      
      diff = abs(hue - colors[indi].hue);
      if ( diff >= 180.0 ){
        diff = 360.0 - diff;
      }
      if( diff < prev_diff ){
        prev_diff = diff;
        identified_color = colors[indi].name;
      }
    }

#if DISPLAY_RGB
#if DISPLAY_TITLES
    Serial.print("B: "); 
#endif
    Serial.print(blue);
    Serial.print(" ");

#if DISPLAY_TITLES
    Serial.print("R: "); 
#endif
    Serial.print(red);
    Serial.print(" ");

#if DISPLAY_TITLES
    Serial.print("G: "); 
#endif
    Serial.print(green);
    Serial.print(" ");
#endif

#if DISPLAY_HUE
#if DISPLAY_TITLES
    Serial.print("Hue: ");
#endif
    Serial.print(hue);
    Serial.print(" ");
#endif

#if DISPLAY_COLOR
#if DISPLAY_TITLES
    Serial.print("Color: ");
#endif
    Serial.print(identified_color);
    Serial.print(" ");
#endif

    Serial.println();
  }
}
