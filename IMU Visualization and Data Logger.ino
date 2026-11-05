/*
RC remote control through WiFi
version 0.3
date:   8/22/23
*/
#include <WiFi.h>
#include <WiFiUdp.h>
#include<Wire.h>
// #include "Adafruit_VL53L0X.h"
const char *ssid_Router     = ""; 
const char *password_Router = "";  

const char * udpAddress = ""; //the IP address of your computer in local WiFi
const int udpPort = 9990;  //Port number used for UDP connection
WiFiUDP udp; //WiFiUDP class by Adruino framework

uint16_t roll = 0;
uint16_t pitch = 100;   

uint8_t val = 100;       //declearing variables

const uint8_t addr = 0x68;   //I2C address of MPU 6050, is a 8-bit value
size_t L = 14;
int16_t xA, yA, zA, T, xG, yG, zG;
float xAcc, yAcc, zAcc, Temp, xGyr, yGyr, zGyr;

  

void setup(){  //keeps on iterating as long as arduino is powered
  //Wi-Fi connection set up
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  WiFi.begin(ssid_Router, password_Router);
  while (WiFi.isConnected() != true){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print(WiFi.localIP());
  Serial.println("");

  //Gyroscope setup
  Wire.begin();
  Wire.beginTransmission(addr);  //communication with the sensor
  Wire.write(0x6B);                  //address of MPU where acc and gyro data can be read
  Wire.write(0);       //ends but does not release the I2C bus
  Wire.endTransmission(true);   //request data from addr, Release the bus
  
}

void loop(){
  Wire.beginTransmission(addr);  
  Wire.write(0x3B);                  
  Wire.endTransmission(false);       
  Wire.requestFrom(addr, L, true);   

  xA = (Wire.read()<<8|Wire.read());     //raw data collected/ 2 bytes coming from I2C
  yA = (Wire.read()<<8|Wire.read());     // '|' bitwise OR operator--->combining two bytes into a 16bit
  zA = (Wire.read()<<8|Wire.read());     // '<<8' shift location by 8 bits(1byte)
  T =  (Wire.read()<<8|Wire.read());
  xG = (Wire.read()<<8|Wire.read());
  yG = (Wire.read()<<8|Wire.read());
  zG = (Wire.read()<<8|Wire.read());

  
  xAcc = xA/16384.0;         //converting raw data values for each by set scaling values
  yAcc = yA/16384.0;
  zAcc = zA/16384.0;
  xGyr = xG/131.0;
  yGyr = yG/131.0;
  zGyr = zG/131.0;
  Temp = T/340.0 + 36.53;

  
  //Roll & Pitch Equations from accelerometers only
  float roll  = atan2(yAcc, zAcc) * 180.0/M_PI;
  float pitch = atan2(-xAcc, sqrt(zAcc * zAcc + yAcc * yAcc)) * 180.0/M_PI;
 
  Serial.print("Roll:"); Serial.print(roll); Serial.print(", ");
  Serial.print("Pitch:"); Serial.print(pitch); Serial.print(", ");
  Serial.println();

  int16_t rollToSend = roll;
  int16_t pitchToSend = pitch;
  val += 1;
  uint8_t buffer[5];
  buffer[0] = rollToSend & 0xFF;
  buffer[1] = (rollToSend >> 8) & 0xFF;
  buffer[2] = pitchToSend & 0xFF;
  buffer[3] = (pitchToSend >> 8) & 0xFF;
  buffer[4] = val;

  udp.beginPacket(udpAddress, udpPort);
  udp.write(buffer, 5);
  udp.endPacket();
  memset(buffer, 0, 5);

  delay(100);


}

  
  



  
