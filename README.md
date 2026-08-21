# COLORs

Source code for the COLORs display piece, currently at Duke Kunshan University's Innovation Hub.
<img width="405" height="540" alt="IMG_0539" src="https://github.com/user-attachments/assets/cca15357-f9bd-4ec1-ab14-a947315b7fdc" />


# Setup
## Terminology
**Base (Plate)**: Giant circle pad
**Lamp**: The lamp
## Turning On the Device
1. Plug in the base plate, then turn on the lamp. DO NOT place the lamp near the plate at the time.
2. Wait approximately 30 seconds for base plate calibration before putting the lamp on the base.
3. Test the lamp-base functionality. If the lamp lights up, set up is completed.
## Troubleshooting/Frequent Sources of Error
- A **5.0V** USB-C cable must be used for both charging the lamp and powering the base plate.
	- Almost all USB to USB-C cables are 5.0V.
- The base plate must be continuously powered by the USB-C cable.
- Sometimes the baseplate will restart in the middle of calibration. Please allow an additional 30 seconds if the lamp is not working properly.
- Is the lamp set to the on position?
	- You can check by opening the bottom of the lamp and checking for power (red LED) on the microcontroller.
- Make sure no magnetic objects are near the area of operation.
- Physical pins on multiplexer do not match up with the pin in the code. See guide below.

<img width="129" height="341" alt="image" src="https://github.com/user-attachments/assets/1feb8831-8a89-45ae-bbcd-619827ed78e1" />

Please contact me if any issues arise.
