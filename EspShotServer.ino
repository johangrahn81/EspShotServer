#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <ESPmDNS.h>
#include <WiFiAP.h>
#include <WiFi.h>

const char *ssid = "EspShotServer";
const char *password = "password";

struct hit_type {
  float x;
  float y;
  float v;
};

byte noOfHits=0;
hit_type hits[100];

AsyncWebServer  server(80);

const int led = 13;

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  if (MDNS.begin("EspShotServer")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
    Serial.println("/");
  });
    server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.js", "application/javascript");
    Serial.println("/index.js");
  });
  server.on("/fabric.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/fabric.js", "application/javascript");
    Serial.println("/fabric.js");
  });
  server.on("/data.json", HTTP_GET, [](AsyncWebServerRequest *request){
    String jsondata;
    jsondata+="{\n\t\"hits\":[\n";
    if(noOfHits>0){
      for(int i=0;i<noOfHits;i++){
        if(i!=0){
          jsondata+=",\n";
        }
        jsondata+="\t\t{\"id\":"+String(i)+",\"x\":"+String(hits[i].x, 2)+",\"y\":"+String(hits[i].y, 2)+",\"v\":"+String(hits[i].v, 2)+"}";
      }
    }
    jsondata+="\n\t]\n}";
    request->send(200, "application/json", jsondata);
    Serial.println("/data.json");
    Serial.println(jsondata);
  });
  server.on("/img/1.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/img/1.svg", "image/svg+xml");
    Serial.println("/img/1.svg");
  });
  server.on("/img/2.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/img/2.svg", "image/svg+xml");
    Serial.println("/img/2.svg");
  });

    
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  if(Serial.available()){
    Serial.println("Adding hit....");
    hits[noOfHits].x=random(4000,6000)/10.0;
    hits[noOfHits].y=random(4000,6000)/10.0;
    hits[noOfHits].v=random(8300,8500)/10.0;
    noOfHits++;
    while(Serial.available()){
      Serial.read();
    }
  }
  delay(2);//allow the cpu to switch to other tasks
}
