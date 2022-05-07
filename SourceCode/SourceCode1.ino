#include "DHT.h"

#define DHTPIN D2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11


// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 3 (on the right) of the sensor to GROUND (if your sensor has 3 pins)
// Connect pin 4 (on the right) of the sensor to GROUND and leave the pin 3 EMPTY (if your sensor has 4 pins)
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.

#include <ESP8266WiFi.h> // to connect with the internet
#include<ThingSpeak.h> // to speak with cloud (IoT platform)
//#include <ESP8266WebServer.h>

const char *ssid = "CS21MTECH11008";
const char* password = "1234567890";

WiFiClient client;
//ESP8266WebServer server;

unsigned long channel_id = 1690146;
const char *writeAPI = "U90MHK8C5SAVCGGM";
const char *readAPI = "Z517JTKCRVI5EASF";



DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600); //baud rate
  Serial.println(F("DHTxx test!"));
  WiFi.begin(ssid,password);

  ThingSpeak.begin(client); 
//  WiFi.softAP(ssid,password); // to make node mcu device as hotspot
  dht.begin();

  // check wether the device is connected to wifi or not
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("*");
  }
  Serial.println("Connected to AP");
  Serial.println("");
  Serial.println("IP Address : ");
  Serial.println(WiFi.localIP());
  Serial.print("Exiting Setup block..."); 
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  /* Printing Data on the Serial Monitor*/
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print("°C ");
  Serial.println();
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%");
  Serial.println();
  

  /* writing Data to the ThingSpeak */
  ThingSpeak.writeField(channel_id, 1, t, writeAPI);
  ThingSpeak.writeField(channel_id, 2, h, writeAPI);
  
  //delay(500); 

/*
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  */

  
  Serial.println();
  Serial.println("Reading Data from the ThingSpeak cloud");
  
  float temp = ThingSpeak.readFloatField(channel_id, 1, readAPI);
  Serial.print("Temparature : ");
  Serial.print(temp);
  Serial.println(" *C");

  float hum = ThingSpeak.readFloatField(channel_id, 2, readAPI);
  Serial.print("Humidity : ");
  Serial.print(hum);
  Serial.println("%");
  Serial.println("---------------------------------------------------------");
    
}
