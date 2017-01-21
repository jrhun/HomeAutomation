#include <ESP8266WebServer.h>
//#include "LedFunction.h"

const uint16_t PixelCount = 60;

const CRGB colorCorrection = CRGB(255,176,240); //typical 5050 surface mount
const CRGB colorTemperature = CRGB(201,226,255); // overcast sky

class LedStates
{
	public:
	CRGB leds[PixelCount];
	int count = 0;
	int brightness = 96;
  bool lightsOn = true;
	bool dirty = false;
	NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> &pixels;
	
	
	LedStates(NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> &ledPixels)
	:pixels(ledPixels)
	{
		count = PixelCount;
    setAllRgb(60,60,60);
	}
	
	inline RgbColor fl_to_neo(CRGB led)
	{
		//helper function to convert fastled to neopixel
		RgbColor colorOut(led.red, led.green, led.blue);
		return colorOut;
	}
	
	// static CRGB adjustColor(const CRGB &led)
	// {
		// CRGB adj(0,0,0);
		
		// if (brightness > 0)
		// {
			// for(uint8_t i = 0; i < 3; i++) {
				// uint8_t cc = colorCorrection.raw[i];
				// uint8_t ct = colorTemperature.raw[i];
				// uint32_t work = (((uint32_t)cc)+1) * (((uint32_t)ct)+1) * brightness;
					// work /= 0x10000L;
					// adj.raw[i] = work & 0xFF;	
			// }
		// }
		// return adj;
	// }
	
	void setRgb(int i, uint8_t r, uint8_t g, uint8_t b)
	{
		leds[i] = CRGB(r,g,b);
		dirty = true;
	}
  void setAllRgb(uint8_t r, uint8_t g, uint8_t b)
  {
    for (uint8_t i = 0; i < PixelCount; i++)
    {
      setRgb(i,r,g,b);
    }
  }
	
	void setHsv(int i, uint8_t h, uint8_t s, uint8_t v)
	{
		leds[i] = CHSV(h,s,v);
		dirty = true;
	}

  void setAllHsv(uint8_t h, uint8_t s, uint8_t v)
  {
    for (uint8_t i = 0; i < PixelCount; i++)
    {
      setHsv(i,h,s,v);
    }
  }
	
	void setBrightness(int scale)
	{
		// scale from 0-255
		brightness = scale;
    dirty = true;
	}

 int getBrightness()
 {
    // get average brightness of whole strip
    uint8_t avgLuma = 0;
    for (uint8_t i = 0; i < PixelCount; i++)
    {
      avgLuma += leds[i].getLuma();
    }
    avgLuma /= PixelCount;
    return avgLuma;
 }

 CHSV getAvgColor()
 {
  //gets average color of whole strip
    uint8_t avgR, avgG, avgB;
    avgR = avgG = avgB = 0;
    for (uint8_t i = 0; i < PixelCount; i++)
    {
      avgR += leds[i].red;
      avgG += leds[i].green;
      avgB += leds[i].blue;
    }
    avgR /= PixelCount;
    avgG /= PixelCount;
    avgB /= PixelCount;
    return CHSV(avgR,avgG,avgB);
 }
	
	void rainbow()
	{
		uint8_t gHue = 102;
		fill_rainbow(leds, count, gHue, 7);
		dirty = true;
	}
	
	void commit()
	{
		if (!dirty)
			return;
    if (!lightsOn)
    {
        pixels.ClearTo(RgbColor(0,0,0));
        pixels.Show();
        return;
    } else
    {
      for (int i = 0; i < PixelCount; i++)
      {
        CRGB cLed = leds[i];
//        uint8_t temp = constrain(255-brightness, 0, 255);
        cLed %= brightness;  // %= scales as a percentage, 255 being dark, 0 being bright
        pixels.SetPixelColor(i, fl_to_neo(cLed) );
      }
    }
		pixels.Show();
		dirty = false;
	}
};
