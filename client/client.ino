/**
   BasicHTTPSClient.ino

    Created on: 14.10.2018

*/

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include "betaone.h"
#include <HTTPClient.h>
#include <TFT_eSPI.h>


WiFiMulti WiFiMulti;

TFT_eSPI tft= TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("", "");

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");

  // setup screen
  tft.init();
  tft.setRotation(1);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_WHITE);
  tft.pushImage(240,0, 80, 64,betaone);

  sprite.createSprite(240,170);
 
  sprite.setTextColor(TFT_BLACK,TFT_WHITE);
  sprite.setTextDatum(4);
}

void loop() {
  WiFiClient *client = new WiFiClient;
  if(client) {

    // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(*client, "http://192.168.1.100")) {  // HTTP
      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);

          // print angle read to screen
          sprite.fillSprite(TFT_WHITE);
          sprite.setFreeFont(&Orbitron_Light_32);
          sprite.drawString("Angle: ",75,54);
          sprite.setFreeFont(&Orbitron_Light_32);
          sprite.drawString(payload + " degree",125,104);
          sprite.pushSprite(0,0);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP] Unable to connect\n");
    }

    // End extra scoping block

    delete client;
  } else {
    Serial.println("Unable to create client");
  }

  Serial.println();
  delay(300);
}
