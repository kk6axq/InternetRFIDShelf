
/*
 * Based on code from: https://randomnerdtutorials.com/esp8266-web-server-spiffs-nodemcu/
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

#include <SoftwareSerial.h>

// Replace with your network credentials
const char* ssid = "ProjectNetwork";
const char* password = "password";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

int inventory = 0;
int min_inventory = 0;

SoftwareSerial ss(D5, D6); //RX, TX
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  ss.begin(9600);

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    ss.println("Connecting to WiFi...");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  ss.println(WiFi.localIP());

  // Serve API endpoint
  server.on("/inventory", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(inventory));
  });

  server.on("/minimum", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(min_inventory));
  });
  
  // Serve root page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });

  // Start server
  server.begin();
}
 
void loop(){
  if(Serial.available()>0){
    inventory = Serial.parseInt(); 
    Serial.print("Got ");
    Serial.println(inventory);  
  }

  if(ss.available()>0){
    inventory = ss.parseInt();
    ss.read();
    min_inventory = ss.parseInt(); 
    Serial.print("Got ");
    Serial.println(inventory);  
  }
}
