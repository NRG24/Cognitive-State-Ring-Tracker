# NERVA Ring

An open-source, sensor-packed smart ring designed to monitor cognitive health and autonomic nervous system metrics rather than traditional fitness tracking. 

## Project Overview

The NERVA Ring shifts the focus of wearable technology away from step-counting and workouts toward real-time mental well-being. By embedding precise biosensors into a compact, flexible PCB form factor, the device continuously captures physiological indicators tied directly to stress, mental fatigue, and psychological arousal.

## Core Features

* **Cognitive Focus:** Tailored strictly for mental wellness monitoring rather than physical fitness or gym tracking.
* **Rigorous Hardware Validation:** Circuitry completely modeled, tested, and validated via LT-Spice to guarantee electrical correctness.
* **Ultra-Compact Layout:** All hardware components optimized to fit onto a highly constrained, circular flex PCB architecture.
* **Wireless Telemetry:** Real-time, low-latency data streaming to mobile devices using Bluetooth Low Energy (BLE).
* **Intelligent Software Pipeline:** Dynamic in-app data filtering to remove motion artifacts and anomalies, powering a custom algorithm that delivers a "Cognitive Score" out of 100 alongside meaningful behavioral insights.

## Hardware Architecture

* **Microcontroller / RF:** Raytac nRF52832 module handling low-power processing and BLE transmission.
* **Biometrics (PPG):** MAX30102 sensor capturing Heart Rate, Heart Rate Variability (HRV), and Blood Oxygen Saturation (SpO2).
* **Arousal Tracking (GSR):** Custom analog Galvanic Skin Response circuit to measure electrodermal activity and sympathetic nervous system activation.
* **Motion Filtering:** Integrated accelerometer dedicated to detecting and filtering out motion noise from raw biometric streams.



https://github.com/user-attachments/assets/21d7508c-1ee5-49b8-9717-f8bbbc1bc319

<img width="1526" height="1260" alt="image" src="https://github.com/user-attachments/assets/9e2d3639-5bc1-4c2b-b5d9-fed1e8b23c1c" />

<img width="1280" height="1024" alt="image" src="https://github.com/user-attachments/assets/bb193762-28db-4391-b3c4-8594ef432642" />

<img width="436" height="368" alt="Screenshot 2026-02-07 at 9 29 02 PM" src="https://github.com/user-attachments/assets/2e2c0f2e-898e-42cc-9a81-37765b89f813" />
<br>

<img width="1077" height="370" alt="Screenshot 2026-03-21 at 9 27 09 PM" src="https://github.com/user-attachments/assets/76b5d239-362e-46cb-b721-c6f8a9353247" />
<img width="718" height="494" alt="Screenshot 2026-02-11 at 9 21 46 PM" src="https://github.com/user-attachments/assets/ed82f334-0ea8-4d68-94f6-a0611016e22b" />
<img width="1101" height="417" alt="Screenshot 2026-03-21 at 9 28 39 PM" src="https://github.com/user-attachments/assets/bfdef878-dc1f-4e6c-975c-ed55c9a32da5" />

