This is a sensor packed device that features a GSR sensor and max30102 sensor that measure skin electrical conductivity (arousal), temperature, spo2, heart rate, and HRV. The point of this is to build a tracker that isn't focused on fitness, but cognitive health tied to the nervous system. These sensors will be packed into a small PCB that goes on one of the two rings. There is a ring on the index finger with the PCB+ one electrode and the other ring on the middle finger has the other electrode. This data is controlled by the nrf52840, a small MCU on the PCB, and sent via Bluetooth low energy to a mobile based Flutter app. The data goes through a filter, removing any outliers. Additionally, in the app there is a "Cognitive Score." The app interprets all the data and based off of an algorithm, it can give you a score out of 100 as well as some insights.

In this GitHub, I have the PCB, Schematic, and all of the KiCad data, the Flutter app code, the code for the MCU, the 3D CAD Model, and the Bill of Materials Link Below.


<img width="852" height="432" alt="Screenshot 2025-12-12 at 11 38 10 PM" src="https://github.com/user-attachments/assets/3e590c1b-528f-4147-ade6-f91b583a9231" />

Here are pictures of my PCB:

<img width="807" height="701" alt="Screenshot 2025-12-13 at 11 49 00 AM" src="https://github.com/user-attachments/assets/746d098b-2ffa-4a75-8c62-d2e36283b3f3" />
<img width="549" height="395" alt="Screenshot 2025-12-13 at 11 50 08 AM" src="https://github.com/user-attachments/assets/0b39c7f4-953d-454a-91fd-9111fd6d3f08" />
