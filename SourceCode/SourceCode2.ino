#include "DHT.h"

#define DHTPIN D2 // Digital pin connected to the DHT sensor
#include <ESP8266WiFi.h> // to connect with the internet
#include<ThingSpeak.h> // to speak with cloud (IoT platform)
#define DHTTYPE DHT11

const char* ssid     = "Prem";
const char* password = "kyakyakya";

WiFiServer server(80);


unsigned long channel_id = 1691750;
const char *writeAPI = "5GE84CMS3VNLE54T";
const char *readAPI = "FK6C5RRN8RV1ESNW";



String header;

String output5State = "off";
String output4State = "off";

const int output5 = 5;
const int output4 = 4;

unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;


DHT dht(DHTPIN, DHTTYPE);



void setup() {
  Serial.begin(115200);

  Serial.println(F("DHTxx test!"));
  WiFi.begin(ssid,password);

  
  dht.begin();
 

  // Connect to Wi-Fi network with SSID and password
  //Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWifi is successfully connected");
  Serial.println("Enter this IP address in your browser to see weather data:");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients
  
  ThingSpeak.begin(client); 
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  
 
  

  
  Serial.print(F("Humidity: "));
  Serial.println(h);
  Serial.print(F(" Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  
  if (client) {                               
    String currentLine = "";                
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { 
      currentTime = millis();         
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                    
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            //defining two buttons used as fileds one for temperature and one for humidity
            if (header.indexOf("GET /Temperature_Humidity/on") >= 0) 
            {  
              output5State = "on";
            } 
            else if (header.indexOf("GET /Temperature_Humidity/off") >= 0) 
            {
              output5State = "off";
            } 
            else if (header.indexOf("GET /Temperature_Humidity/on") >= 0) {
              output4State = "on";
            } else if (header.indexOf("GET /Temperature_Humidity/off") >= 0) 
            {
              output4State = "off";
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #043E2D; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #043E2D;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Weather Monitoring System</h1>");
            client.println("</br>");
            String temp = String(t);
            client.println("<h3>Temperature </h3>");
             client.println("<p><a href=\"/temperature_humidity/on\"><button class=\"button\">"+temp +"</button></a></p>");
             
            String hum = String(h);
            client.println("<h3>Humidity </h3>");
            
              client.println("<p><a href=\"/temperature_humidity/on\"><button class=\"button\">"+ hum +"</button></a></p>");
            client.println("</br>");
            client.println("</br>");
            client.println("<h3>CS5453 IoT Project 2022</h3>");
            client.println("</body></html>");
            
            client.println();
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;    
        }
      }
    }
    header = "";
    client.stop();
  }
   ThingSpeak.writeField(channel_id, 2, t, writeAPI);
  ThingSpeak.writeField(channel_id, 4, h, writeAPI);
  delay(500); 
  
}
