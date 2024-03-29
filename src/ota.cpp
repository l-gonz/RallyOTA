#if defined(USE_OTA)
#include <WiFi.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <ESP2SOTA.h>

#include "secrets.h"

WebServer server(80);
#endif

void beginOTA(void) {
#if defined(USE_OTA)
  WiFi.mode(WIFI_AP);  
  WiFi.softAP(SECRET_WIFI_SSID, SECRET_WIFI_PWD);
  delay(1000);
  IPAddress IP = IPAddress (10, 10, 10, 1);
  IPAddress NMask = IPAddress (255, 255, 255, 0);
  WiFi.softAPConfig(IP, IP, NMask);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  /* INITIALIZE ESP2SOTA LIBRARY */
  ESP2SOTA.begin(&server);
  server.begin();
#endif
}

void endOTA(void) {
#if defined(USE_OTA)
    server.close();
    WiFi.softAPdisconnect(true);
#endif
}

void loopOTA(void) {
#if defined(USE_OTA)
  server.handleClient();
#endif
}