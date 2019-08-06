// Libraries for DNS, WiFi, and Config AP
#include <WiFi.h>
#include <ESPmDNS.h>

// Libraries for web server & database
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>

// Default STA settings
const char* ssid = "Turbocharged";
const char* pswd = "wholefoods";
File dataFile;
// Web server settings
AsyncWebServer server(80);
const char* host = "radio";

String getMIME(String filename) {
  if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".mp3")) {
    return "audio/mp3";
  }
  return "text/plain";
}

// On not found - checks SD card if file exists
void handleError(AsyncWebServerRequest *request) {
  dataFile = SD.open(request->url());
  if (dataFile) {
    request->send(getMIME(request->url()), dataFile.size(), 
      [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
      return dataFile.read(buffer, maxLen); });
    return;
  } 
  
  request->send(404, "text/plain", "Page not found.");
}

void setup() {
  Serial.begin(115200);

  // Connect WiFi
  WiFi.mode(WIFI_STA);
  if (!WiFi.begin(ssid, pswd)) {
    Serial.println("Error connecting to WiFi!");
    return;
  }

  // Begin MDNS
  if (!MDNS.begin(host)) {
    Serial.println("Error setting up MDNS responder!!");
    return;
  }
  
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An error has occurred while mounting SPIFFS!!!");
    return;
  }

  // Initialize SD card
  if(!SD.begin(SS)) {
    Serial.println("An error has occured mounting the SD card!!!!");
    Serial.println("The server will operate in a limited capacity.");
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
  server.on("/radio", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/radio.html","text/html");
  });

  // Routes to load site content
  server.on("/css/site.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/site.css", "text/css");
  });
  server.on("/js/site.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/site.js", "text/js");
  });

  // Routes to load metadata
  server.on("/stats.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/stats.txt", "text/plain");
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

  // Backup music
  server.on("/media/rick.mp3", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/media/rick.mp3", "audio/mp3");
  });
  
  server.begin();
}

void loop() { }
