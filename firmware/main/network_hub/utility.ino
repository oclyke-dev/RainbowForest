/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

void printHex8( Stream* stream, uint32_t val ){
  if((uint32_t)val < 0x10000000){ stream->print(0); }
  if((uint32_t)val < 0x1000000){ stream->print(0); }
  if((uint32_t)val < 0x100000){ stream->print(0); }
  if((uint32_t)val < 0x10000){ stream->print(0); }
  if((uint32_t)val < 0x1000){ stream->print(0); }
  if((uint32_t)val < 0x100){ stream->print(0); }
  if((uint32_t)val < 0x10){ stream->print(0); }
  stream->print(val, HEX);
}
