#include "src/components/configuration/configuration.h"

#define DEBUG_PORT Serial
#define DEBUG_BAUD (115200)

double h = 0.0f;
double dh = 1.0f;
hsv_f_t hsv;
rgb_f_t rgb;

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);

//  hsv.s = 2.0f / 3.0f;
//  hsv.v = 4.0f / 3.0f;

  hsv.s = 1.0f;
  hsv.v = 1.0f;
}

void loop() {
  hsv.h = h;
  rgb = hsvToRgb(&hsv);
  double mag = rgbToMagnitude(&rgb);
  hsv = rgbToHsv(&rgb);

//  DEBUG_PORT.print(rgb.r);
//  DEBUG_PORT.print(" ");
//  DEBUG_PORT.print(rgb.g);
//  DEBUG_PORT.print(" ");
//  DEBUG_PORT.print(rgb.b);
//  DEBUG_PORT.print(" ");
//  DEBUG_PORT.print(mag);
//  DEBUG_PORT.print(" ");
//  DEBUG_PORT.println();

//  DEBUG_PORT.print(hsv.h/360.0f);
//  DEBUG_PORT.print(" ");
//  DEBUG_PORT.print(hsv.s);
//  DEBUG_PORT.print(" ");
//  DEBUG_PORT.print(hsv.v);
//  DEBUG_PORT.print(" ");
//  DEBUG_PORT.print(mag);
//  DEBUG_PORT.print(" ");
//  DEBUG_PORT.println();

  size_t index;
  if(COLOR_DETECT_OK == detectedColor(&rgb, &index)){
    DEBUG_PORT.println("Color detected!");
    DEBUG_PORT.println(detectable_colors[index].name);
  }else{
    DEBUG_PORT.println("error detecting color");
  }

  delay(50);
  
  h += dh;
  if(h >= 360.0f){
    h = 0;
  }
}
