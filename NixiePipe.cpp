// MIT License

// Copyright (c) 2016 John Whittington www.jbrengineering.co.uk

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "NixiePipe.h"

// Error if we can't find FastLED
#if FASTLED_VERSION < 3000000
  #error "NixiePipe requires FastLED 3.0 lib: https://github.com/FastLED/FastLED"
#endif

// Calculates int power (math int uses floats)
static inline long powint(int factor, unsigned int exponent)
{
  long product = 1;
  while (exponent--)
    product *= factor;
  return product;
}

// NixiePipe class constructor
NixiePipe::NixiePipe(uint8_t n, uint8_t units) : numPipes(n), numUnits(units), numLEDs(0), pipeNum(0), brightness(255), modNum(0), maxNum(0), pixels(0), pipeColour(0)
{
  // Fill number of LEDs var
  numLEDs = PIXEL_OFFSET * numPipes;
  // Fill maximum displayable number var
  maxNum = powint(10,(numPipes-numUnits)) - 1;

  // Init pixels
  pixels = new CRGB[numLEDs]();
  // Init pipe number buffer
  pipeNum = new uint8_t[numPipes]();
  // Init pipe colour buffer
  pipeColour = new CRGB[numPipes]();
  // Set the default colour
  this->setPipeColour(CRGB::White);
  
  // Create the FastLED object (now done with begin)
  /*FastLED.addLeds<LED_TYPE, 6, COLOR_ORDER>(pixels, numLEDs);*/
}

// Class destructor
NixiePipe::~NixiePipe() {
  if(pixels) delete[] pixels;
  if(pipeNum) delete[] pipeNum;
  if(pipeColour) delete[] pipeColour;
}

// Pass hardware Serial pointer to class
void NixiePipe::passSerial(HardwareSerial &serial) {
  pserial = &serial;
}

// Get current displayed number
uint32_t NixiePipe::getNumber(void) {
  return this->modNum;
}

// Get maximum displayable number
uint32_t NixiePipe::getMax(void) {
  return this->maxNum;
}

// Set brightness
void NixiePipe::setBrightness(uint8_t nvalue) {
  brightness = nvalue;
  FastLED.setBrightness(brightness);
}

// Set individual pipe module number
void NixiePipe::setPipeNumber(uint8_t n /*module*/, uint8_t num /*number*/) {
  int32_t newd = 0;

  // can number can be writen?
  if (num < PIXEL_OFFSET) {
    // update the displayed number buffer if it's a number pipe
    if (n >= this->numUnits) {
      newd = num - pipeNum[n];
      newd *= powint(10,(n - this->numUnits));
      this->modNum += newd; // update module reference number
    }
    
    // update number reference
    pipeNum[n] = num;
    
    // if (pserial != NULL) {
    //   pserial->print("setPipeNumber: ");
    //   pserial->println(modNum);
    //   pserial->print(n);
    //   pserial->print(",");
    //   pserial->print(num);
    //   pserial->print(" ");
    //   pserial->print("modNum: ");
    // }
  }
}

// Set display number (auto decimates digit to modules)
void NixiePipe::setNumber(uint32_t num) {
  uint8_t digit = 0;
  uint8_t i = this->numUnits;
  if (pserial != NULL) {
    pserial->print("setNumber: ");
    pserial->println(num);
  }

  if (num <= this->maxNum) {
    do {
      digit = num % 10;
      num /= 10;
      this->setPipeNumber(i++,digit);
    } while (i < this->numPipes);
  }

  // write leading zeros (automatically as int number becomes 0)
  // while (i < this->numPipes) {
  //   this->setPipeNumber(i++,digit);
  // }

}

// Set colour, number and write
void NixiePipe::writePipeNumber(uint8_t n, uint8_t num, CRGB c) {
  this->setPipeColour(n,c);

  this->setPipeNumber(n,num);
  this->write();
}

// Set number and write
void NixiePipe::writePipeNumber(uint8_t n, uint8_t num) {
  this->writePipeNumber(n,num,this->pipeColour[n]);
}

// Set pipe module colour
void NixiePipe::setPipeColour(uint8_t n, CRGB c) {
  pipeColour[n] = c;
}

// Set all pipes' colour
void NixiePipe::setPipeColour(CRGB c) {
  for (int i = 0; i < this->numPipes; i++)
    this->setPipeColour(i,c);
}

// Write display number with colour
void NixiePipe::writeNumber(uint32_t num, CRGB c) {
  this->setNumber(num);
  this->writeSolid(c);
}

// Write display number
void NixiePipe::writeNumber(uint32_t num) {
  this->setNumber(num);
  this->write();
}

