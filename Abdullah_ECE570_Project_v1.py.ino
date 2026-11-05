#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>

const char *ssid_Router = "NETGEAR97";
const char *password_Router = "yellowriver414";

const char *udpAddress = "192.168.0.16";
const int udpPort = 9990;
WiFiUDP udp;

const uint8_t addr = 0x68;
size_t L = 14;
int16_t xA, yA, zA, T, xG, yG, zG;
float xAcc, yAcc, zAcc;

const int BUFFER_SIZE = 100; // Adjust this based on your requirements
float bufferX[BUFFER_SIZE];
float bufferY[BUFFER_SIZE];
float bufferZ[BUFFER_SIZE];
int bufferIndex = 0;

uint8_t val = 100; // Declare 'val' here
uint8_t ack;

float Temp, roll, pitch; // Declare temperature, roll, and pitch

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    WiFi.begin(ssid_Router, password_Router);
    while (WiFi.isConnected() != true) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print(WiFi.localIP());

    Wire.begin();
    Wire.beginTransmission(addr);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);
}

void loop() {
    val += 1;
    uint8_t buffer[2] = {ack, val};
    udp.beginPacket(udpAddress, udpPort);
    udp.write(buffer, 2);
    udp.endPacket();
    memset(buffer, 0, 2);

    udp.parsePacket();
    if (udp.read(buffer, 1) > 0) {
        Serial.print("Server to client: ");
        ack = buffer[0];
        Serial.println((String)ack);
    }

    // Read sensor data
    float x = readSensorX();
    float y = readSensorY();
    float z = readSensorZ();

    // Store data in the buffer
    bufferX[bufferIndex] = x;
    bufferY[bufferIndex] = y;
    bufferZ[bufferIndex] = z;

    // Increment the buffer index and wrap around if necessary
    bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;

    // Calculate temperature (Temp)
    Temp = (T / 340.0) + 36.53;

    // Calculate roll and pitch
    roll = atan2(y, z) * 180.0 / PI;
    pitch = atan2(-x, sqrt(y * y + z * z)) * 180.0 / PI;

    // Print X, Y, and Z values to Serial Monitor
    Serial.print("X: ");
    Serial.print(x);
    Serial.print(" Y: ");
    Serial.print(y);
    Serial.print(" Z: ");
    Serial.println(z);

    // Handle communication with Python
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        if (command == "request_data") {
            // Send the buffered data, temperature, roll, and pitch back to Python
            for (int i = 0; i < BUFFER_SIZE; i++) {
                // Send the buffered data, temperature, roll, and pitch back to Python
                Serial.print(x);
                Serial.print(',');
                Serial.print(y);
                Serial.print(',');
                Serial.print(z);
                Serial.print(',');
                Serial.print(Temp);
                Serial.print(',');
                Serial.print(roll);
                Serial.print(',');
                Serial.println(pitch);


            }
        }
    }

    // Add a delay to control the packet transmission rate
    delay(100); // Adjust the delay as needed
}

float readSensorX() {
    Wire.beginTransmission(addr);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(addr, L, true);
    
    int16_t xA = (Wire.read() << 8 | Wire.read());
    T = (Wire.read() << 8 | Wire.read());
    int16_t xG = (Wire.read() << 8 | Wire.read());

    xAcc = xA / 16384.0;
    return xAcc; // Return the X-axis acceleration value
}

float readSensorY() {
    // Implement your sensor reading logic for the Y-axis here
    Wire.beginTransmission(addr);
    Wire.write(0x3D); // Adjust to 0x3D for Y-axis
    Wire.endTransmission(false);
    Wire.requestFrom(addr, L, true);
    int16_t yA = (Wire.read() << 8 | Wire.read());
    int16_t yG = (Wire.read() << 8 | Wire.read());
    yAcc = yA / 16384.0;
    return yAcc; // Return the Y-axis acceleration value
}

float readSensorZ() {
    // Implement your sensor reading logic for the Z-axis here
    Wire.beginTransmission(addr);
    Wire.write(0x3F); // Adjust to 0x3F for Z-axis
    Wire.endTransmission(false);
    Wire.requestFrom(addr, L, true);
    zA = (Wire.read() << 8 | Wire.read());
    zG = (Wire.read() << 8 | Wire.read());
    zAcc = zA / 16384.0;
    return zAcc; // Return the Z-axis acceleration value
}

