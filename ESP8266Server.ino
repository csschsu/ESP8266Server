#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 12 // Higher resolution

#ifndef STASSID
#define STASSID "TP-Link_CF06"
#define STAPSK  "68001841"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = 13;
const char *sensor = "ds18b20";
const int sensorid = 0; // only the first onewire device ( Need only one for this )
int deviceCount = 1;
float celcius = 0.0;

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);  
// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!\r\n");
  digitalWrite(led, 0);
}


void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  

  server.on("/", handleRoot);

  server.on("/metrics", []() {
  sensors.requestTemperatures(); 
  // Display temperature from each sensor
  for (int i = 0;  i < deviceCount;  i++)
  {
    celcius = sensors.getTempCByIndex(i);
    char temp[400];
    snprintf(temp, 400,
    "# HELP climate Climate data from sensor\n# TYPE climate gauge\nclimate{unit=\"C\",sensor=\"%s\"} %.2f\n",
    sensor, celcius);
    server.send(200, "text/html", temp);
  }
  });

  server.begin();
  Serial.println("HTTP server started");
  sensors.begin();  // Start up the onewirelibrary
  sensors.getAddress(tempDeviceAddress, sensorid);
  sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);

}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
