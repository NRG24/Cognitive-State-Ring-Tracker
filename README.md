This is a sensor packed device that features a GSR sensor and max30102 sensor that measure skin electrical conductivity (arousal), temperature, spo2, heart rate, HRV, and an accelerometer for filtering. The point of this is to build a tracker that isn't focused on fitness, but cognitive health tied to the nervous system. These sensors will be packed into a small flex PCB in a ring shape.. All of the circuitry has been tested in LT-Spice to ensure electrical correctness. The key point of this project is to have this ring focused on mental and cognitive health rather than fitness and physical health. So, the sensors send data to the Raytac nRF52840 module and the Raytac sends it to a mobile device via BLE. The data goes through a filter in the app, removing any outliers. Additionally, in the app there is a "Cognitive Score." The app interprets all the data and based off of an algorithm, it can give you a score out of 100 as well as some insights.

I made this project because I like biomedical devices and especially ones that can track and analyze your sympathetic, parasympathetic, and autonomic nervous system. Many smart wearables like smart rings have the PPG (heart rate sensor) but they don't have the crucial GSR sensor. 

To use this project, when everything is assembled and firmware is updated, the user opens the app, connects to the device and lets the tracking run throughout the day. Or, they can do short sessions of tracking. 

<img width="489" height="422" alt="Screenshot 2026-02-07 at 7 46 40 PM" src="https://github.com/user-attachments/assets/4cca5557-c2f1-45cb-8358-3a0cf60e1aaf" />
<br>
I could not get the flex PCB to actually flex or bend into a circular shape, so the green represents the flex PCB with proper dimensions. But I also included the important components (PPG sensor) in the respective location.
<img width="864" height="612" alt="Screenshot 2026-01-24 at 11 11 21 PM" src="https://github.com/user-attachments/assets/7879b4dc-c5ab-473a-8045-d4015adbab2c" />
<img width="436" height="368" alt="Screenshot 2026-02-07 at 9 29 02 PM" src="https://github.com/user-attachments/assets/2e2c0f2e-898e-42cc-9a81-37765b89f813" />

<img width="1084" height="406" alt="Screenshot 2026-01-24 at 11 11 37 PM" src="https://github.com/user-attachments/assets/25264fb5-25fe-4dbe-ba53-a23d3237ebc3" />
<img width="1165" height="605" alt="Screenshot 2026-01-26 at 3 05 09 PM" src="https://github.com/user-attachments/assets/06eb6955-13c5-45c2-9da7-03a3fe81a2aa" />
<img width="782" height="559" alt="Screenshot 2026-01-26 at 4 04 11 PM" src="https://github.com/user-attachments/assets/2f345a7a-1446-4251-8ff1-f1bb022085d0" />
