#include <Adafruit_NeoPixel.h>
#include "Definitions.h"

#define PIN 6
#define LED_COUNT 113
#define DELAY 10

Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);

void clearLEDs();
void Wave(int, uint32_t, uint32_t);
String mode;
int pos = 0;

void setup()
{
	Serial.begin(115200);
	leds.begin();
	clearLEDs();
	leds.setBrightness(99);
	leds.show();
	
	Wave(0, RED, BLACK);
	Wave(1, GREEN, BLACK);
	Wave(0, BLUE, WHITE);
}

void loop()
{
  if(mode == "rainbow"){
    rainbow(pos);
    pos++;
    delay(DELAY * 5);
  }
  
	if (Serial.available() > 0) {
		auto incomingByte = Serial.read();
		Serial.println(incomingByte, DEC);
    
		switch(incomingByte)
		{
			case 48:
			  Wave(1, RED, BLACK);
			  mode = "off";
				break;
			case 49:
			  Wave(0, GREEN, WHITE);
			  mode = "on";
				break;
			case 50:
  			mode = "rainbow";
				break;
			case 16: // Init the Serial for PC
			  Serial.print("hello");
				break;
			default:
			  Serial.println("Invalid profile");
				break;
		}
	}
}

void Wave(int direction, uint32_t color, uint32_t colorBehind)
{
  if(direction == 0)
  {
    for (auto i = 0; i <= 36; i++)
  	{
  		if (i < 3)
  		{
  			leds.setPixelColor(i, color);
  			leds.show();
  		}
  		else
  		{
  			leds.setPixelColor(i, color);
  			leds.setPixelColor(i - 3, colorBehind);
  			leds.show();
  		}
  
  		if (i == 36)
  		{
  			leds.setPixelColor(i - 2, colorBehind);
  			leds.show();
  			delay(50);
  			leds.setPixelColor(i - 1, colorBehind);
  			leds.show();
  			delay(50);
  			leds.setPixelColor(i, colorBehind);
  			leds.show();
  		}
  
  		delay(DELAY);
  	}
  } else {
    for (auto i = 36; i >= 0; i--)
  	{
  		if (i > 33)
  		{
  			leds.setPixelColor(i, color);
  			leds.show();
  		}
  		else
  		{
  			leds.setPixelColor(i, color);
  			leds.setPixelColor(i + 3, colorBehind);
  			leds.show();
  		}
  
  		if (i == 0)
  		{
  			leds.setPixelColor(i + 2, colorBehind);
  			leds.show();
  			delay(DELAY);
  			leds.setPixelColor(i + 1, colorBehind);
  			leds.show();
  			delay(DELAY);
  			leds.setPixelColor(i, colorBehind);
  			leds.show();
  		}
  
  		delay(DELAY);
  	}
  }
}

void rainbow(byte startPosition) 
{
  int rainbowScale = 192 / 37;
  
  for (int i=0; i<37; i++)
  {
    leds.setPixelColor(i, rainbowOrder((rainbowScale * (i + startPosition)) % 192));
  }
  leds.show();
}

uint32_t rainbowOrder(byte position) 
{
  if (position < 31)
  {
    return leds.Color(0xFF, position * 8, 0);
  }
  else if (position < 63)
  {
    position -= 31;
    return leds.Color(0xFF - position * 8, 0xFF, 0);
  }
  else if (position < 95)
  {
    position -= 63;
    return leds.Color(0, 0xFF, position * 8);
  }
  else if (position < 127)
  {
    position -= 95;
    return leds.Color(0, 0xFF - position * 8, 0xFF);
  }
  else if (position < 159)
  {
    position -= 127;
    return leds.Color(position * 8, 0, 0xFF);
  }
  else
  {
    position -= 159;
    return leds.Color(0xFF, 0x00, 0xFF - position * 8);
  }
}

void clearLEDs()
{
	for (auto i = 0; i < LED_COUNT; i++)
	{
		leds.setPixelColor(i, 0);
	}
	leds.show();
}
