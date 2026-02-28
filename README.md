# ROS 2 Custom Hardware Interface

Hardware-in-the-loop DC motor control using ROS 2, Raspberry Pi, and Arduino with encoder feedback and real-time visualization.

# Overview

This repository implements a custom hardware integration stack for controlling a real DC motor through ROS 2.
Unlike simulation-only projects, this system runs a real closed-loop motor control pipeline:
High-level commands are generated in ROS 2
Commands are sent to an Arduino over serial
The Arduino executes PID + feedforward control
Encoder feedback is returned to ROS
Joint states are updated and visualized in RViz
The goal of this project is to structure hardware control in a way that mirrors real robotic system architectures.

## 🎥 Demo

[![Hardware Interface Demo](https://img.youtube.com/vi/DjoeGvIWBWg/0.jpg)](https://youtu.be/DjoeGvIWBWg)

▶ Click the image to watch the full hardware-in-the-loop demonstration.

# Workspace Structure
src/
 ├── my_py_pkg1
 │    └── Python control and communication node
 │
 ├── robot_firmware
 │    ├── include/robot_firmware
 │    └── src
 │    → C++ hardware interface implementation
 │    └── robot_interface.xml
      → Hardware interface/plugin declaration
 ├── my_urdfs
 │    └── urdf
 │    → Robot model and joint definitions
 │
 ├── program_bringup
 │    ├── launch
 │    └── config
 │    → Launch files and runtime configuration
 │
 
# System Architecture

User Input (Joystick or Command Node)
↓
ROS 2 Python Node
↓
C++ Hardware Interface Layer
↓
Serial Communication
↓
Arduino (PID + Feedforward Controller)
↓
Motor Driver
↓
DC Motor
↑
Encoder Feedback
↑
Arduino → Serial → ROS 2
↓
Joint State Publisher → RViz

As the physical motor rotates, the corresponding joint in RViz updates in real time.

# Package Responsibilities
## my_py_pkg1

Subscribes to velocity or control commands
Sends formatted commands to hardware layer
Publishes joint states based on encoder feedback
Acts as high-level control interface

## robot_firmware (C++)

Implements hardware communication
Handles serial transmission
Parses encoder feedback
Provides structured interface between ROS and embedded controller

## my_urdfs

Defines robot joint configuration
Ensures RViz visualization matches physical hardware
Maintains consistent frame structure

## program_bringup

Launches required nodes
Loads robot description
Starts RViz
Configures runtime parameters

# Control Loop Description

Command is issued from teleop or control node.
Python node forwards target velocity to hardware interface.
Hardware interface sends target velocity to Arduino.
Arduino executes PID + feedforward control.
Encoder measures motor motion.
Encoder data is sent back to ROS.
Joint state message is published.
RViz reflects real motor movement.
This creates a hardware-in-the-loop validation environment.

# Key Technical Concepts Demonstrated

ROS 2 multi-package architecture
Mixed Python and C++ stack
Hardware abstraction layer
Bidirectional serial communication
Encoder feedback integration
Embedded PID control
Real-time state synchronization
Separation of high-level and low-level control

# Current Scope

Single motor validation
No full differential drive implementation yet
Focused on validating hardware control pipeline

# Build Instructions

```bash
From workspace root:

colcon build
source install/setup.bash
Launch system:
ros2 launch program_bringup <your_launch_file>.launch.py
```

Ensure:

Arduino firmware is flashed
Correct serial port is configured
Motor driver is powered

# Demo

Hardware-in-the-loop demonstration:
https://youtu.be/DjoeGvIWBWg

# Future Work

Extend to dual motor differential drive
Convert to full ros2_control SystemInterface
Deploy on complete mobile robot platform

# Why This Project Matters

This repository marks the transition from simulation-based robotics to physical hardware integration.
The architecture is intentionally designed to scale toward:
Full mobile robot platforms

ros2_control-compliant systems

Hardware abstraction suitable for real-world deployment
