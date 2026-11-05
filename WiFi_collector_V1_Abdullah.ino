/*
socket send UDP packet through Verizon MiFi
version 0.1
date:   6/13/23
*/
#include "LoRaWan_APP.h"
#include "Arduino.h"
#include "heltec.h"
#include "HT_SSD1306Wire.h"
#include <WiFi.h>
#include <WiFiUdp.h>

#define BAND    915E6 //868E6, 915E6
#define RF_FREQUENCY                                915000000 // Hz
#define TX_OUTPUT_POWER                             14        // dBm  (zzz))
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
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 30 

static RadioEvents_t RadioEvents;
int16_t txNumber;
int16_t pitch, roll;
uint8_t val = 100;



/* WiFi network name and password */
// const char * ssid = "TP-LINK_B0BA";
// const char * pwd = "42602682";
const char * ssid_Router = "NETGEAR97";
const char * password_Router = "yellowriver414";

// const char * udpAddress = "192.168.0.101";
const char * udpAddress = "192.168.0.10";
const int udpPort = 9990;
//create UDP instance
WiFiUDP udp;

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];
int16_t rssi,rxSize;
bool lora_idle = true;
SSD1306Wire OLEDdisplay(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); // addr, freq, 12c group, resolution, rst

//uint32_t ntx = 0; // Tx packet counter
//uint32_t nrx = 0; // Rx packet counter
//long CTS1; //timestamp 1
//long CTS2; //timestamp 2
//int Npkg = 0; //packet length
//String pkgtx = ""; //Tx packet
//String pkgrx = ""; //Rx packet
//int rssi = 0; //rssi in dBm
//String ssAdd;
//int sid[2];

void setup(){
  Serial.begin(115200);     //Serial commnication and LoRa Setup
  Mcu.begin();

  txNumber = 0;
  rssi = 0;
  pitch = 0;
  roll = 0;

  RadioEvents.RxDone = OnRxDone;
  Radio.Init( &RadioEvents );
  Radio.SetChannel( RF_FREQUENCY );
  Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                               LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                               LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                               0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
  //Initialize OLED display
  OLEDdisplay.init();
  OLEDdisplay.clear();
  OLEDdisplay.display();


  
  //Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  //Serial.begin(115200);
  //LoRa.setSyncWord(0xF3);           // ranges from 0-0xFF, default 0x34, see API docs
  //LoRa.setSignalBandwidth(500E3); //125e3, 250e3, and 500e3
  //LoRa.setSpreadingFactor(7); //7, 8, 9, 10, 11, 12

  //Heltec.display->clear();
  //Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  //Heltec.display->setFont(ArialMT_Plain_10);
  //Heltec.display->drawString(30, 20, "LoraWAN Collector");
  //Heltec.display->display();
  
  //Connect to the WiFi network
  //WiFi.begin(ssid, pwd);
  //Serial.println("");

  Serial.setDebugOutput(true);
  Serial.println();
  WiFi.begin(ssid_Router, password_Router);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid_Router);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
}

void loop(){
  if(lora_idle)
  {
    lora_idle = false;
    Serial.println("into RX mode");
    Radio.Rx(0);

  }
  Radio.IrqProcess( );
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    rssi = rssi;
    txNumber++;
    
    int16_t rollReceived = payload[0] | (payload[1] << 8);
    int16_t pitchReceived = payload[2] | (payload[3] << 8);

    Radio.Sleep( );

    Serial.printf("\r\nreceived packet \"%s\" with Pitch: %d and Roll: %d, RSSI: %d, length %d\r\n",txNumber,pitchReceived, rollReceived, rssi, size);
    lora_idle = true;

    OLEDdisplay.clear();
    OLEDdisplay.setTextAlignment(TEXT_ALIGN_LEFT);
    OLEDdisplay.setFont(ArialMT_Plain_10);
    OLEDdisplay.drawString(0, 0, "Hello, ESP32!");
    //OLEDdisplay.drawString(0, 0, String("roll: " + String(rollReceived) + ", pitch:" + String(pitchReceived)));
    OLEDdisplay.display();

    if (OLEDdisplay.init()) {
    Serial.println("OLED initialization failed!");
    while (1);
}

    val +=1;
    
    uint8_t buffer[5];
    buffer[0] = rollReceived & 0xFF;
    buffer[1] = (rollReceived >> 8) & 0xFF;
    buffer[2] = pitchReceived & 0xFF;
    buffer[3] = (pitchReceived >> 8) & 0xFF;
    buffer[4] = val;

    udp.beginPacket(udpAddress, udpPort);
    udp.write(buffer, 5);
    udp.endPacket();
    memset(buffer, 0, 5);

    delay(100);

}


  //Npkg = LoRa.parsePacket();
  //if (Npkg != 0){
    //nrx++;
    //pkgrx = "";
    //while (LoRa.available()) pkgrx += (char)LoRa.read();
    //uint8_t rssi = LoRa.packetRssi() * -1;
    // sid = 1 + int(ssAdd.indexOf(pkgrx.substring(0, 6))/8);
    //sid[0] = pkgrx.charAt(4) - '0';
    //sid[1] = pkgrx.charAt(5) - '0';
    //Serial.print(String(nrx) + "," + String(millis() - CTS1) + "," + sid[0] + sid[1] + "," + String(rssi));
    //Serial.println();
  //uint8_t buffer[3] = {rssi, sid[0], sid[1]};
  //udp.beginPacket(udpAddress, udpPort);
  //udp.write(buffer, 3);
  //udp.endPacket();
  //memset(buffer, 0, 3);
  //}
  // udp.parsePacket();
  // if(udp.read(buffer, 1) > 0){
  //   Serial.print("Server to client: ");
  //   uint8_t ack = buffer[0];
  //   Serial.println((String)ack);
  // }
//}