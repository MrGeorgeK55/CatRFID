# CatRFID


im not a good programmer so i dont know if this code is the best way to do this but it works and took me a lot of time to make it work so i hope you like it
im a fanatic of creating unnecessarily complicated solutions to simple problems so i added a lot of features that are not necesary but i think are cool

you are free to use it and modify it as you want but please at least leave a thanks in the comments,
honestly i dont care if you dont do it but it would be nice to read it or at least 
give the thanks to my cat Eliott which is the reason to build this project.

 ![20231015_193042](https://github.com/MrGeorgeK55/CatRFID/assets/103085400/0b417380-127d-45ae-9fa8-ae1a74eb8e03)


## Components
+ a Proximity sensor (Hcsr 501 Pir Sensor Module)  
+ two magnetic sensors (Digital Bipolar Magnetic Hall-effect Sensor SS41)  
+ a registration button  
+ a manual opening button  
+ an rfid card reader (RDM6300 this is important because allows external antenna) [Library](https://github.com/arduino12/rdm6300)
+ two servo motors (any would work but i used MG90S)  
+ an arduino of your choice but make sure is capable of PMW signaling to the servos and enough pins for all the sensors (i used a WeMos D1 mini with ESP8266EX chip)  

## Explanation:

they are 3 buttons, the debug button, the register button and the manual open button

the manual open button opens the door for 5 seconds and then closes it

the registration button wich should register a new card, it also detects the length of the key to save it in the EEPROM memory
this ensures that many different RFID cards can be registered
this is to avoid having to connect the arduino to the computer every time you want to register a card

the debug button shows all the variables and the EEPROM memory in the serial monitor  
  
### Begining
  
when you power the arduino it reads the EEPROM memory to get the registered card, calibrates the hall sensors and set the servos to closed position
this is the setup along with all the variables

then it enters the loop

the Proximity sensor detects something and activates the card reader
this is to avoid having to activate the card reader all the time

the card reader reads the card and compares it with the registered card
the reader has a timer so if it does not detect any card it stops reading and takes a cooldown also works with the register of new cards

if the card is correct it unlocks the door moving the servo motors to the open position 
This also can be achieved with a solenoid but they use a lot of power and diferent voltages so i prefered to use servo motors
if the card is not correct it does nothing and takes a cooldown
this is to avoid that any incorrect data can open the door

if the cat decides to enter the house the magnetic sensor detects the door opening and keeps the door unlocked until the flap door isnt moving in the closed position for 3 seconds
this is to avoid that the door closes while the cat is entering the house

if the cat unlocks the door but doesnt enter the house, the door locks automatically after a 30 seconds
also the sensors know if the door is opening to outside or inside but i dont added use for that feature yet

aditionally the RFID Sensor are RDM6300 wich have a little problem, when i called rdm6300.get_tag_id(); it returns the key value but if its called again or the next 5 times it keeps retuning the same value 
even if the card is removed so i added a buffer to store the last 5 values and compare them to see if they are the same, if they are the same it returns the value and fixed the problem

also i added a debug mode to show all the variables, this can are not necesary but exist for debuging purposes

a code for a transistor exist wich i tried to use in the hall sensors because the arduino wouldnt flash if was recieving voltages in the GPIO pins but it didnt work so i have a switch to turn off manually the hall sensors when im flashing the arduino  
  
  
## Aditional notes:
  
to avoid the servo motors getting stuck or shaking the position has an ofset of 20 degrees
you should build your custom RFID antenna and i barely know how i do it so dont ask me how
if you have a problems with the eeprom memory (like i did) in any ESP32 make sure you are using the right library or the exact board when you are flashing the code
my problem was that i wasnt defining the correct amount of flash memory for the board so the eeprom memory wasnt working even if the code was working fine
also remember the eeprom has a limited amount of writes so be careful with that
  
  
## Things to do:
  
+ feed my cat ✔️
+ add a buzzer to make a sound when the door is unlocked
+ use the Builit in led to show the status of the door or at least "im working"
+ add more memory to save multiple cards
+ add wifi funcionallity to send notifications to the phone (i used a Telegram bot in other project so maybe ill use it here too)
+ give usage to the inside and outside sensors (for now only works as door activity sensor)
+ build a unrelated to this project, automatic feeder for my cat
+ get someone as a human sacrifice as my cat demands

