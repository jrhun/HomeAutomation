#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <NeoPixelBus.h>
#include <FastLED.h>

#include "LedStates.h"

const char* ssid = "**********";
const char* password = "***********";

ESP8266WebServer server(80);

//use FastLEDS to do animations/setting colors/etc since it's much easier
//But use NeoPixelBus for output as it has a DMA method that doesn't disable interupts
extern const uint16_t PixelCount; 
CRGB leds[PixelCount];    
NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> strip(PixelCount);

LedStates currentLedStates(strip);

unsigned long lastMillis = 0; 
const unsigned long frameRate = 30; //fps

inline unsigned long int rgbToHex(uint8_t r, uint8_t g, uint8_t b)
{   
  //convert RGB to hex
  unsigned long int out;
  out = ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
  return out;
}

CHSV hexToHSV();

void handRoot()
{
  String message = "Server is up\n";
  message += "LEDS:\t\t";
  message += (currentLedStates.lightsOn ? "On":"Off");
  message += "\nBrightness:\t";
  message += currentLedStates.getBrightness();
  message += "\nColor:\t\tR:";
  CRGB col = currentLedStates.getAvgColor();
  message += col.r;
  message += "\tG:";
  message += col.g;
  message += "\tB:";
  message += col.b;
//  message += "\nColor(HEX):\t";
//  message += (String)rgbToHex(col.r, col.g, col.b);
	server.send(200, "text/plain", message);
}

int getArgValue(String name)
{
  for (uint8_t i = 0; i < server.args(); i++)
    if(server.argName(i) == name)
      return server.arg(i).toInt();
  return -1;
}

void handleNotFound()
{
	String message = "Error:\n";
	message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
	Serial.print(message);
}

void setup() 
{
  Serial.begin(115200);
	WiFi.begin(ssid, password);
	Serial.println("");

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED) 
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	
	server.on("/", handRoot);
	server.on("/rainbow", [](){
	  server.send(200);
    if (server.args() > 0) 
    {
      int hue = server.arg(0).toInt();
      currentLedStates.function = new RainbowFunction(hue % 255);
    } else
    {
      currentLedStates.function = new RainbowFunction();
    }
    
	} );
	server.onNotFound(handleNotFound);
  server.on("/v1/status", []() {
    server.send( 200, "text/plain", currentLedStates.lightsOn ? "1" : "0");
  } );
  server.on("/v1/on", []() {
    currentLedStates.setLights(true);
//    Serial.print("LED On");
    server.send( 200, "text/plain", "");
  } );
  server.on("/v1/off", []() {
    currentLedStates.setLights(false);
//    Serial.print("LED Off");
    server.send( 200, "text/plain", "");
  } );
  server.on("/v1/brightness", []() {
    server.send( 200, "text/plain", String(currentLedStates.getBrightness()));
  } );
  server.on("/v1/brightness/", []() {
    int brightness = server.arg(0).toInt();
    currentLedStates.setBrightness( brightness );
//    Serial.print("Brightnes set to:\n");
//    Serial.println(brightness);
    server.send( 200, "text/plain", "");
  } );
  server.on("/v1/set", []() {
    CRGB avg = currentLedStates.getAvgColor();
    unsigned long rgb = rgbToHex(avg.r,avg.g,avg.b);
    server.send( 200, "text/plain", String(rgb));
//    Serial.print("Color request: \nHex:");
//    Serial.println(rgb);
  } );
  server.on("/v1/set/", []() {
//    long int rgb = getArgValue("rgb");
    unsigned long int hexValue = strtol(server.arg(0).c_str(), NULL, 16);
    CRGB rgb;
    rgb.r = ((hexValue >> 16) & 0xFF);
    rgb.g = ((hexValue >> 8) & 0xFF);
    rgb.b = ((hexValue) & 0xFF);
//    rgb.r = ((hexValue >> 16) & 0xFF) / 255.0;  // Extract the RR byte
//    rgb.g = ((hexValue >> 8) & 0xFF) / 255.0;   // Extract the GG byte
//    rgb.b = ((hexValue) & 0xFF) / 255.0; 
//    Serial.print("RGB:\n");
//    Serial.print(rgb.r);
//    Serial.print(rgb.g);
//    Serial.println(rgb.b);
//    Serial.print("Arg:");
//    Serial.print(server.arg(0));
//    Serial.print(hexValue);
    currentLedStates.setAllRgb(rgb.r, rgb.g, rgb.b);
    server.send( 200, "text/plain", "");
  } );
//  server.on("/favicon.ico", []() {
//    server.send( 404);
//  } );
	
	server.begin();
	Serial.println("HTTP Server started");
	
	strip.Begin();
	strip.Show();
}

void loop() 
{
	// put your main code here, to run repeatedly:
	server.handleClient();
  unsigned long currentMillis = millis();
  if (currentMillis - lastMillis > (1000 / frameRate))
  {
//    currentLedStates.render();
    currentLedStates.commit();
  }
}
