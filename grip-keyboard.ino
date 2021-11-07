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

long idleTime = 0;

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

#define MOD_ICON_HEIGHT 16
#define MOD_ICON_WIDTH 16
// 'pixil-layer-alt', 16x16px
const unsigned char epd_bitmap_pixil_layer_alt[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40,
	0x03, 0xc0, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// 'pixil-layer-base', 16x16px
const unsigned char epd_bitmap_pixil_layer_base[] PROGMEM = {
	0x00, 0x00, 0x3f, 0xfc, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02,
	0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x3f, 0xfc, 0x00, 0x00};
// 'pixil-layer-control', 16x16px
const unsigned char epd_bitmap_pixil_layer_control[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x02, 0x40, 0x02, 0x00, 0x02, 0x00,
	0x02, 0x00, 0x02, 0x00, 0x02, 0x40, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// 'pixil-layer-highlight', 16x16px
const unsigned char epd_bitmap_pixil_layer_highlight[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x30, 0x0c, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04,
	0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x30, 0x0c, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00};
// 'pixil-layer-os', 16x16px
const unsigned char epd_bitmap_pixil_layer_os[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x02, 0xc0, 0x03, 0x40, 0x02, 0xc0,
	0x03, 0x40, 0x02, 0xc0, 0x03, 0x40, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// 'pixil-layer-shift', 16x16px
const unsigned char epd_bitmap_pixil_layer_shift[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x02, 0x40, 0x02, 0x00, 0x01, 0x00,
	0x00, 0x80, 0x00, 0x40, 0x02, 0x40, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 288)
const int epd_bitmap_allArray_LEN = 6;
const unsigned char *epd_bitmap_allArray[6] = {
	epd_bitmap_pixil_layer_alt,
	epd_bitmap_pixil_layer_base,
	epd_bitmap_pixil_layer_control,
	epd_bitmap_pixil_layer_highlight,
	epd_bitmap_pixil_layer_os,
	epd_bitmap_pixil_layer_shift};

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

	drawModKeys();
	delay(1000);

	hasShift = false;
	hasAlt = false;
	hasControl = false;
	hasOsGui = false;

	drawModKeys();
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
					idleTime = 0;
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


	if (idleTime < 2000) {
		drawModKeys();
		idleTime++;
	} else {
		display.clearDisplay();
		display.display();
	}

	delay(1);
} // End loop

void drawModKeys()
{
	display.clearDisplay();

	drawModKey(epd_bitmap_pixil_layer_shift, hasShift, 0, 0);
	drawModKey(epd_bitmap_pixil_layer_alt, hasAlt, 1, 0);
	drawModKey(epd_bitmap_pixil_layer_control, hasControl, 0, 1);
	drawModKey(epd_bitmap_pixil_layer_os, hasOsGui, 1, 1);

	display.display();
}

void drawModKey(const uint8_t *bitmap, bool hasModkey, int row, int column)
{
	display.drawBitmap(
		(MOD_ICON_HEIGHT * row),
		(MOD_ICON_WIDTH * column),
		epd_bitmap_pixil_layer_base,
		MOD_ICON_WIDTH,
		MOD_ICON_HEIGHT,
		1);

	if (hasModkey)
	{
		display.drawBitmap(
			(MOD_ICON_HEIGHT * row),
			(MOD_ICON_WIDTH * column),
			epd_bitmap_pixil_layer_highlight,
			MOD_ICON_WIDTH,
			MOD_ICON_HEIGHT,
			1);
	}

	display.drawBitmap(
		(MOD_ICON_HEIGHT * row),
		(MOD_ICON_WIDTH * column),
		bitmap,
		MOD_ICON_WIDTH,
		MOD_ICON_HEIGHT,
		1);
}
