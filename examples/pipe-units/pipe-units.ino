#include <Wire.h>
#include <TimeLib.h>
#include <FastLED.h>
#include <DS3232RTC.h>

#include <NixiePipe.h>

#define LED_PIN       6
#define NUM_PIPES     6
#define NUM_UNITS     2
#define BRIGHTNESS    255
#define FADE_DEC	    32
#define SEQUENCE_TIME 15      // Time between sequences

#define ADC_CHANNEL   0
#define SAMPLES       128

#define RAINBOW       false
#define MAIN_COLOUR   CRGB::White

NixiePipe pipes = NixiePipe(NUM_PIPES,NUM_UNITS);
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0;

static void clock_freq(void);
static void temperature(void);
static void voltage(void);

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { clock_freq, temperature, voltage };

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

static void nextPattern()
{
	// add one to the current pattern number, and wrap around at the end
	gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void setup() {
  pipes.begin<LED_PIN>();
  pipes.clear();
  pipes.setBrightness(BRIGHTNESS); 
  pipes.show();
}

void loop() {
  /* gPatterns[gCurrentPatternNumber]();*/
  /* temperature();*/
  voltage();

  pipes.writeFade(FADE_DEC);
  if (RAINBOW)
    pipes.writeRainbow(gHue);
	pipes.show();

	// do some periodic updates
	EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
	EVERY_N_SECONDS( SEQUENCE_TIME ) { nextPattern(); } // change patterns periodically
}

static void clock_freq(void) {
  pipes.setPipeColour(MAIN_COLOUR);
  pipes.setNumber(F_CPU/1000000);
  pipes.setPipeNumber(0,Unit::Hertz);
  pipes.setPipeNumber(1,Prefix::Mega);
}

static void temperature(void) {
  int temp = RTC.temperature() / 4;
  uint8_t sat;
  CRGB rgb;

  if (temp > 0)
    sat = map(temp, 0, 60, 64, 0);
  else
    sat = map(temp, 0, -20, 128, 160);

  CHSV hsv(sat, 255, 255);
  hsv2rgb_rainbow(hsv,rgb);

  pipes.setPipeColour(rgb);
  pipes.setNumber(abs(temp));
  pipes.setPipeNumber(0,Unit::Celsius);
  pipes.setPipeNumber(1,((temp >= 0) ? Prefix::Pos : Prefix::Neg));
}

static void voltage(void) {
  int32_t volts = analogRead(ADC_CHANNEL);
  static int LM[SAMPLES] = {0};      // LastMeasurements
  static int index = 0;
  static long sum = 0;
  static int count = 0;

  volts *= 5000;
  volts /= 1024;
  pipes.setPipeNumber(1,Prefix::Milli);

  if (NUM_PIPES - NUM_UNITS < 4) {
    volts /= 1000;
    pipes.setPipeNumber(1,((volts >= 0) ? Prefix::Pos : Prefix::Neg));
  }

  sum -= LM[index];
  LM[index] = volts;
  sum += LM[index];
  index++;
  index = index % SAMPLES;
  if (count < SAMPLES) count++;

  volts = sum / count;
  
  pipes.setPipeColour(MAIN_COLOUR);
  pipes.setNumber(abs(volts));
  pipes.setPipeNumber(0,Unit::Volts);
  /* pipes.setPipeNumber(1,((volts >= 0) ? Prefix::Pos : Prefix::Neg));*/
}

