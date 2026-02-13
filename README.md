# Formula Car Dashboard

This repository contains the firmware and design files for a custom Formula Student car dashboard.  
The system is built around an **STM32F103C8T6 (Blue Pill)** microcontroller and a **Nextion HMI display**, designed to provide real-time data to the driver.

---

## Dashboard

<img width="443" height="248" alt="image" src="https://github.com/user-attachments/assets/aa8bcdd1-40d6-4e90-99b1-3f517eaf8cf5" />

---

## Project Overview
The dashboard is a custom module that acts as a **listening node** on the vehicle's CAN bus.  
It receives and interprets data broadcasted by other systems on the vehicle's network. The STM32 microcontroller processes these CAN messages and translates them into visual updates on a **Nextion NX4827P043-011C display** via the UART protocol, delivering essential information to the driver during a race.

---

## Key Hardware
- **Microcontroller**: STM32F103C8T6 ("Blue Pill")  
- **HMI Display**: Nextion NX4827P043-011C (4.3" Capacitive Touch)

---

## System Architecture & Features
The dashboard interface is built around a **single Racer Page**, serving as the driver's main hub during a race.

### Displayed Parameters
- **Electrical RPM (ERPM):** Visualized as a dynamic slider/gauge.  
- **Vehicle Speed:** Calculated in real-time from the two front wheel proximity sensors.  
- **Motor Temperature:** Monitors motor health to prevent overheating.  
- **Battery State:** Displays the current state of charge or voltage.  
- **Throttle Position:** Represented by a slider indicating throttle input.  
- **Brake Pressure:** Represented by a slider indicating braking input.  
- **Lap Stopwatch:** Fully functional stopwatch to track lap times.  

---

## Lap Stopwatch Functionality
The stopwatch is controlled directly by the driver using **steering wheel buttons**:

- **Start/Stop:** Begin or end a lap timing session.  
- **Pause/Unpause:** Temporarily pause and resume the timer.  
- **Best Lap:** Automatically records and displays the best lap time whenever the stopwatch is stopped.  

---

## CAN Bus Data Reception
The STM32 microcontroller is used to process CAN data.

- **Listening Node:** Continuously monitors the CAN bus for incoming data packets.  
- **Message Filtering:** Used filters to ensure only relevant CAN IDs (ERPM, motor temp, etc.) are processed.  
- **Data Parsing:** Extracts and encodes data from received messages.  
- **Display Update:** Sends formatted command strings via UART to update the Nextion display visuals.  

---

## Data Update Logic
### Vehicle Speed (Proximity Sensors)
- **Mechanism:** Proximity sensors are mounted near the front wheels. As a wheel rotates, triggers (like bolt heads or magnets) on the hub pass by the sensor, which generates a digital pulse for each pass..  
- **Calculation:** The STM32 microcontroller captures these pulses as interrupts. By measuring the time between consecutive pulses, it calculates the rotational speed (RPM) of each front wheel. This RPM value, combined with the known circumference of the tire, is used to calculate the linear speed of the vehicle.  

### CAN-Based Parameters
- **ERPM, Throttle Position, Brake Pressure, Motor Temp, Battery State**  
  → Updated in real-time from CAN messages.  

---

## Illustration

[Dashboard Illustration Video](https://drive.google.com/file/d/1BPAwngIeQNHiXKz2zAf2UPMLvtQwkGAO/view?usp=sharing)


---

## Getting Started
To run this project, you will need:

### Hardware
- STM32F103C8T6 board  
- Nextion NX4827P043-011C display  

### Software
- **IDE for STM32 development:** STM32CubeIDE  
- **Nextion Editor:** For uploading the HMI file to the display  

### Setup
1. Flash the firmware from this repository onto the STM32 microcontroller.  
2. Upload the `.HMI` file (found in `/GUI`) to the display using the Nextion Editor.  

---
