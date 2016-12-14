/*
//  GPL 3.0 License

//  Copyright (c) 2016 John Whittington www.jbrengineering.co.uk

//  This program is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.

//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.

//     You should have received a copy of the GNU General Public License
//     along with this program.  If not, see <http:www.gnu.org/licenses/>.
*/

#ifndef NIXIEPIPE_H
#define NIXIEPIPE_H

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

#include <FastLED.h>

// Led config
#define LED_TYPE            WS2812B
#define COLOR_ORDER         GRB
#define PIXEL_OFFSET        10 // pixel index offset per module

// Touch button arduino pin numbers
#define PIPE_TB0            2
#define PIPE_TB1            3

// Conversion from pipe number to LED index
#define NUM2IDX(x)          (PIXEL_OFFSET - 1) - x

// Index for SI prefix pipe
typedef enum {
  Pico,
  Nano,
  Micro,
  Milli,
  Kila,
  Mega,
  Giga,
  Tera,
  Neg,
  Pos
} Prefix;

// Index for SI units pipe
typedef enum {
  Volts,
  Amps,
  Watts,
  Grams,
  Hertz,
  Celsius,
  Newtons,
  Meters,
  Seconds,
  Ohm
} Unit;

// Index for weather pipe
typedef enum {
  Sun,
  Rain,
  Cloud,
  SunCloud,
  Snow,
  Wind,
  Storm,
  Fog,
  Percent,
  Pascal
} Weather;

class NixiePipe {
  public: 

    NixiePipe(uint8_t n = 1, uint8_t units = 0);
    ~NixiePipe();

	template< uint8_t DATA_PIN = 6 > void begin(void) { FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(pixels, numLEDs); }

    void
      show(void), // FastLED show, must be run after each write
      passSerial(HardwareSerial &serial), // Pass serial object for debug output
      write(void), // Write settings to pipes
      writeFade(uint8_t step), // Write settings, fading previous by step decrement (must be run repeatly)
      setPipeNumber(uint8_t n, uint8_t num), // Set pipe number
      setNumber(uint32_t num), // Set pipe array number
      writePipeNumber(uint8_t n, uint8_t num), // Set and write pipe number (clear previous)
      writePipeNumber(uint8_t n, uint8_t num, CRGB c), // Set and write pipe number with colour (clear previous)
      setPipeColour(uint8_t n, CRGB c), // Set pipe colour
      setPipeColour(CRGB c), // Set pipe array colour
      writeNumber(uint32_t num, CRGB C), // Write new number with new colour (clear previous)
      writeNumber(uint32_t num), // Write new number (clear previous)
      writeSolid(CRGB c), // Write new colour
      writeRainbow(uint8_t gHue), // Write rainbow (gHue should be incremented each call)
      writePipeFill(uint8_t n, CRGB c), // Write single pipe with new colour
      setBrightness(uint8_t brightness), // Set array brightness
      clearPipe(uint8_t n), // Clear single pipe (set black)
      clear(void), // Clear all pipes
      setNumberUnits(uint8_t unit_pipes); // Set number of Nixie Pipe units
    CRGB
      *getPixels(void), // Return pointer to LEDs
      *getPipePixels(uint8_t n); // Return pointer to pipe LEDs
    CRGBSet
      getPipe(uint8_t n); // Return pointer to pipe LED array
    NixiePipe
      &shift(int8_t n), // Shift pipe numbers n (+/-) times
      &operator++(), // Increment pipe array number
      &operator--(), // Decrement pipe array number
      operator++(int),
      operator--(int);
      // operator+(int rhs);
      // operator-(int rhs);
    uint32_t 
      getNumber(void), // Return curret pipe array number
      getMax(void); // Get maximum display number
    operator int() {return modNum;}

  private:
    const uint8_t
      numPipes; // Number of pipes (includes units)
    uint8_t
      numUnits, // Number of unit pipes
      numLEDs, // Number of LEDs
      *pipeNum, // Array to hold single pipe numbers
      brightness; // Array brightness
    uint32_t
      modNum, // Array number
      maxNum; // Maximum display number
    CRGB 
      *pixels, // The FastLEDs
      *pipeColour; // Array to hold single pipe colour
    HardwareSerial *pserial; // Pointer to passed serial object
};

#endif // NIXIEPIPE_H
