 #define ARDUINOJSON_ENABLE_COMMENTS 1
//Arduino to NodeMCU Lib
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//dht11

#include <DHT.h>

//HCSR04 
#include <EasyUltrasonic.h>
#define TRIGPIN 5 // Digital pin connected to the trig pin of the ultrasonic sensor
#define ECHOPIN 6 // Digital pin connected to the echo pin of the ultrasonic sensor
EasyUltrasonic ultrasonic;

//Initialise Arduino to NodeMCU (14=Rx & 15=Tx)
SoftwareSerial nodemcu(10, 11);

//MPU6050
#include<Wire.h>
 
const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
 
int minVal=265;
int maxVal=402;
 
double X;
double Y;
double Z;


//Initialisation of DHT11 Sensor
#define DHTPIN 3
DHT dht(DHTPIN, DHT11);


#include "HX711.h"
//RELAY1------------//52//50//48//46
int FAN1 =52 ;

//RELAY2
int POWCOM = 50;

//RELAY3
int COMP = 48; 

//RELAY4
int TEMPFAN = 46;

//RELAY7------------//53//51//49//47
int BAG1 = 49; 
//RELAY8
int BAG2 = 47; 

int i = 0;
int j = 0;
int l = 0;

const int pinData1 = 22;
const int pinData2 = 23;
const int pinData3 = 25;
const int pinData4 = 26;
const int pinClk = 24;
 
HX711 scale1;
HX711 scale2;
HX711 scale3;
HX711 scale4;

float calibration_factor = 21869;

double temp =0;
double hum =0;
double col =0;
double viby =0;
double m1 =0;
double m2 =0;
double m3 =0;
double m4 =0;

//--timer--
unsigned long previousMillis = 0;
unsigned long interval = 5000;



void setup() {
  Serial.begin(115200);
  
  dht.begin();
  ultrasonic.attach(TRIGPIN, ECHOPIN);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  
  scale1.begin(pinData1, pinClk);
  scale1.set_scale();
  scale1.tare();  //Reset the scale to 0
  long zero_factor1 = scale1.read_average();
  

  scale2.begin(pinData2, pinClk);
  scale2.set_scale();
  scale2.tare();  //Reset the scale to 0
  long zero_factor2 = scale2.read_average();
  

  scale3.begin(pinData3, pinClk);
  scale3.set_scale();
  scale3.tare();  //Reset the scale to 0
  long zero_factor3 = scale3.read_average();
  

  scale4.begin(pinData4, pinClk);
  scale4.set_scale();
  scale4.tare();  //Reset the scale to 0
  long zero_factor4 = scale4.read_average();
  
  nodemcu.begin(9600);
  delay(2000);

  pinMode(BAG1, OUTPUT); 
  pinMode(BAG2, OUTPUT); 
  pinMode(FAN1, OUTPUT);
  pinMode(TEMPFAN, OUTPUT);   
  pinMode(COMP, OUTPUT); 
  pinMode(POWCOM, OUTPUT);


  

  Serial.println("Program started");
}

void loop() {

 unsigned long currentMillis = millis();
 if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

  StaticJsonDocument<400> doc;
  
  //Obtain Temp and Hum data
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  Serial.print("Humidity: ");
  Serial.println(hum);
  Serial.print("Temperature: ");
  Serial.println(temp);
  
  //relay for airconditioning
  if (temp < 38.5) {
    digitalWrite(TEMPFAN, HIGH);
  }
  if (temp > 38.5){
    digitalWrite(TEMPFAN,LOW);
  }
  
  //honeyqty
  float sonred = ultrasonic.getPreciseDistanceCM(temp, hum);
  float tank = 12;
  float gap = tank - 2;
  float TEMP1 = sonred - 2; 
  float ACTLVL= gap - TEMP1;
  float col = ACTLVL / gap * 8.5;
  Serial.print("Collection: ");
  Serial.println(sonred);
  Serial.println(TEMP1);
  Serial.println(ACTLVL);
  Serial.println(col);
 
  //Vibration
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  float Acx=Wire.read()<<8|Wire.read();
  float Acyy=Wire.read()<<8|Wire.read();
  AcY = Acx ;
  float Acy = Acyy ;
  float AcZ=Wire.read()<<8|Wire.read();
  int xAng = map(AcX,minVal,maxVal,-90,90);
  int yAng = map(AcY,minVal,maxVal,-90,90);
  int zAng = map(AcZ,minVal,maxVal,-90,90);
  viby= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
  Serial.print("AngleY= ");
  Serial.println(viby);
  Serial.print("RawY= ");
  Serial.println(AcY);
  Serial.print("RawX= ");
  Serial.println(Acy);
  Serial.print("RawZ= ");
  Serial.println(AcZ);
  
  //Frames--------------------
  scale1.set_scale(calibration_factor);
  float m1 = (scale1.get_units());
  Serial.print("M1: ");
  Serial.println(m1);

  scale2.set_scale(calibration_factor);
  float m2 = (scale2.get_units());
  Serial.print("M2: ");
  Serial.println(m2);

  scale3.set_scale(calibration_factor);
  float m3 = (scale3.get_units());
  Serial.print("M3: ");
  Serial.println(m3);

  scale4.set_scale(calibration_factor);  
  float m4 = (scale4.get_units());
  Serial.print("M4: ");
  Serial.println(m4);
  //relay for weight
  float weight = m1 + m2 + m3 + m4;

 if (weight < 1.01){
  digitalWrite(BAG1, HIGH);
  digitalWrite(BAG2, HIGH);
  digitalWrite(POWCOM, HIGH);
  digitalWrite(COMP, HIGH);
  i = 0;

  if (weight < 1.01 && j == 1){
    j = 0;
    digitalWrite(POWCOM, LOW);
    delay(500);
    digitalWrite(BAG2, LOW);
    delay(1000);
    digitalWrite(COMP, LOW);
    delay(5000);
    digitalWrite(BAG2, HIGH);
    delay(1000);
    digitalWrite(COMP, HIGH);
    delay(500);
    digitalWrite(POWCOM, HIGH);
    delay(500);
  }
 }
 if (weight > 20){
  i = i + 1;
  
  
  if ((weight > 20) && i == 1){ //opening
    digitalWrite(POWCOM, LOW);
    delay(500);
    digitalWrite(BAG1, LOW);
    delay(1000);
    digitalWrite(COMP, LOW);
    delay(5000);
    digitalWrite(BAG1, HIGH);
    delay(1000);
    digitalWrite(COMP, HIGH);
    delay(500);
    digitalWrite(POWCOM, HIGH);
    delay(500);
    j = j+1;
  }
 }

  //Assign collected data to JSON Object
  doc["humidity"] = hum;
  doc["temperature"] = temp;
  doc["Collection"] = col;
  doc["y"] = AcY;
  doc["f1"] = m1;
  doc["f2"] = m2;
  doc["f3"] = m3;
  doc["f4"] = m4;
  doc["x"] = Acy;
  doc["z"] = AcZ;
  

  //Send data to NodeMCU
  nodemcu.print(serializeJson(doc, nodemcu));
 }
}
