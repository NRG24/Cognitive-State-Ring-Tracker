This is a sensor packed device that features a GSR sensor and max30102 sensor that measure skin electrical conductivity (arousal), temperature, spo2, heart rate, and HRV. The point of this is to build a tracker that isn't focused on fitness, but cognitive health tied to the nervous system. These sensors will be packed into a small PCB that goes on one of the two rings. There is a ring on the index finger with the PCB+ one electrode and the other ring on the middle finger has the other electrode. This data is controlled by the nrf52840, a small MCU on the PCB, and sent via Bluetooth low energy to a mobile based Flutter app. The data goes through a filter, removing any outliers. Additionally, in the app there is a "Cognitive Score." The app interprets all the data and based off of an algorithm, it can give you a score out of 100 as well as some insights.

In this GitHub, I have the PCB, Schematic, and all of the KiCad data, the Flutter app code, the code for the MCU, the 3D CAD Model, and the Bill of Materials Link Below.

My PCB and Schematic are in the Split Ring folder.


<img width="795" height="423" alt="Screenshot 2026-01-06 at 12 37 14 PM" src="https://github.com/user-attachments/assets/9eccdecf-2001-4705-b264-248e9f64f14d" />

<img width="380" height="435" alt="Screenshot 2026-01-06 at 12 13 58 PM" src="https://github.com/user-attachments/assets/e46cc063-5446-4d6e-97bd-e6fb506012dc" />
<img width="305" height="354" alt="Screenshot 2026-01-06 at 12 14 04 PM" src="https://github.com/user-attachments/assets/9c35dfe5-4c0c-4184-b900-8f5e1ad9a757" />
<img width="860" height="544" alt="Screenshot 2026-01-06 at 12 14 09 PM" src="https://github.com/user-attachments/assets/baad2086-63c9-48f9-b7e0-16869e23e1fb" />
