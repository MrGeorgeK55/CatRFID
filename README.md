# CatRFID


I'm not a good programmer, so I don't know if this code is the best way to do this, but it works and took me a lot of time to make it work, so I hope you like it.
I'm a fan of creating unnecessarily complicated solutions to simple problems, so I added a lot of features that are not necessary but that I think are cool.

You are free to use it and modify it as you want, but please at least leave a thanks in the comments.
honestly, I dont care if you don't do it, but it would be nice to read it, or at least
Give thanks to my cat Eliott, which is the reason to build this project.

 ![20231015_193042](https://github.com/MrGeorgeK55/CatRFID/assets/103085400/0b417380-127d-45ae-9fa8-ae1a74eb8e03)


## Components
+ a proximity sensor (Hcsr 501 Pir Sensor Module)
+ two magnetic sensors (Digital Bipolar Magnetic Hall-effect Sensor SS41)  
+ a registration button  
+ a manual opening button  
+ an RFID card reader (RDM6300; this is important because it allows an external antenna). [Library](https://github.com/arduino12/rdm6300)
+ two servo motors (any would work, but I used the MG90S)  
+ an Arduino of your choice, but make sure it is capable of PMW signaling to the servos and has enough pins for all the sensors (I used a WeMos D1 mini with an ESP8266EX chip)  

## Explanation:

There are 3 buttons: the debug button, the register button, and the manual open button.

The manual open button opens the door for 5 seconds and then closes it.

The registration button, which should register a new card, also detects the length of the key to save it in the EEPROM memory.
This ensures that many different RFID cards can be registered.
This is to avoid having to connect the Arduino to the computer every time you want to register a card.

The debug button shows all the variables and the EEPROM memory on the serial monitor.
  
### Begining
  
When you power the Arduino, it reads the EEPROM memory to get the registered card, calibrates the hall sensors, and sets the servos to the closed position.
This is the setup, along with all the variables.

then it enters the loop.

The proximity sensor detects something and activates the card reader.
This is to avoid having to activate the card reader all the time.

The card reader reads the card and compares it with the registered card.
The reader has a timer, so if it does not detect any cards, it stops reading and takes a cooldown. It also works with the registration of new cards.

If the card is correct, it unlocks the door, moving the servo motors to the open position.
This can also be achieved with a solenoid, but they use a lot of power and different voltages, so I prefer to use servo motors.
If the card is not correct, it does nothing and takes a cooldown.
This is to avoid any incorrect data could open the door.

If the cat decides to enter the house, the magnetic sensor detects the door opening and keeps the door unlocked until the flap door isn't moving in the closed position for 3 seconds.
This is to avoid the door closes while the cat is entering the house.

If the cat unlocks the door but does not enter the house, the door locks automatically after 30 seconds.
Also, the sensors know if the door is opening to the outside or inside, but I have not added use for that feature yet.

Additionally, the RFID reader is RDM6300, which has a little problem, when I called rdm6300.get_tag_id(); it returns the key value, but if it is called again or the next five times, it keeps returning the same value.
Even if the card is removed, so I added a buffer to store the last 5 values and compare them to see if they are the same. If they are the same, it returns the value and fixes the problem.

Also, I added a debug mode to show all the variables, which are not necessary but exist for debugging purposes.

A code exists for a transistor that I tried to use in the hall sensors because the Arduino wouldn't flash if it was receiving voltages in the GPIO pins, but it didn't work, so I have a switch to manually turn off the hall sensors when I flash the Arduino.
  
  
## Additional notes:
  
To avoid the servo motors getting stuck or shaking, the position has an offset of 20 degrees.
You should build your custom RFID antenna, and I barely know how I do it, so don't ask me how.
If you have problems with the eeprom memory (like I did) in any ESP32, make sure you are using the right library or the exact board when you are flashing the code.
My problem was that I wasn't defining the correct amount of flash memory for the board, so the eeprom memory wasn't working even if the code was working fine.
Also, remember that the eeprom has a limited number of writes, so be careful with that.
  
  
## Things to do:
  
+ Feed my cat ✔️
+ Add a buzzer to make a sound when the door is unlocked
+ Use the built-in LED to show the status of the door, or at least "I'm working."
+ Add more memory to save multiple cards.
+ Add wifi functionality to send notifications to the phone (I used a Telegram bot in another project, so maybe I will use it here too).
+ Give usage to the inside and outside sensors (for now only works as door activity sensor).
+ Build an unrelated automatic feeder for my cat
+ Get someone as a human sacrifice as my cat demands.
