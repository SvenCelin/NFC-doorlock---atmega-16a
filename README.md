# NFC-doorlock---atmega-16a


This project is made for Atmega 16a and Atmega 32. <br />
Project is written in C and in Atmel Studio IDE <br />

## For replicating this project you will need:

Component | Function
------------ | -------------
[ATMega16a](images/image001.png) | Main microcontroler
[LCM1602C LCD](images/image003.png) | Shows the states of the program
[MFRC522](images/image009.png) | Reads and writes to RFID card
[RFID Cards](images/image013.png) | Used to varify the user
[Breadboard](images/image015.png) | Used to test all the components

## Final design:
![Image description](images/image019.jpg)

## All the states of the system:
State | Image
------------ | -------------
Main screen | <img src="./images/image026.jpg" alt="image026.jpg" width="400"/>
No stored cards message | <img src="./images/image029.jpg" alt="image019.jpg" width="400"/>
Scan card menu | <img src="./images/image031.jpg" alt="image019.jpg" width="400"/>
Delete card menu | <img src="./images/image033.jpg" alt="image019.jpg" width="400"/>
Card deleted message | <img src="./images/image035.jpg" alt="image019.jpg" width="400"/>
Unknown card message | <img src="./images/image037.jpg" alt="image019.jpg" width="400"/>
Add card menu | <img src="./images/image039.jpg" alt="image019.jpg" width="400"/>
Card saved message | <img src="./images/image041.jpg" alt="image019.jpg" width="400"/>
Memory flushed | <img src="./images/image043.jpg" alt="image019.jpg" width="400"/>

