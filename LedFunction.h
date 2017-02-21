#include <ESP8266WebServer.h>
//#include "LedStates.h"

//class LedStates
//{
//  CRGB leds[PixelCount];
//  int count;
//  uint8_t brightness;
//  bool dirty;
//}
//class LedFunction
//{
//  public:
//    LedStates *state;
//    LedFunction()
//    {
//    }
//    
//    virtual void render() = 0;
//};

//class RainbowFunction: public LedFunction
//{
//  public:
//    uint8_t mHue;
//    uint8_t delta = 7;
//
//    RainbowFunction(uint8_t hue = 0)
//    {
//      mHue = hue;
//    }
//    virtual void render()
//    {
//      // called once every 1000/framerate msec
//      mHue++;
//      fill_rainbow(state->leds, state->count, mHue, delta);
//      state->dirty = true;
//    }
//};

///
/// Combines nicely with the waveform functions (like sin8, etc)
/// to produce continuous hue gradients back and forth:
///
///          hue = map8( sin8( myValue), HUE_BLUE, HUE_RED);