// Output and display updates to pipes
void NixiePipe::show(void) {
  FastLED.show();
}

// Write new settings to pipes
void NixiePipe::write(void) {
  this->clear();
  
  for (int i = 0; i < this->numPipes; i++) {
    CRGB *ppipe = &pixels[i * PIXEL_OFFSET];
    ppipe[NUM2IDX(pipeNum[i])] = pipeColour[i];
  }
}

// Write new settings and solid colour to pipes
void NixiePipe::writeSolid(CRGB c) {
  this->setPipeColour(c);
  this->write();
}

// Write new settings and fade previous
void NixiePipe::writeFade(uint8_t step) {
  for (int i = 0;i < this->numPipes; i++) {
    CRGB *ppipe = &pixels[i * PIXEL_OFFSET];
    fadeToBlackBy(ppipe,PIXEL_OFFSET,step); // turn off all pixels in block
    ppipe[NUM2IDX(pipeNum[i])] = pipeColour[i];
    fadeLightBy(&ppipe[pipeNum[i]-1],1,step);
  }
}

// Write new settings with rainbow colour
void NixiePipe::writeRainbow(uint8_t gHue) {
  CHSV hsv;
  CRGB rgb;
  hsv.hue = gHue;
  hsv.val = 255;
  hsv.sat = 240;
  hsv2rgb_rainbow(hsv,rgb);

  for (int i=0; i < numPipes; i++) {
    CRGB *ppipe = &pixels[i * PIXEL_OFFSET];
    ppipe[NUM2IDX(pipeNum[i])] = hsv;
  }
}

// Fill pipe module with solid colour (used with Black to clear)
void NixiePipe::writePipeFill(uint8_t n, CRGB c) {
  CRGBSet module = this->getPipe(n);

  module.fill_solid(c);
}


// Fill pipe display with Black (off)
void NixiePipe::clear(void) {
  CRGBSet pipes(pixels,numLEDs);

  pipes.fill_solid(CRGB::Black);
}

// Fill single pipe module with Black (off)
void NixiePipe::clearPipe(uint8_t n) {
  CRGBSet module = this->getPipe(n);

  module.fill_solid(CRGB::Black);
}

// Return a pointer to the FastLED pixels
CRGB* NixiePipe::getPixels() {
  return pixels;
}

// Return a pointer to module specific FastLED pixels
CRGB* NixiePipe::getPipePixels(uint8_t n) {
  return &pixels[n * PIXEL_OFFSET];
}


// Return a module pixel array (see FastLED docs)
CRGBSet NixiePipe::getPipe(uint8_t n) {
  CRGB *ppipe = &pixels[n * PIXEL_OFFSET];
  CRGBSet module(ppipe,PIXEL_OFFSET);
  return module;
}

// (Pre) Increment overload adds 1 to current display
NixiePipe& NixiePipe::operator++() {
  uint32_t temp = this->modNum;
  if (temp < this->maxNum )
    ++temp;
  else
    temp = 0;
  this->setNumber(temp);
  return *this;
}

// (Post) Increment (does not work?)
NixiePipe NixiePipe::operator++(int) {
  NixiePipe tmp = *this;
  ++*this;
  return tmp;
}

// (Pre) Decrement overload subtracts 1 from current display
NixiePipe& NixiePipe::operator--() {
  uint32_t temp = this->modNum;
  if (temp > 0)
    --temp;
  else
    temp = this->maxNum;
  this->setNumber(temp);
  return *this;
}

// (Post) Decrement (does not work?)
NixiePipe NixiePipe::operator--(int) {
  NixiePipe tmp = *this;
  --*this;
  return tmp;
}

// Shift current display (+/-) n digits
NixiePipe& NixiePipe::shift(int8_t n) {
  int idx = 0;

  if (n >= 0) {
    for(int i = (this->numPipes - 1); i >= this->numUnits; i--) {
      idx = i - n;
      this->setPipeNumber(i,(idx >= this->numUnits && idx < this->numPipes) ? this->pipeNum[idx] : 0);
    }
  } else {
    for(int i = 0; i < this->numPipes; i++) {
      idx = i - n;
      this->setPipeNumber(i,(idx >= this->numUnits && idx < this->numPipes) ? this->pipeNum[idx] : 0);
    }
  }

  
  return *this;
}

// NixiePipe operator+(int rhs) {
//   this->setPipeNumber(this->modNum + rhs);

//   return this;
// }

// NixiePipe operator-(int rhs) {
//   this->setPipeNumber(this->modNum - rhs);

//   return this;
// }
