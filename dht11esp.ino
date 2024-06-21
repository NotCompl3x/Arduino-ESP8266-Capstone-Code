#define BLYNK_TEMPLATE_ID "TMPL4JM1T1oO"
#define BLYNK_TEMPLATE_NAME "Hybee"
#define BLYNK_AUTH_TOKEN "ZpcVD11-ZYllAy6aCyuu8LWKlTdmZghD"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
 

#include <DHT.h>

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "TUP-Hybrid Beehive";
char pass[] = "Hyb33_TUP";

#define DHTPIN 2          // Mention the digital pin where you connected 
#define DHTTYPE DHT11     // DHT 11  
DHT dht(DHTPIN, DHTTYPE);

//MPU6050
#include<Wire.h>
 
const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
 
int minVal=265;
int maxVal=402;
 
double X;
double Y;
double Z;

BlynkTimer timer;



void setup(){
   Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  timer.setInterval(2500L, sendSensor);
}
void sendSensor(){
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
   Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  float AcX=Wire.read()<<8|Wire.read();
  float AcY=Wire.read()<<8|Wire.read();
  float AcZ=Wire.read()<<8|Wire.read();
  int xAng = map(AcX,minVal,maxVal,-90,90);
  int yAng = map(AcY,minVal,maxVal,-90,90);
  int zAng = map(AcZ,minVal,maxVal,-90,90);
  
  Serial.print("RawX= ");
  Serial.println(AcX);
  Serial.print("RawY= ");
  Serial.println(AcY);
  Serial.print("RawZ= ");
  Serial.println(AcZ);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, t);
  Blynk.virtualWrite(V3, AcX);
  Blynk.virtualWrite(V4, AcY);
  Blynk.virtualWrite(V5, AcZ);
  
    Serial.print("Temperature : ");
    Serial.print(t);
    Serial.print("    Humidity : ");
    Serial.println(h);
}

void loop(){
  Blynk.run();
  timer.run();
}
