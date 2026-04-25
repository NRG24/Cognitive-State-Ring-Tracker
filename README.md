This is a sensor packed device that features a GSR sensor and max30102 sensor that measure skin electrical conductivity (arousal), temperature, spo2, heart rate, HRV, and an accelerometer for filtering. The point of this is to build a tracker that isn't focused on fitness, but cognitive health tied to the nervous system. These sensors will be packed into a small flex PCB in a ring shape.. All of the circuitry has been tested in LT-Spice to ensure electrical correctness. The key point of this project is to have this ring focused on mental and cognitive health rather than fitness and physical health. So, the sensors send data to the Raytac nRF52832 module and the Raytac sends it to a mobile device via BLE. The data goes through a filter in the app, removing any outliers. Additionally, in the app there is a "Cognitive Score." The app interprets all the data and based off of an algorithm, it can give you a score out of 100 as well as some insights.

I made this project because I like biomedical devices and especially ones that can track and analyze your sympathetic, parasympathetic, and autonomic nervous system. Many smart wearables like smart rings have the PPG (heart rate sensor) but they don't have the crucial GSR sensor. 

To use this project, when everything is assembled and firmware is updated, the user opens the app, connects to the device and lets the tracking run throughout the day. Or, they can do short sessions of tracking. 

NOTES ABOUT SHIPPING:
JLCPCB only does Global Direct for orders under $99 to Canada. The next cheapest is DHL $17.

<img width="489" height="422" alt="Screenshot 2026-02-07 at 7 46 40 PM" src="https://github.com/user-attachments/assets/4cca5557-c2f1-45cb-8358-3a0cf60e1aaf" />

<img width="436" height="368" alt="Screenshot 2026-02-07 at 9 29 02 PM" src="https://github.com/user-attachments/assets/2e2c0f2e-898e-42cc-9a81-37765b89f813" />
<br>
I could not get the flex PCB to actually flex or bend into a circular shape, so the green represents the flex PCB with proper dimensions. But I also included the important components (PPG sensor) in the respective location.
<img width="1077" height="370" alt="Screenshot 2026-03-21 at 9 27 09 PM" src="https://github.com/user-attachments/assets/76b5d239-362e-46cb-b721-c6f8a9353247" />
<img width="718" height="494" alt="Screenshot 2026-02-11 at 9 21 46 PM" src="https://github.com/user-attachments/assets/ed82f334-0ea8-4d68-94f6-a0611016e22b" />
<img width="1101" height="417" alt="Screenshot 2026-03-21 at 9 28 39 PM" src="https://github.com/user-attachments/assets/bfdef878-dc1f-4e6c-975c-ed55c9a32da5" />
<img width="1163" height="555" alt="Screenshot 2026-03-21 at 9 30 06 PM" src="https://github.com/user-attachments/assets/126d4b80-af4b-493e-b5fa-aa013f26baca" />

