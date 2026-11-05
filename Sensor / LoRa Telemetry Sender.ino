/*
Senders code
*/
#include <WiFi.h>
#include <WiFiUdp.h>
#define BAND    915E6 //868E6, 915E6 change frequency here
#include "LoRaWan_APP.h"
#include "Arduino.h"
#include "heltec.h"
#include "Wire.h"
#include "HT_SSD1306Wire.h"

#define SDA_OLED 41
#define SCL_OLED 42

#define RF_FREQUENCY                                915000000 // Hz
#define TX_OUTPUT_POWER                             5        // dBm
#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
//#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 30 // Define the payload size here

//char txpacket[BUFFER_SIZE];
//char rxpacket[BUFFER_SIZE];

//char rollchar[BUFFER_SIZE];
//char pitchchar[BUFFER_SIZE];

double txNumber;
bool lora_idle=true;
static RadioEvents_t RadioEvents;

int16_t roll = 0;  //initailize
int16_t pitch = 0;
uint8_t val = 100;

//MPU6050 Gyroscope parameters
const uint8_t addr = 0x68; 
size_t L = 14;
float xA, yA, zA, T, xG, yG, zG;
float xAcc, yAcc, zAcc, Temp, xGyr, yGyr, zGyr;

void OnTxDone( void );
void OnTxTimeout( void );

//int ntx = 0; // Tx packet counter
//long CTS; //timestamp
//int nic[3]; //Slot vector mapped from NIC
//String cid, sid, aid; //collector id, sensor id, acked id
//int msg; //
//String ssid;
//String pkgrx, pkgtx; // commented out
//char pkgrx[BUFFER_SIZE];
//char pkgtx[BUFFER_SIZE];

/* WiFi network name and password */
//const char * ssid_Router = "NETGEAR97";
//const char * password_Router = "yellowriver414";

// IP address to send UDP data to.
// it can be ip address of the server or 
// a network broadcast address
// here is broadcast address
//const char * udpAddress = "23.235.207.63";
const char * udpAddress = "192.168.0.10";
const int udpPort = 9990;
//create UDP instance
WiFiUDP udp;



SSD1306Wire OLEDdisplay(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); // addr, freq, 12c group, resolution, rst
//SSD1306Wire OLEDdisplay(0x3c, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); // addr, freq, 12c group, resolution, rst
//SSD1306Wire OLEDdisplay(0x3c, SDA_OLED, SCL_OLED, GEOMETRY_128_64); 


void setup(){
    Wire.begin();                     //gyroscope setup
    Wire.beginTransmission(addr);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);

    //Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
    Serial.begin(115200);     //Serial commnication and LoRa Setup
    Mcu.begin();
  
    //Set WiFi connection to station node and disconnect from an AP if it was previously connected
    //WiFi.disconnect(true);
    //delay(100);
    //WiFi.mode(WIFI_STA);
    //WiFi.setAutoConnect(true);
    //Connect to the WiFi network
    //WiFi.begin(ssid_Router, password_Router);
    //delay(100);
    //Serial.println("");
    // Wait for connection
    //while (WiFi.status() != WL_CONNECTED) {
    //    delay(500);
    //    Serial.print(".");
    //}
    //Serial.println("");
    //Serial.print("Connected to ");
    //Serial.println(ssid_Router);
    //Serial.print("IP address: ");
    //Serial.println(WiFi.localIP());

    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000);


    //LoRa.setSyncWord(0xF3);           // ranges from 0-0xFF, default 0x34, see API docs
    //LoRa.setSignalBandwidth(500E3); //125e3, 250e3, and 500e3
    //oRa.setSpreadingFactor(7); //7, 8, 9, 10, 11, 12
    //uint64_t macAddr = ESP.getEfuseMac(); //read MAC address
    //for(int i = 0; i<3; i++) {
    //nic[i] = (macAddr>>(40 - i * 8))&0xff; //nic
    //initialize OLED display

    //}
    //for(int i = 0; i<3; i++){
    //  String s = String(nic[2 -i], HEX);
    //  if(s.length() == 1) s = "0" + s;
    //  sid += s;
    //}

    OLEDdisplay.init();
    OLEDdisplay.clear();
    //OLEDdisplay.setTextAlignment(TEXT_ALIGN_LEFT);
    //OLEDdisplay.setFont(ArialMT_Plain_10);
    //OLEDdisplay.drawString(30, 20, "NIC: " + sid + "...");
    OLEDdisplay.display();
  
    //Heltec.display->clear();
    //Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    //Heltec.display->setFont(ArialMT_Plain_10);
    //Heltec.display->drawString(30, 20, "NIC: " + sid + "...");
    //Heltec.display->display();
    //delay(1000);
    //CTS = millis();
    //Serial.println(sid);
  }

