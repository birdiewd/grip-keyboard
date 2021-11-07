#include <Keyboard.h>

// ====

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* @file MultiKey.ino
|| @version 1.0
|| @author Mark Stanley
|| @contact mstanley@technologist.com
||
|| @description
|| | The latest version, 3.0, of the keypad library supports up to 10
|| | active keys all being pressed at the same time. This sketch is an
|| | example of how you can get multiple key presses from a keypad or
|| | keyboard.
|| #
*/

#include <Keypad.h>

const byte ROWS = 2; //four rows
const byte COLS = 2; //three columns
char keys[ROWS][COLS] = {
	{KEY_LEFT_SHIFT, KEY_LEFT_ALT},
	{KEY_LEFT_CTRL, KEY_LEFT_GUI}};
byte rowPins[ROWS] = {6, 7}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {9, 8}; //connect to the column pinouts of the kpd

Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

bool hasShift = true;
bool hasAlt = true;
bool hasControl = true;
bool hasOsGui = true;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET 4		// Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
	Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16
static const unsigned char PROGMEM logo_bmp[] = {
	0b00000000, 0b11000000,
	0b00000001, 0b11000000,
	0b00000001, 0b11000000,
	0b00000011, 0b11100000,
	0b11110011, 0b11100000,
	0b11111110, 0b11111000,
	0b01111110, 0b11111111,
	0b00110011, 0b10011111,
	0b00011111, 0b11111100,
	0b00001101, 0b01110000,
	0b00011011, 0b10100000,
	0b00111111, 0b11100000,
	0b00111111, 0b11110000,
	0b01111100, 0b11110000,
	0b01110000, 0b01110000,
	0b00000000, 0b00110000
};

void setup()
{
	Serial.begin(9600);

	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
	{
		Serial.println(F("SSD1306 allocation failed"));
		for (;;)
			; // Don't proceed, loop forever
	}

	testdrawbitmap();
	delay(1000);

	hasShift = false;
	hasAlt = false;
	hasControl = false;
	hasOsGui = false;

	testdrawbitmap();
}

void loop()
{
	// Fills kpd.key[ ] array with up-to 10 active keys.
	// Returns true if there are ANY active keys.
	if (kpd.getKeys())
	{
		for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
		{
			if (kpd.key[i].stateChanged) // Only find keys that have changed state.
			{
				unsigned char keyCheck = (unsigned char)kpd.key[i].kchar;

				switch (kpd.key[i].kstate)
				{ // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
				case PRESSED:
					Keyboard.press(kpd.key[i].kchar);
				case HOLD:
					switch (keyCheck)
					{
					case KEY_LEFT_SHIFT:
						hasShift = true;
						break;
					case KEY_LEFT_ALT:
						hasAlt = true;
						break;
					case KEY_LEFT_CTRL:
						hasControl = true;
						break;
					case KEY_LEFT_GUI:
						hasOsGui = true;
						break;

					default:
						break;
					}
					break;
				case RELEASED:
					Keyboard.release(kpd.key[i].kchar);
				case IDLE:
					switch (keyCheck)
					{
					case KEY_LEFT_SHIFT:
						hasShift = false;
						break;
					case KEY_LEFT_ALT:
						hasAlt = false;
						break;
					case KEY_LEFT_CTRL:
						hasControl = false;
						break;
					case KEY_LEFT_GUI:
						hasOsGui = false;
						break;

					default:
						break;
					}

					break;
				}
			}
		}
	}

	if (kpd.getState() == IDLE)
	{
	}

	testdrawbitmap();
} // End loop

void testdrawbitmap()
{
	display.clearDisplay();

	if (hasShift) {
		display.drawBitmap(
			5 + (LOGO_WIDTH * 0),
			(display.height() - LOGO_HEIGHT) / 2,
			logo_bmp,
			LOGO_WIDTH,
			LOGO_HEIGHT,
			1);
	}

	if (hasAlt)
	{
		display.drawBitmap(
			5 + (LOGO_WIDTH * 1),
			(display.height() - LOGO_HEIGHT) / 2,
			logo_bmp,
			LOGO_WIDTH,
			LOGO_HEIGHT,
			1);
	}

	if (hasControl)
	{
		display.drawBitmap(
			5 + (LOGO_WIDTH * 2),
			(display.height() - LOGO_HEIGHT) / 2,
			logo_bmp,
			LOGO_WIDTH,
			LOGO_HEIGHT,
			1);
	}

	if (hasOsGui)
	{
		display.drawBitmap(
			5 + (LOGO_WIDTH * 3),
			(display.height() - LOGO_HEIGHT) / 2,
			logo_bmp,
			LOGO_WIDTH,
			LOGO_HEIGHT,
			1);
	}

	display.display();
}
