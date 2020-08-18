/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "color_detection.h"
#include <math.h>
#include <limits>

double rgbElemByUI16(uint16_t val){
  return ((double)val / 65535.0);
}

// https://stackoverflow.com/a/6930407/8662931
hsv_f_t rgbToHsv(const rgb_f_t* rgb){
  hsv_f_t hsv;
  hsv.h = 0.0f;
  hsv.s = 0.0f;
  hsv.v = 0.0f;

  double epsilon = 1.0e-4;

  if(!rgb){ return hsv; }

  double min;
  double max;
  double delta;

  double r = rgb->r;
  double g = rgb->g;
  double b = rgb->b;

  min = (r < g) ? r : g;
  min = (min < b) ? min : b;

  max = (r > g) ? r : g;
  max = (max > b) ? max : b;

  hsv.v = max;
  delta = max - min;

  if(delta < epsilon){
    hsv.h = std::numeric_limits<double>::quiet_NaN();
    hsv.s = 0.0f;
    return hsv;
  }

  hsv.s = (delta / max);

  // calculate hue based on which element is max
  if(abs(r - max) < epsilon){
    hsv.h = 0.0f + (g - b)/delta;
  }
  if(abs(g - max) < epsilon){
    hsv.h = 2.0f + (b - r)/delta;
  }
  if(abs(b - max) < epsilon){
    hsv.h = 4.0f + (r - g)/delta;
  }

  hsv.h *= 60;
  if(hsv.h < 0.0f){
    hsv.h += 360.0f;
  }

  return hsv;
}

rgb_f_t hsvToRgb(const hsv_f_t* hsv){
  rgb_f_t rgb;
  rgb.r = 0.0f;
  rgb.g = 0.0f;
  rgb.b = 0.0f;

  if(!hsv){return rgb; }

  double epsilon = 1.0e-4;

  double h = hsv->h;
  double s = hsv->s;
  double v = hsv->v;

  if(abs(s) < epsilon){
    rgb.r = v;
    rgb.g = v;
    rgb.b = v;
    return rgb;
  }

  double hh = fmod(h, 360.0f)/60.0f;
  uint8_t sextant = (uint8_t)hh;
  double ff = hh - sextant;
  double p = v*(1.0f - s);
  double q = v*(1.0f - (s*ff));
  double t = v*(1.0f - (s*(1.0f - ff)));

  switch(sextant){
    case 0:
      rgb.r = v;
      rgb.g = t;
      rgb.b = p;
      break;
    case 1:
      rgb.r = q;
      rgb.g = v;
      rgb.b = p;
      break;
    case 2:
      rgb.r = p;
      rgb.g = v;
      rgb.b = t;
      break;
    case 3:
      rgb.r = p;
      rgb.g = q;
      rgb.b = v;
      break;
    case 4:
      rgb.r = t;
      rgb.g = p;
      rgb.b = v;
      break;
    case 5:
    default:
      rgb.r = v;
      rgb.g = p;
      rgb.b = q;
      break;
  }
  return rgb;
}

double rgbToMagnitude(const rgb_f_t* rgb){
  if(!rgb){ return 0.0; }

  double r = rgb->r;
  double g = rgb->g;
  double b = rgb->b;

  return (sqrt(r*r + g*g + b*b) / sqrt(3.0));
}

bool colorIndexByRGB(const rgb_f_t* rgb, size_t* idx, const color_definition_t* colors, size_t num_colors){
  if(!rgb){ return COLOR_DETECT_ERR; }
  if(!colors){ return COLOR_DETECT_ERR; }
  if(!idx){ return COLOR_DETECT_ERR; }

  // Serial.println(num_colors);

  // Serial.print("rgb: [");
  // Serial.print(rgb->r);
  // Serial.print(", ");
  // Serial.print(rgb->g);
  // Serial.print(", ");
  // Serial.print(rgb->b);
  // Serial.print("]");
  // Serial.println();

  hsv_f_t hsv = rgbToHsv(rgb);

  // Serial.print("hsv: [");
  // Serial.print(hsv.h);
  // Serial.print(", ");
  // Serial.print(hsv.s * (3.0f / 2.0f));
  // Serial.print(", ");
  // Serial.print(hsv.v * (3.0f / 4.0f));
  // Serial.print("]");
  // Serial.println();

  double min_dh = 720.0f;
  double mag = rgbToMagnitude(rgb);
  size_t possible_color_count = 0;

  *(idx) = 0;
  for(size_t color = 0; color < num_colors; color++){
    color_definition_t def = *(colors + color);

    if(mag < def.min_mag){ continue; }
    if(mag > def.max_mag){ continue; }
    if(hsv.h > def.max_hue){ continue; }
    if(hsv.h < def.min_hue){ continue; }

    double dh = abs(hsv.h - def.hue);
    if(dh > 180.0f){
      dh = 360.0f - dh;
    }

    if(dh < min_dh){
      min_dh = dh;
      *(idx) = color;
      possible_color_count++;
    }
  }
  
  if(possible_color_count == 0){
    return COLOR_DETECT_ERR;
  }

  return COLOR_DETECT_OK;
}
