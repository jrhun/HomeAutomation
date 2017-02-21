#include <ESP8266WebServer.h>
//#include "LedFunction.h"

const uint16_t PixelCount = 60;
const uint8_t maxBrightness = 128;

const CRGB colorCorrection = CRGB(255,176,240); //typical 5050 surface mount
const CRGB colorTemperature = CRGB(201,226,255); // overcast sky

class LedStates;
class LedFunction
{
  public:
    LedStates *state;
    LedFunction()
    {
    }

    virtual void render() = 0;
};

class RainbowFunction: public LedFunction
{
  public:
    uint8_t mHue;
    uint8_t delta = 7;

    RainbowFunction(uint8_t hue = 0)
    {
      mHue = hue;
    }
    virtual void render()
    {
      // called once every 1000/framerate msec
      mHue++;
      fill_rainbow(state->leds, state->count, mHue, delta);
      state->dirty = true;
    }
};

class LedStates
{
	public:
	CRGB leds[PixelCount];
	int count = 0;
	uint8_t brightness = 96;    //acts like a master fader
  bool lightsOn = true;
	bool dirty = false;
  uint8_t gHue = 120;     // used for various effects
  LedFunction *function = 0;
  
	NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> &pixels;
	LedStates(NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> &ledPixels)
	:pixels(ledPixels)
	{
		count = PixelCount;
    setAllRgb(60,60,60);
	}

//  void setFunction(LedFunction *newFunction)
//  {
//    if(function)
//      delete function;
//    function = newFunction;
//    if(!function)
//      return;
//    function->state = this;
//  }
	
	CRGB adjustColor(const CRGB &led)
	{
		 CRGB adj(0,0,0);
		
		 if (brightness > 0)
		 {
			  for(uint8_t i = 0; i < 3; i++) {
				  uint8_t cc = colorCorrection.raw[i];
				  uint8_t ct = colorTemperature.raw[i];
				  uint32_t work = (((uint32_t)cc)+1) * (((uint32_t)ct)+1) * brightness;
					  work /= 0x10000L;
					  adj.raw[i] = work & 0xFF;	
			  }
		 }
		 return adj;
	}

  void setLights(bool value)
  {
    lightsOn = value;
    dirty = true;
  }
  
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
		// scale 0-100 from homekit to 0-255
    if (scale < 0 or scale > 100)
    {
      Serial.print("Invalid brightness value, must be between 0-100");
      return; 
    }
    scale = (float) scale / 100.0 * 255.0;
		brightness = (int) scale;
    dirty = true;
	}

 int getBrightness()
 {
    // get average brightness of whole strip. Should return between 0-100 for homebridge
//    int avgLuma = 0;
//    for (uint8_t i = 0; i < PixelCount; i++)
//    {
//      CRGB led(leds[i]);
//      led %= brightness;    // correct for master brightness
//      avgLuma += led.getLuma();
//    }
//    avgLuma /= PixelCount;
//    return (int) (avgLuma / 255.0 * 100.0);         // should be between 0-100
      // Just return brightness scale value
    return (int) ((float) brightness / 255.0 * 100.0);
 }

 CRGB getAvgColor()
 {
  //gets average color of whole strip
    int avgR, avgG, avgB;
    avgR = avgG = avgB = 0;
    for (uint8_t i = 0; i < PixelCount; i++)
    {
      CRGB led(leds[i]);
      led %= brightness;    // correct led color for brightness
      avgR = avgR + led.r;
      avgG = avgG + led.g;
      avgB = avgB + led.b;
    }
    avgR /= PixelCount;
    avgG /= PixelCount;
    avgB /= PixelCount;
//    Serial.print("Average is: (RGB)\t");
//    Serial.print(avgR);
//    Serial.print(avgG);
//    Serial.println(avgB);
    return CRGB(avgR,avgG,avgB);
 }
	
	void commit()
	{
		if (!dirty)
			return;
    uint8_t b = brightness; 
    if (!lightsOn)
    {
      b = 0;
    } 
    for (int i = 0; i < PixelCount; i++)
    {
      CRGB led = leds[i];
      led %= b;  // %= scales as a percentage, 255 being dark, 0 being bright
      pixels.SetPixelColor(i, RgbColor(led.r,led.g,led.b) );
    }
		pixels.Show();
		dirty = false;
	}
 
  virtual void render()
  {
    if(function)
      function->render();
  }
};
