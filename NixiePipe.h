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

class NixiePipe {
  public: 

    NixiePipe(uint8_t n = 1, uint8_t units = 0);
    ~NixiePipe();

	template< uint8_t DATA_PIN = 6 > void begin(void) { FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(pixels, numLEDs); }

    void
      show(void), // FastLED show
      passSerial(HardwareSerial &serial),
      write(void),
      writeFade(uint8_t step), // fade to black, write lit LEDs call repeat
      setPipeNumber(uint8_t n, uint8_t num), // set number but don't clear previous
      setNumber(uint32_t num), // set number to array
      writePipeNumber(uint8_t n, uint8_t num), // set and clear previous (set black)
      writePipeNumber(uint8_t n, uint8_t num, CRGB c), // set and clear with colour
      setPipeColour(uint8_t n, CRGB c), // set pipe colour
      setPipeColour(CRGB c), // set pipe colour
      writeNumber(uint32_t num, CRGB C),
      writeNumber(uint32_t num),
      writeSolid(CRGB c), // set pipe colour
      writeRainbow(uint8_t gHue), // set pipe rainbow
      writePipeFill(uint8_t n, CRGB c),
      setBrightness(uint8_t brightness),
      clearPipe(uint8_t n),
      clear();
    CRGB
      *getPixels(void),
      *getPipePixels(uint8_t n);
    CRGBSet
      getPipe(uint8_t n);
    NixiePipe
      &shift(int8_t n),
      &operator++(),
      &operator--(),
      operator++(int),
      operator--(int);
      // operator+(int rhs);
      // operator-(int rhs);
    uint32_t 
      getNumber(void),
      getMax(void);
    operator int() {return modNum;}

  private:
    const uint8_t
      numPipes,
      numUnits;
    uint8_t
      numLEDs,
      *pipeNum,
      brightness;
    uint32_t
      modNum,
      maxNum;
    CRGB 
      *pixels,
      *pipeColour;
    HardwareSerial *pserial;
};

#endif // NIXIEPIPE_H
