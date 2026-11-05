# WiFi IMU Telemetry Sender

A lightweight embedded prototype that reads motion data from an MPU-6050 accelerometer/gyroscope, computes roll and pitch angles, and transmits the values over a Wi‑Fi UDP connection to a remote host. This project is intended for wireless sensor telemetry, embedded systems experimentation, and simple real-time motion monitoring.

## Overview

This project demonstrates a compact wireless sensing workflow:

- The ESP32 reads raw motion data from an MPU-6050 sensor over I2C.
- The system converts raw sensor values into acceleration and angular measurements.
- Roll and pitch are calculated in real time from the accelerometer data.
- The computed values are packed into a small UDP payload.
- The payload is transmitted to a configured IP address and port over Wi‑Fi.
- Serial output is used to monitor the measured values and verify communication.

The code is designed as a prototype for academic, laboratory, or embedded systems research use.

## Project Purpose

The goal of this project is to explore how embedded sensors can be used to transmit motion data wirelessly to a host computer or receiving application. The project focuses on:

- Sensor integration with an MPU-6050
- Real-time IMU data acquisition
- Calculation of roll and pitch
- Wireless data transfer using UDP
- Proof-of-concept remote monitoring

## Features

- MPU-6050 sensor interfacing over I2C
- Real-time acceleration data acquisition
- Roll and pitch estimation
- UDP communication over Wi‑Fi
- Serial debugging output
- Compact binary packet transmission
- Simple, low-cost telemetry design

## Hardware Requirements

- ESP32 development board
- MPU-6050 accelerometer/gyroscope module
- Wi‑Fi router or local access point
- Host computer or server listening on UDP port 9990
- Breadboard and jumper wires

## Software Requirements

This project is written in Arduino C++ for the ESP32 platform.

Required tools:
- Arduino IDE
- ESP32 board support package
- Necessary Arduino libraries:
  - WiFi.h
  - WiFiUdp.h
  - Wire.h

## System Architecture

The sender node performs the following steps:

1. Connects to the configured Wi‑Fi network.
2. Initializes the MPU-6050 sensor.
3. Reads raw acceleration and gyroscope register values.
4. Converts values into engineering units.
5. Computes roll and pitch.
6. Packages the values into a 5-byte UDP message.
7. Sends the message to the target IP and port.
8. Repeats the process continuously.

## Sensor Processing

The code reads acceleration data from the MPU-6050 registers starting at `0x3B`. These values are then converted to usable acceleration units:
xAcc = xA / 16384.0;
yAcc = yA / 16384.0;
zAcc = zA / 16384.0;

Gyroscope values are also converted using the standard MPU-6050 scale factor:
xGyr = xG / 131.0;
yGyr = yG / 131.0;
zGyr = zG / 131.0;

Roll and pitch are estimated from the acceleration vector using:
float roll  = atan2(yAcc, zAcc) * 180.0 / M_PI;
float pitch = atan2(-xAcc, sqrt(zAcc * zAcc + yAcc * yAcc)) * 180.0 / M_PI;

These values are then transmitted to the receiving system.

Network Configuration
The Wi‑Fi SSID, password, and UDP target address are configured in the sketch:

const char *ssid_Router     = "NETGEAR97";
const char *password_Router = "yellowriver414";

const char * udpAddress = "192.168.0.16";
const int udpPort = 9990;

const char *ssid_Router     = "NETGEAR97";
const char *password_Router = "yellowriver414";

const char * udpAddress = "192.168.0.16";
const int udpPort = 9990;

Before running the code, update these with your own network details and the IP address of the receiving machine.

UDP Packet Format
The sketch sends a 5-byte UDP payload:

Byte 0: roll low byte
Byte 1: roll high byte
Byte 2: pitch low byte
Byte 3: pitch high byte
Byte 4: counter value
This compact format is useful for real-time monitoring and simple data logging.

Example Serial Output

Roll: 12.45, Pitch: -7.21
Roll: 12.81, Pitch: -7.35
Roll: 13.02, Pitch: -7.40

This data confirms the sensor is responding and the UDP packet stream is active.

Project Status
Status: Prototype / Proof of Concept

Notes and Limitations
This project is not intended for production deployment. It includes:

hardcoded Wi‑Fi credentials
static network configuration
basic error handling only
simple packet formatting with no encryption or authentication
These tradeoffs make it suitable for learning, experimentation, and lightweight telemetry development, but not for secure or industrial-grade deployment.

License
This project is intended for educational and research use. Please check with the project owner before using it in a public or commercial setting.

Acknowledgements
This project was developed as part of an embedded systems and wireless communication experiment focused on real-time motion sensing and Wi‑Fi-based telemetry transmission.

xAcc = xA / 16384.0;
yAcc = yA / 16384.0;
zAcc = zA / 16384.0;
