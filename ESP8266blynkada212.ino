
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>


/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID   " "

// adafruit io
#define IO_USERNAME  " "
#define IO_KEY       " "

#define ARDUINOJSON_ENABLE_COMMENTS 1
#include <SoftwareSerial.h>
// ArduinoJson - Version: 5.13.5
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>


SoftwareSerial nodemcu(D6, D5);


#include "BlynkSimpleEsp8266.h"


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = " ";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = " ";
char pass[] = " ";

float h;
float t;
float temp;
float hum;
float m1;
float m2;
float m3;
float m4;
float AcY;
float Acy;
float AcZ;
float col;

BlynkTimer timer;


void setup()
{
  // Debug console
  Serial.begin(9600);
  nodemcu.begin(9600);
  while (!Serial) continue;
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(5000L, sendSensor);

  
}


void sendSensor(){
  
    Blynk.virtualWrite(V2, hum);
    Blynk.virtualWrite(V1, temp);
    Blynk.virtualWrite(V3, col);
    Blynk.virtualWrite(V8, AcY);
    Blynk.virtualWrite(V4, m1);
    Blynk.virtualWrite(V5, m2);
    Blynk.virtualWrite(V6, m3);
    Blynk.virtualWrite(V7, m4);
    Blynk.virtualWrite(V11, Acy);
    Blynk.virtualWrite(V12, AcZ);
    
    if(temp > 38)
    {
      Blynk.logEvent("temp_alert","The colony reached critical temperature");
    }
    if(col > 5) 
    {
      Blynk.logEvent("col_full","The honey container is full.");
    }
}

void loop()
{ 
  Blynk.run();
  timer.run();
  
  if (nodemcu.available() > 0)
  {
    Serial.setTimeout(5000);
    StaticJsonDocument<400> doc;
    DeserializationError error = deserializeJson(doc, nodemcu.readString());
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
    hum = doc["humidity"];
    temp = doc["temperature"];
    col = doc["Collection"];
    AcY = doc["y"];
    m1 = doc["f1"];
    m2 = doc["f2"];
    m3 = doc["f3"];
    m4 = doc["f4"];
    Acy = doc["x"];
    AcZ = doc["z"];
    Serial.println(hum);
    
    Serial.println(temp);
    
    Serial.println(col);
    
    Serial.println(AcY);
   
    Serial.println(m1);
    
    Serial.println(m2);
    
    Serial.println(m3);
    
    Serial.println(m4);
    
    Serial.println(Acy);
    
    Serial.println(AcZ);
  }
  
}
