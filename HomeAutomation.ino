#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <NeoPixelBus.h>
#include <FastLED.h>

#include "LedStates.h"

const char* ssid = "The House of Benjamin";
const char* password = "bellasatonthemat";

ESP8266WebServer server(80);

//use FastLEDS to do animations/setting colors/etc since it's much easier
//But use NeoPixelBus for output as it has a DMA method that doesn't disable interupts
extern const uint16_t PixelCount; 
CRGB leds[PixelCount];    
NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> strip(PixelCount);

LedStates currentLedStates(strip);

inline unsigned long createRGB(int r, int g, int b)
{   
  //convert RGB to hex
    return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

CHSV hexToHSV();

void handRoot()
{
	server.send(200, "text/plain", "Server is up");
}

int getArgValue(String name)
{
  for (uint8_t i = 0; i < server.args(); i++)
    if(server.argName(i) == name)
      return server.arg(i).toInt();
  return -1;
}

void rainbow()
{
	currentLedStates.rainbow();
//  currentLedStates.commit(); 
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
	server.on("/rainbow", rainbow);
	server.onNotFound(handleNotFound);
  server.on("/v1/status", []() {
    server.send( 200, "text/plain", currentLedStates.lightsOn ? "1" : "0");
  } );
  server.on("/v1/on", []() {
    currentLedStates.lightsOn = true;;
//    Serial.print("LED On");
    server.send( 200, "text/plain", "");
  } );
  server.on("/v1/off", []() {
    currentLedStates.lightsOn = false;;
//    Serial.print("LED Off");
    server.send( 200, "text/plain", "");
  } );
  server.on("/v1/brightness", []() {
    server.send( 200, "text/plain", String(currentLedStates.getBrightness()));
  } );
  server.on("/v1/brightness/", []() {
    int brightness = server.arg(0).toInt();
    currentLedStates.setBrightness( brightness );
    Serial.print("Brightnes set to:\n");
    Serial.println(brightness);
    server.send( 200, "text/plain", "");
  } );
  server.on("/v1/set", []() {
    CHSV avgColor = currentLedStates.getAvgColor();
    unsigned long hsv = createRGB(avgColor.hue,avgColor.sat,avgColor.val);
    server.send( 200, "text/plain", String(hsv));
    Serial.print("Color request: current:");
    Serial.print(hsv);
  } );
  server.on("/v1/set/", []() {
//    long int rgb = getArgValue("rgb");
    long int hexValue = strtol(server.arg(0).c_str(), NULL, 16);
    CRGB rgb;
    rgb.r = ((hexValue >> 16) & 0xFF);
    rgb.g = ((hexValue >> 8) & 0xFF);
    rgb.b = ((hexValue) & 0xFF);
//    rgb.r = ((hexValue >> 16) & 0xFF) / 255.0;  // Extract the RR byte
//    rgb.g = ((hexValue >> 8) & 0xFF) / 255.0;   // Extract the GG byte
//    rgb.b = ((hexValue) & 0xFF) / 255.0; 
    Serial.print("RGB:\n");
    Serial.print(rgb.r);
    Serial.print(rgb.g);
    Serial.println(rgb.b);
    Serial.print("Arg:");
    Serial.print(server.arg(0));
    Serial.print(hexValue);
    currentLedStates.setAllHsv(rgb.r, rgb.g, rgb.b);
    server.send( 200, "text/plain", "");
  } );
	
	server.begin();
	Serial.println("HTTP Server started");
	
	strip.Begin();
	strip.Show();
}

void loop() 
{
	// put your main code here, to run repeatedly:
	server.handleClient();
  currentLedStates.commit();
  delay(50);
//	currentLedStates.render();
}
