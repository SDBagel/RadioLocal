// Libraries for DNS, WiFi, and Config AP
#include <WiFi.h>
#include <ESPmDNS.h>

// Libraries for web server & database
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>

// SD card helpers
bool sdInitialized = false;
File dataFile;

// Default STA settings
const char* ssid = "SBG6700AC-77379";
const char* pswd = "2fa66a5417";

// Web server settings
AsyncWebServer server(80);
const char* host = "radio";

// Song & Channel structs to store info
struct Song {
  int len;
  String author;
  String title;
  File file;
};

struct Channel {
  String pubName;
  int sCount;
  File dir;
  Song currentSong;
};

// Channel 1, 2, & 3
Channel a;
Channel b;
Channel c;

// Metadata
DynamicJsonDocument metaDoc(8192);

// Timer helper
unsigned long prevMs = 0;

// Updates current song info for new connections
String NowPlaying() {
  StaticJsonDocument<512> doc;
  
  doc["a"] = a.currentSong.title;
  doc["a_a"] = a.currentSong.author;
  doc["a_t"] = a.sCount;
  doc["a_u"] = a.currentSong.file.name();
  doc["b"] = b.currentSong.title;
  doc["b_a"] = b.currentSong.author;
  doc["b_t"] = b.sCount;
  doc["b_u"] = b.currentSong.file.name();
  doc["c"] = c.currentSong.title;
  doc["c_a"] = c.currentSong.author;
  doc["c_t"] = c.sCount;
  doc["c_u"] = c.currentSong.file.name();

  String output = "";
  serializeJson(doc, output);
  return output;
}

// Gets the next valid MP3 file in a dir
void GetNextSong(Channel& channel) {
  if (channel.currentSong.file)
    channel.currentSong.file.close();
  
  File query = channel.dir.openNextFile();
  int tries = 0;
  while(!(((String)query.name()).endsWith(".mp3") || 
        ((String)query.name()).endsWith(".ogg")) 
        && tries < 5) {
    query = channel.dir.openNextFile();
    if (!query) {
      channel.dir.rewindDirectory();
      query = channel.dir.openNextFile();
    }
    tries++;
  }

  const char* queryName = query.name();
  Serial.print("Set next song for ");
  Serial.print(channel.dir.name());
  Serial.print(" to be "); 
  Serial.println(queryName);
  channel.currentSong.file = query;

  channel.currentSong.len = metaDoc[queryName]["l"];
  String author = metaDoc[queryName]["a"];
  channel.currentSong.author = author;
  String title = metaDoc[queryName]["n"];
  channel.currentSong.title = title;
  Serial.print(channel.currentSong.author + " ");
  Serial.print(channel.currentSong.title + " ");
  Serial.println(channel.currentSong.len);
}

void InitializeChannel(Channel& channel, const char* dir, const char* pub) {
  // Get next song & set info
  channel.pubName = pub;
  channel.sCount = 0;
  channel.dir = SD.open(dir);
  GetNextSong(channel);
}

// Get MIME type for SD
String getMIME(String filename) {
  if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".mp3")) {
    return "audio/mp3";
  } else if (filename.endsWith(".ogg")) {
    return "audio/ogg";
  }
  
  return "text/plain";
}

// On not found - checks SD card if file exists
void handleError(AsyncWebServerRequest *request) {
  if (sdInitialized) {
    dataFile.close();
    dataFile = SD.open(request->url());
    if (dataFile) {
      request->send(getMIME(request->url()), dataFile.size(), 
        [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
        return dataFile.read(buffer, maxLen); });
      return;
    } 
  }
  
  request->send(404, "text/plain", "Page not found.");
}

String processor(const String& var) {
  if(var == "CHA") return a.pubName;
  if(var == "CHB") return b.pubName;
  if(var == "CHC") return c.pubName;
  if(var == "CHAS") return a.currentSong.title;
  if(var == "CHBS") return b.currentSong.title;
  if(var == "CHCS") return c.currentSong.title;
  return String();
}

void setup() {
  // Begin serial
  Serial.begin(115200);

  // Connect WiFi
  WiFi.mode(WIFI_STA);
  if (!WiFi.begin(ssid, pswd)) {
    Serial.println("Error connecting to WiFi!"); return; }

  // Begin MDNS
  if (!MDNS.begin(host)) {
    Serial.println("Error setting up MDNS!"); return; }
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)){
    Serial.println("Error mounting SPIFFS!"); return; }

  // Initialize SD card
  if (!SD.begin(SS)) {
    Serial.println("Error mounting the SD card!");
    Serial.println("The server will operate in a limited capacity.");
  }
  else {
    sdInitialized = true;
    File channels = SD.open("/channels.txt");
    if (!channels) Serial.println("Invalid SD config!");
  
    String json;
    while (channels.available()) {
      json += char(channels.read());
    }
    channels.close();
  
    StaticJsonDocument<512> doc;
    deserializeJson(doc, json);
  
    // Set meta information
    File meta = SD.open("/meta.txt");
    if (!meta) Serial.println("Invalid meta file!");
    String metaJson;
    while (meta.available()) {
      metaJson += char(meta.read());
    }
    meta.close();
    deserializeJson(metaDoc, metaJson);
  
    // Init channels
    InitializeChannel(a, doc["channelA"], doc["a"]);
    InitializeChannel(b, doc["channelB"], doc["b"]);
    InitializeChannel(c, doc["channelC"], doc["c"]);
  }
  
  // On 404 error
  server.onNotFound(handleError);
  
  // Pages
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/about", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/about.html", "text/html");
  });
  server.on("/radio", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/radio.html", String(), false, processor);
  });

  // Routes to load site content
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/favicon.ico", "image/x-icon");
  });
  server.on("/css/site.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/css/site.css", "text/css");
  });
  server.on("/js/site.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/js/site.js", "text/js");
  });

  // Routes to load metadata
  server.on("/playing.txt", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", NowPlaying());
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

void loop() { 
  unsigned long currentMs = millis();

  // Every second, check for song updates
  if (currentMs - prevMs >= 1000 && sdInitialized) {
    prevMs = currentMs;
    a.sCount++;
    b.sCount++;
    c.sCount++;

    // If current song is defined & count is over, get next song
    if (a.sCount >= a.currentSong.len && a.currentSong.len != 0) {
      GetNextSong(a); 
      a.sCount = 0;
    }
    if (b.sCount >= b.currentSong.len && b.currentSong.len != 0) {
      GetNextSong(b);
      b.sCount = 0;
    }
    if (c.sCount >= c.currentSong.len && c.currentSong.len != 0) {
      GetNextSong(c);
      c.sCount = 0;
    }
  }
}
