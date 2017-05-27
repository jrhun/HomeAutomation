#homeAutomation.ino

#include "server.h"

void setup(){
	Serial.begin(115200);
	WiFi.begin(ssid, password);
	
}



#server.h

#include <ESP8266WebServer.h>

const char* ssid = "The House Of Benjamin";
const char* password = "bellasatonthemat";

ESP8266WebServer server(80);

void setupWifi(){
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
}


void setupServer(){
	server.on("/", handleRoot);
	server.onNotFound(handleNotFound);
	
	//Status
	server.on("/status", []() {
		server.send
	} );
}
void handleRoot(){
	
}

void handleNotFound(){
	
}