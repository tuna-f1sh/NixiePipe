# Arduino Library Installation

## Dependancies

* [FastLED library](https://github.com/FastLED/FastLED)

Download or clone this reposity folder into your Arduino libraries folder.
Restart or open the Arduino IDE and head to File->Examples->NixiePipe for
example projects.

# Packages for Serial Firmware (np-serial)

* (Python Package)[https://github.com/tuna-f1sh/py-nixiepipe]
* (Node.js)[https://github.com/tuna-f1sh/node-nixiepipe]

# General Methodology

The class uses the [FastLED library](https://github.com/FastLED/FastLED) to
manage the daisy chained WS2812B on each Nixie Pipe PCB. The LEDs are split
into 'pipes' using pointer offset addressing of 10 LEDs. Each pipe can then
index the correct LED index for the current number to be displayed.

Pipes are `set` (display number, colour changed) prior to manipulation of the
LED array using `write`. Reasoning behind this is to allow an array of pipes to be setup
prior to the FastLED array changing. This allows fading and rainbow effects on
the complete pipe array.

To optimise processor usage, the hardware level write to the WS2812Bs is not
done until `show` is called; you will not see any changes on the display prior
to this. As with the FastLED library, consideration must be made to the finite
time required to transmit to a number of pipes and the fact that interrupts
are disabled during this period. For this reason, when using Serial
communication, polling `show` (for fade and rainbow), will cause erratic
communication.

## Units

Nixie Pipe units (non-number display), must be placed at the start of an array
(RHS) and are treated as an offset from the display number. For example, four
Nixie Pipes with two Nixie Pipe units with have a maximum display number of
99 using `setNumber`. To set the unit, one must call a single pipe function
set as `setPipeNumber` using the unit _enum_ as the number. Eg:

`pipes.setPipeNumber(0,Unit::Volts);`

Current Nixie Pipe unit enums: 'Prefix' (SI prefix), 'Unit' (SI units),
'Weather' (weather symbols). See 'pipe-units' in the 'examples/' folder.

# Usage

See the examples folder for multiple usage cases. The basic process flow is as
follows:

```C
// Create pipes object holding FastLEDs and display buffers
NixiePipe pipes = NixiePipe(NUM_PIPES);

void setup() {
  // Begin LEDs
  pipes.begin();
  // Clear display
  pipes.clear();
  // Set brightness
  pipes.setBrightness(BRIGHTNESS); 
  // Show clear and brightness
  pipes.show();
}

void loop() {
	static uint8_t i = 0;
	// Set pipe array number
	pipes.setNumber(i++);
	// Show pipe array number
	pipes.show();
	delay(1000);
}
```
## Constructor and Begin

The constructor (`NixiePipe(uint8_t n = 1, uint8_t units = 0)`) can be
overloaded with number of pipes _and_ number of units. Number of pipes is
inclusive of number of units (eg n=4,units=2 would be 2 number pipes and 2
unit pipes).

Call begin with or without the template LED_PIN. Without passing the
LED_PIN, it will default to 6 - the pin used on the Nixie Pipe PCB.

# Class Public Functions

Call these functions with your `pipes` object, eg. `pipes.show()`.

## show(void)

Wrapper for _FastLED_ show. Must be called after function call to make visible
change on display.

## setBrightness(uint8_t brightness)

Set array brightness (0-255), as FastLED library.

## clearPipe(uint8_t n)

* param1: Pipe number to clear.

Clear single pipe (set black). Must call `show` after to show changes.


## clear(void)

Clear pipe array. Must call `show` after to show changes.

## write(void)

Write settings to pipes; sets current number LED on and clears all others.
Should be called after settings change and before `write`

## writeNumber(uint32_t num)

* param1: New array number to display, should be less than `getMax`.

`write` new array number (clear previous).

## writeSolid(CRGB c)

* param1: New colour for pipe array.

`write` new array colour.

## writeRainbow(uint8_t gHue)

* param1: Rainbow hue, should be changed between calls.

`write` array rainbow, must be called repeatedly and with hue changing value.

## writeFade(uint8_t step)

* param1: Step to decrement LEDs other than current number.

As `write` but fades all other LEDs but decrement 'step' each call. Must be
called repeatedly to create fading effect.

## setPipeNumber(uint8_t n, uint8_t num)

* param1: Pipe to set.
* param2: New number to display, should be < 10.

Set new number to single pipe.

## setNumber(uint8_t num)

* param1: New array number to display, should be less than `getMax`.

Sets array number by automatically decimating to individual pipes. Will apply
with unit offset if pipe units are present.

## writePipeNumber(uint8_t n, uint8_t num)

* param1: Pipe to set.
* param2: New number to display, should be < 10.

Set and `write` pipe number (clear previous).

## writePipeNumber(uint8_t n, uint8_t num, CRGB c)

* param1: Pipe to set.
* param2: New number to display, should be < 10.
* param3: New colour for pipe.

Set and `write` pipe number with colour (clear previous).

## setPipeColour(uint8_t n, CRGB c)

* param1: Pipe to set.
* param2: New colour for pipe.

Set pipe colour.

## passSerial(HardwareSerial &serial)

* param1: Serial object to use for debug output

Pass serial object to pipes for debug output.