//void sendMessage(char* packet);
void loop()
{
  if(lora_idle == true)
  {
    delay(1000);
    //Collect Gyroscope data
    Serial.println("------------------");
    Wire.beginTransmission(addr);  //communication with the sensor
    Wire.write(0x3B);                  //address of MPU where acc and gyro data can be read
    Wire.endTransmission(false);       //ends but does not release the I2C bus
    Wire.requestFrom(addr, L, true);   //request data from addr, Release the bus
  
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

    Serial.print("Roll:"); Serial.print(roll); Serial.print(",  ");
    Serial.print("Pitch:"); Serial.print(pitch); Serial.print(",  ");


    int16_t rollToSend = roll;
    int16_t pitchToSend = pitch;
    val += 1;

    //uint16_t buffer[BUFFER_SIZE];
    uint8_t buffer[BUFFER_SIZE];
    buffer[0] = rollToSend & 0xFF;
    buffer[1] = (rollToSend >> 8) & 0xFF;
    buffer[2] = pitchToSend & 0xFF;
    buffer[3] = (pitchToSend >> 8) & 0xFF;
    buffer[4] = val;

    txNumber += 0.01;

    Radio.Send(buffer, BUFFER_SIZE);
    Serial.print("Roll to send:"); Serial.print(rollToSend); Serial.print(",  ");
    Serial.print("Pitch to send:"); Serial.print(pitchToSend); Serial.print(",  ");

    lora_idle = false;

    //sending package
    //CTS = millis();
    //pkgtx = sid + ",";
    //ssid = sid + ",";
    //msg = random(0, 1000000); //sensor data
    //ssid += msg;
    //ssid.toCharArray(pkgtx, BUFFER_SIZE);
    //pkgtx += String(msg);
    //delay(38); //add to 50ms
    //delay(950); //adjust rate to 1Hz, 
    //sendMessage(pkgtx);

    
    OLEDdisplay.clear();
    OLEDdisplay.setTextAlignment(TEXT_ALIGN_LEFT);
    OLEDdisplay.setFont(ArialMT_Plain_10);
    OLEDdisplay.drawString(30, 20, String("roll: " + String(rollToSend)));
    OLEDdisplay.display();

    //OLEDdisplay.clear();
    //OLEDdisplay.drawString(0, 0, "packet counter: " + String(ntx));
    //OLEDdisplay.drawString(0, 10, String(millis() - CTS) + " (ms) >>  " + String(sid));
    //OLEDdisplay.drawString(0, 20, "message is: " + String(msg));
    //OLEDdisplay.display();

    //Heltec.display->clear();
    //Heltec.display->drawString(0, 0, "packet counter: " + String(ntx));
    //Heltec.display->drawString(0, 10, String(millis() - CTS) + " (ms) >>  " + String(pkgtx));
    //Heltec.display->display();
  
    //sending WIFI RSSI
    // int rssi = -1 * WiFi.RSSI();
    // Serial.println(rssi);
    // uint8_t buffer[50] = {rssi};
    // udp.beginPacket(udpAddress, udpPort);
    // udp.write(buffer, 1);
    // udp.endPacket();
    // memset(buffer, 0, 1);
    // udp.parsePacket();
    // if(udp.read(buffer, 1) > 0){
    //   Serial.print("Server to client: ");
    //   uint8_t ack = buffer[0];
    //   Serial.println((String)ack);
    // }
    // delay(1000);
  }
  Radio.IrqProcess( );
}

void OnTxDone( void )
{
	Serial.println("TX done......");
	lora_idle = true;
}


void OnTxTimeout( void )
{
    Radio.Sleep( );
    Serial.println("TX Timeout......");
    lora_idle = true;
}
//void sendMessage(char* packet)
//{
//  Radio.Send( (uint8_t *)packet, strlen(packet) ); //send the package out
  //LoRa.beginPacket();                 // start packet
  //LoRa.print(packet);                 // add payload
  //LoRa.endPacket();                     // finish packet and send it
  //ntx++;                           // increment message ID
//}
