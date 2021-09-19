#ifdef ARDUINO_ARCH_ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

#ifdef ENABLE_DEBUG
 #define DEBUG_ESP_PORT Serial
 #define NODEBUG_WEBSOCKETS
 #define NDEBUG
#endif 


#include <Arduino.h>
#include "SinricPro.h"
#include "SinricProTemperaturesensor.h"
#include "DHT.h"
#include "SinricProMotionsensor.h"
#include <Espalexa.h>


#define WIFI_SSID         "TP-Link_9ED7"    
#define WIFI_PASS         "076646637654"

#define APP_KEY           "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"      // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define BAUD_RATE         9600                // Change baudrate to your need (used for serial monitor)
#define EVENT_WAIT_TIME   60000               // send event every 60 seconds

#define RelayPin1 18
#define RelayPin2 19
#define RelayPin3 21
#define HeartBeat 2

#define TEMP_SENSOR_ID    "5dc1564130xxxxxxxxxxxxxx"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define DHTPIN    33
#define DHTTYPE DHT22

#define DEVICE_ID_MQ2         "5dc1564130xxxxxxxxxxxxxx"      // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define PIN_MQ2 35 //define pin sensor MQ-2

#define DEVICE_ID_MQ7         "5dc1564130xxxxxxxxxxxxxx"      // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define PIN_MQ7 34 //define pin sensor MQ-7



Espalexa espalexa;
DHT dht(DHTPIN, DHTTYPE);



bool deviceIsOn;                              // Temeprature sensor on/off state
float temperature;                            // actual temperature
float humidity;                               // actual humidity
float lastTemperature;                        // last known temperature (for compare)
float lastHumidity;                           // last known humidity (for compare)
unsigned long lastEvent = (-EVENT_WAIT_TIME); // last time event has been sent

bool myPowerStateMQ2 = true;                     // assume device is turned on
bool lastMotionStateMQ2 = false;
unsigned long lastChangeMQ2 = 0;

bool myPowerStateMQ7 = true;                     // assume device is turned on
bool lastMotionStateMQ7 = false;
unsigned long lastChangeMQ7 = 0;


void setup() {

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(HeartBeat, OUTPUT);
  pinMode(DHTPIN, INPUT);
  pinMode(PIN_MQ2, INPUT);
  pinMode(PIN_MQ7, INPUT);


  digitalWrite(RelayPin1, HIGH);
  digitalWrite(RelayPin2, HIGH);
  digitalWrite(RelayPin3, HIGH);

  
  Serial.begin(BAUD_RATE);
  
  setupWiFi();

  dht.begin();

  setupSinricPro();
  
  espalexa.addDevice("Rele 1", firstLightChanged, 255);
  espalexa.addDevice("Rele 2", secondLightChanged, 255);
  espalexa.addDevice("Rele 3", thirdLightChanged, 255);

  espalexa.begin(); 

}

void loop() {
  espalexa.loop();
  SinricPro.handle();
  handleTemperaturesensor();
  handleMotionsensorMQ2();
  handleMotionsensorMQ7();
}
