// Libraries for DNS, WiFi, and Config AP
#include <WiFi.h>
#include <ESPmDNS.h>

// Libraries for web server & database
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <SD.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Default STA settings
const char* ssid = "SBG6700AC-77379";
const char* pswd = "2fa66a5417";

// Web server settings
AsyncWebServer server(80);
const char* host = "radio";

// Gets called on not found
void handleError(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Page not found.");
}

void setup() {
  Serial.begin(115200);

  // Connect WiFi
  WiFi.mode(WIFI_STA);
  if (!WiFi.begin(ssid, pswd)) {
    Serial.println("Error connecting to WiFi.");
    return;
  }

  // Begin MDNS
  if (!MDNS.begin(host)) {
    Serial.println("Error setting up MDNS responder!");
    return;
  }
  
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }
  
  // On 404 error
  server.onNotFound(handleError);
  
  // Pages
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html","text/html");
  });
  server.on("/about", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/about.html","text/html");
  });

  // Routes to load site content
  server.on("/css/site.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/site.css", "text/css");
  });
  server.on("/js/site.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/site.js", "text/js");
  });

  // Routes to load media content
  server.on("/media/banner.jpg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/media/banner.jpg", "image/jpeg");
  });
  server.on("/media/rl.jpg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/media/rl.jpg", "image/jpeg");
  });
  server.on("/media/rlinvert.jpg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/media/rlinvert.jpg", "image/jpeg");
  });
  server.on("/media/rlinvertsmall.jpg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/media/rlinvertsmall.jpg", "image/jpeg");
  });
  
  server.begin();
}

void loop() { }
