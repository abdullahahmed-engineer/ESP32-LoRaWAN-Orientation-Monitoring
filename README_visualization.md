# WiFi IMU Visualization and Data Logger

A Python-based receiver and visualization tool that listens for UDP packets from an ESP32-based IMU sensor node, decodes the incoming motion data, logs it to a file, and renders a 3D rotating cube to represent the orientation changes in real time.

## Overview

This project receives a compact UDP packet containing roll and pitch values from a wireless sensor node and uses those values to:

- decode the IMU measurements
- log the values to a text file
- visualize the attitude using a rotating OpenGL cube
- update the cube orientation based on the received motion data

The application is intended for demonstrating real-time wireless telemetry acquisition and visualization in a prototype embedded systems project.

## Project Purpose

The purpose of this project is to provide a simple host-side system that:

- listens for telemetry from a remote microcontroller
- reconstructs motion data from the packet payload
- displays the motion in a visual form
- stores the captured data for analysis

This project is useful for academic demonstrations, research prototyping, and learning how sensor data can be interpreted and visualized from a remote embedded device.

## Features

- UDP socket receiver for network telemetry
- Decoding of roll and pitch values from incoming packets
- Real-time motion visualization using OpenGL
- Rotation of a 3D cube based on sensor orientation
- Data logging to a text file
- Simple and lightweight host-side processing

## Hardware and Software Requirements

### Hardware
- ESP32 or other microcontroller with IMU telemetry
- MPU-6050-based motion sensor
- Local Wi‑Fi network
- Host computer running the Python script

### Software
- Python 3
- Pygame
- PyOpenGL
- NumPy

Install dependencies using:


bash
pip install pygame PyOpenGL numpy

System Architecture
The system operates in two main parts:

Remote sensor node

reads motion data from an IMU
computes roll and pitch
sends a UDP packet to the host
Python receiver/visualizer

binds to the UDP port
receives and decodes the incoming packet
logs values
updates the OpenGL visualization
Data Packet Format
The incoming UDP payload is 5 bytes long:

Byte 0: roll low byte
Byte 1: roll high byte
Byte 2: pitch low byte
Byte 3: pitch high byte
Byte 4: counter value
The script reconstructs:

roll = int.from_bytes(data[0:2], byteorder='little', signed=True)
pitch = int.from_bytes(data[2:4], byteorder='little', signed=True)
val = data[4]

This allows the host to reconstruct the motion values from the compact binary packet.

Visualization
The project uses a 3D cube rendered with OpenGL. The cube rotates around the X and Y axes according to the received roll and pitch values. The orientation is updated continuously as new data arrives.

This provides a simple visual representation of the motion being measured by the remote sensor.

Logging
The script writes received values to a file called:

gyroscope_data.txt

Each logged line contains:

Roll: <value>, Pitch: <value>, Counter: <value>

This allows the data to be reviewed or processed later.

How to Run
Start the sensor node and ensure it is sending UDP packets to the host.
Configure the host IP and UDP port in the transmitter code.
Run the Python script
Observe the rotating cube and the logged sensor values in the terminal.

Example Output
Roll: 12, Pitch: -6, Counter: 104
Roll: 13, Pitch: -7, Counter: 105
Roll: 15, Pitch: -8, Counter: 106

Notes and Limitations
This project is a prototype and has the following limitations:

fixed UDP port configuration
minimal error handling
basic visualization rather than a full motion-tracking system
no advanced filtering or calibration for the sensor data
no secure network communication
This makes it suitable for educational and experimental use, but not for production deployment.

Project Status
Status: Prototype / Proof of Concept

License
This project is intended for educational and research use. Please ensure compliance with institutional or project-specific usage policies before public or commercial deployment.

Acknowledgements
This project was developed as part of a wireless embedded systems and sensor telemetry investigation focused on real-time monitoring and visualization of motion data.
