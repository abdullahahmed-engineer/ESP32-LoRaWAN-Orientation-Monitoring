# WiFi Collector / LoRa Telemetry Forwarder

A prototype embedded system that receives LoRa packets, decodes motion-related sensor values, and forwards the data over a Wi‑Fi UDP connection. This project is designed as a proof-of-concept wireless telemetry collector for laboratory and academic applications.

## Overview

This project demonstrates a simple end-to-end wireless data collection workflow:

- A remote sensor node transmits motion data over LoRa.
- A Heltec ESP32 LoRa-enabled board receives the packet.
- The payload is decoded into roll and pitch values.
- The data is forwarded to a configured UDP endpoint over Wi‑Fi.
- Serial output and OLED display provide quick status and debugging information.

The design is intended for embedded systems experimentation, wireless communication analysis, and prototype data acquisition.

## Project Purpose

The goal of this project is to investigate how wireless sensor telemetry can be received and relayed using:

- LoRa communication
- Wi‑Fi networking
- Embedded microcontrollers
- Real-time packet forwarding

This is a prototype system and is intended for research, classroom demonstrations, and proof-of-concept testing rather than production use.

## Features

- LoRa packet reception
- Wi‑Fi connection to a local network
- UDP transmission to a remote host
- Payload decoding for roll and pitch values
- RSSI measurement for signal quality estimation
- Serial monitoring for debugging
- OLED display for status updates
- Simple packet counting and metadata handling

## Hardware Requirements

- Heltec ESP32 LoRa development board
- LoRa transmitter or test packet source
- Wi‑Fi router or mobile hotspot
- Host computer or server receiving UDP packets
- Optional OLED display (integrated on the board)

## Software Requirements

This project is written for the Arduino IDE with ESP32 support and the Heltec board package installed.

Required libraries:
- Arduino.h
- heltec.h
- WiFi.h
- WiFiUdp.h
- LoRaWan_APP.h
- HT_SSD1306Wire.h

## System Architecture

The receiver node performs the following:

1. Initializes the LoRa radio and Wi‑Fi module.
2. Connects to the configured Wi‑Fi network.
3. Enters LoRa receive mode.
4. Waits for an incoming packet.
5. When a packet is received, decodes the payload.
6. Extracts roll and pitch values from the payload.
7. Sends the values to a UDP destination using Wi‑Fi.
8. Displays output on the OLED screen and serial monitor.

## LoRa Configuration

The project uses a LoRa configuration designed for short-range telemetry testing:

- Frequency: `915000000 Hz`
- Output power: `14 dBm`
- Bandwidth: `125 kHz`
- Spreading factor: `7`
- Coding rate: `4/5`
- Preamble length: `8`

These settings are suitable for prototype testing and demonstration in a controlled environment.

## Packet Structure

The payload is decoded from a compact binary packet, where the first four bytes represent the measured roll and pitch values:

- Byte 0: roll low byte
- Byte 1: roll high byte
- Byte 2: pitch low byte
- Byte 3: pitch high byte

The system then forwards this information using UDP to a configured receiver.

## Network Configuration

Before uploading the code, update the Wi‑Fi and UDP settings:

```cpp
const char * ssid_Router = "NETGEAR97";
const char * password_Router = "yellowriver414";

const char * udpAddress = "192.168.0.10";
const int udpPort = 9990;

Replace these values with the actual local network credentials and target host IP.

Example Serial Output

into RX mode
received packet "1" with Pitch: 125 and Roll: -42, RSSI: -75, length 4
This output confirms that a packet was received, decoded, and forwarded successfully.

OLED Display
The OLED display is used to show message status and quick device feedback. It helps verify that the board is alive and receiving packets correctly without needing constant Serial monitoring.

Notes and Limitations
This is a prototype and not a production-ready communication system. The implementation includes:

hardcoded Wi‑Fi credentials
static IP and port configuration
simple packet handling
minimal validation and error checking
no encryption or authentication
These limitations are acceptable for a research or educational prototype, but the system should not be treated as secure or robust for deployment in real-world environments.

Project Status
Status: Prototype / Proof of Concept

License
This project is intended for educational and research use. Please check with the project owner before using it in a public or commercial deployment.

Acknowledgements
This project was developed as part of an embedded systems and wireless communication investigation focused on telemetry forwarding and LoRa-based data collection.
