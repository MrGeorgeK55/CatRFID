//Credits:
//Coding, debugging and testing: George
//Sleeping, Constant demanding food and petting, moral support: Eliott (my cat)

#include <Arduino.h>
#include <Servo.h>   //Servo library
#include <rdm6300.h> //RFID reader library
#include <EEPROM.h>  //EEPROM library

// Pins

// RFID reader RX pin
const int RDM6300_RX_PIN = 14;

// servo pins
const int servo1Pin = 5;
const int servo2Pin = 4;

// Sensor pins
const int proximitySensorPin = 12;
const int hallSensorPin1 = 13;
const int hallSensorPin2 = 15;
const int transistor = 16;

// Button pins
const int debugButtonPin = 0;     
const int manualOpenButtonPin = 2; 
const int registerButtonPin = 3;   


// Variables

// Proximity sensor variables
int proximityValue = 0;
bool motionState = false;

// Hall sensor variables
int hallS1State = 0;
int hallS2State = 0;
int hallS1InitState = 0;
int hallS2InitState = 0;

// Variables for servo angles
int servoOpenAngle = 110;
int servoClosedAngle = 20;

// Door variables
int doorCooldown = 100;
int doorIdleTime = 3000;
int doorTimeToClose = 300;
int doorIncrement = 0;
bool doorStatus = false; // false = closed, true = open
bool doorTrigger = false;

// Other variables
int tagRead = 0;
int tagData = 0;
int dataRead = 0;
int dataReturned = 0;
int masterKey = 1;
bool cardDetected = false;
bool registered = false;
int buffer[5] = {0};  // Array to act as a buffer to store 5 values
int currentIndex = 0; // Index to keep track of the current buffer position

bool debugMode = false;

///////////////////////Testing Area///////////////////////////

///////////////////////Testing Area///////////////////////////

Servo servo1;
Servo servo2;
Rdm6300 rdm6300;

void setup()
{
    // Initialize Serial communication for debugging (optional)
    Serial.begin(115200);
    Serial.println("");
    Serial.println("Starting Setup");

    // Initialize the RFID reader
    rdm6300.begin(RDM6300_RX_PIN);
    rdm6300.set_tag_timeout(300);

    // Initialize the EEPROM
    EEPROM.begin(64);

    // Read the master key from EEPROM
    masterKey = readFromEEPROM(0);
    Serial.print("Master key: 0x");
    Serial.println(masterKey, HEX);

    // Initialize the proximity sensor
    pinMode(proximitySensorPin, INPUT);

    // Attach servo objects to their corresponding pins
    servo1.attach(servo1Pin);
    servo2.attach(servo2Pin);

    // Set initial positions of the servo motors (optional)
    servo1.write(servoClosedAngle); // Closed position
    servo2.write(servoClosedAngle); // Closed position

    // Initialize button pins as inputs
    pinMode(debugButtonPin, INPUT_PULLUP);
    pinMode(manualOpenButtonPin, INPUT_PULLUP);
    pinMode(registerButtonPin, INPUT_PULLUP);

    Serial.println("Calibrating Hall Sensors");

    // Initialize the transistor
    pinMode(transistor, OUTPUT);
    delay(3000);
    digitalWrite(transistor, HIGH);

    // Initialize the hall sensors
    pinMode(hallSensorPin1, INPUT);
    pinMode(hallSensorPin2, INPUT);
    delay(200);
    hallS1InitState = digitalRead(hallSensorPin1);
    hallS2InitState = digitalRead(hallSensorPin2);

    Serial.println("Hall Sensor 1");
    Serial.println(hallS1InitState);
    Serial.println("Hall Sensor 2");
    Serial.println(hallS2InitState);

    /// Testing Area///

    /// Testing Area///

    Serial.println("Setup Complete");
}

void loop()
{


    if (digitalRead(debugButtonPin) == LOW || debugMode == true)
    {
        Serial.println("Entering Debug mode");

        showData();

        Serial.println("Waiting");
        delay(10000);

        clearData();
        restartValues();
        showData();

        Serial.println("returning to normal mode");
        delay(10000);
        debugMode = false;
    }

    if (digitalRead(registerButtonPin) == LOW)
    {
        Serial.println("Register button pressed");
        Serial.println("Registering new RFID card...");
        registerNewRFIDCard();
        delay(500);
        if (registered == true)
        {
            registered = false;
            Serial.println("Registered suscessfully");
        }
        else
        {
            Serial.println("Error registering card");
        }
    }

    if (digitalRead(manualOpenButtonPin) == LOW)
    {
        // Button is pressed, manually open the servos
        Serial.println("Manually opening servos...");
        servo1.write(servoOpenAngle);
        servo2.write(servoOpenAngle);
        delay(5000); // Open servos for 5 seconds
        servo1.write(servoClosedAngle);
        servo2.write(servoClosedAngle);
        delay(500); // Debounce delay
        Serial.println("Manually closing servos...");
    }

    proximityValue = digitalRead(proximitySensorPin);

    // Check the proximity sensor
    if (proximityValue == HIGH)
    {

        // Change the motion state to true (motion detected):
        if (motionState == false)
        {
            Serial.println("Motion detected!");
            motionState = true;
        }
    }

    if (motionState == true)
    {
        motionState = false;

        // Proximity sensor detected something, initialize RFID card reading
        Serial.println("Proximity sensor detected something, initializing RFID card reading...");
        tagData = 0;
        tagData = readRFIDCard();
        if (tagData == masterKey && masterKey != 0)
        {
            // RFID card matched, open servos
            Serial.println("Key Matched Oppening Door");
            servo1.write(servoOpenAngle);
            servo2.write(servoOpenAngle);

            doorTrigger = doorActivity(doorIdleTime);
            if (doorTrigger == true)
            {
                Serial.println("Door Opened");
                while (1)
                {
                    int hallS1State = digitalRead(hallSensorPin1);
                    int hallS2State = digitalRead(hallSensorPin2);

                    if (hallS1State != hallS1InitState || hallS2State != hallS2InitState)
                    {
                        doorIncrement = 0;
                    }

                    if (hallS1State == hallS1InitState && hallS2State == hallS2InitState)
                    {
                        doorIncrement++;
                    }
                    if (doorIncrement == doorTimeToClose)
                    {
                        Serial.println("Door ready to close");
                        doorStatus = true;
                        break;
                    }
                    delay(10);
                }
            }
            else
            {
                Serial.println("No Activity, closing door");
                doorStatus = true;
            }

            if (doorStatus == true)
            {
                Serial.println("Door Closed");
                doorStatus = false;
                doorTrigger = false;
                servo1.write(servoClosedAngle);
                servo2.write(servoClosedAngle);
            }
        }
        else
        {
            // RFID card didn't match, put a cooldown of 30 seconds
            delay(doorCooldown);
            Serial.println("Key not matched");
        }
    }
    else
    {
        // RFID card didn't match, put a cooldown of 30 seconds
        delay(doorCooldown);
        Serial.println("No movement detected, waiting for movement");
    }
}

void registerNewRFIDCard()
{
    clearData();
    tagData = readRFIDCard();
    if (tagData == 0)
    {
        Serial.println("No RFID card detected.");
        return;
    }
    writeToEEPROM(0, tagData);
    dataRead = readFromEEPROM(0);
    if (dataRead == tagData)
    {
        Serial.println("Data written to EEPROM successfully!");
        Serial.println("Data read from EEPROM: ");
        Serial.println(dataRead);
        Serial.println(dataRead, HEX);
        masterKey = dataRead;
        registered = true;
    }
    else
    {
        Serial.println("Data read from EEPROM does not match the data written!");
        Serial.println("Data read from EEPROM: ");
        Serial.println(dataRead);
        Serial.println(dataRead, HEX);
    }
}

int readRFIDCard()
{
    Serial.println("Entering the loop");
    rdm6300.listen();
    for (int i = 0; i < 1000; i++)
    {
        if (digitalRead(debugButtonPin) == LOW)
        {
            Serial.println("Debug button pressed");
            Serial.println("interrumpting loop");
            debugMode = true;
            break;
        }
        tagRead = rdm6300.get_tag_id();
        delay(10);

        // Store the value in the buffer
        buffer[currentIndex] = tagRead;
        currentIndex = (currentIndex + 1) % 5; 

        // Check if all values in the buffer are the same
        bool allEqual = true;
        for (int j = 1; j < 5; j++)
        {
            if (buffer[j] != buffer[0])
            {
                allEqual = false;
                break;
            }
        }

        if (allEqual && i > 100 && buffer[0] != 0)
        {
            Serial.println("RFID card detected.");
            Serial.println("Stored Value: " + String(buffer[0]));
            Serial.println(tagRead);
            Serial.println(tagRead, HEX);
            Serial.print("After the loop:");
            Serial.println(i);
            cardDetected = true;

            if (cardDetected == true)
            {
                cardDetected = false;
                dataReturned = tagRead;
                return dataReturned;
            }
        }
    }
    if (cardDetected == false)
    {
        Serial.println("RFID card not detected.");
        dataReturned = 0;
        return dataReturned;
    }
}

void writeToEEPROM(int addrOffset, int valueToConvert)
{
    if (valueToConvert == 0)
    {
        Serial.println("No value to write to EEPROM");
        return;
    }
    Serial.print("Writing value ");
    Serial.print(valueToConvert);
    Serial.print(" to EEPROM at address ");
    Serial.println(addrOffset);

    // Convert the integer value to hexadecimal
    String hexValue = String(valueToConvert, HEX);

    Serial.print("Value Converted to: ");
    Serial.println(hexValue);
    // Write each hexadecimal character to EEPROM
    for (int i = 0; i < hexValue.length(); i++)
    {
        EEPROM.write(addrOffset + i, hexValue[i]);
    }

    // Add a null terminator to mark the end of the value
    EEPROM.write(addrOffset + hexValue.length(), '\0');
    // Commit the changes to EEPROM
    EEPROM.commit();
    delay(5000);
    Serial.println("Ending write to EEPROM");
}

int readFromEEPROM(int address)
{

    Serial.print("Reading value from EEPROM at address ");
    delay(100);
    Serial.println(address);

    String hexValue = "";

    // Read characters from EEPROM until a null terminator is encountered
    char c = EEPROM.read(address);
    while (c != '\0')
    {
        hexValue += c;
        address++;
        c = EEPROM.read(address);
    }

    Serial.println("Value read from EEPROM: ");
    Serial.println(hexValue);

    // Convert the hexadecimal string back to an integer
    int value = strtol(hexValue.c_str(), NULL, 16);

    Serial.print("Value converted to: ");
    Serial.println(value);
    return value;
}

bool doorActivity(int doorValue)
{
    Serial.println("Checking for activity");
    for (int i = 0; i < doorValue; i++)
    {
        int hallS1State = digitalRead(hallSensorPin1);
        int hallS2State = digitalRead(hallSensorPin2);

        if (hallS1State != hallS1InitState && hallS2State == hallS2InitState)

        {
            Serial.println("The Cat Entered");
            Serial.println(hallS1State);
            Serial.println(hallS2State);
            return true;
        }
        if (hallS2State != hallS2InitState && hallS1State == hallS1InitState)

        {
            Serial.println("The Cat Exited");
            Serial.println(hallS1State);
            Serial.println(hallS2State);
            return true;
        }
        delay(10);
    }
    return false;
}
void restartValues()
{
    Serial.println("Restarting Values");
    doorCooldown = 100;
    doorIdleTime = 3000;
    doorTimeToClose = 300;
    doorIncrement = 0;
    doorStatus = false; // false = closed, true = open
    doorTrigger = false;
}
void clearData()
{
    Serial.println("Clearing Variables");
    tagData = 0;
    Serial.println("Variable: tagData =");
    Serial.println(tagData);
    Serial.println(tagData, HEX);
    dataRead = 0;
    Serial.println("Variable: dataRead =");
    Serial.println(dataRead);
    Serial.println(dataRead, HEX);
    Serial.println("Clearing buffer");
    for (int i = 0; i < 5; i++)
    {
        buffer[i] = 0;
    }
    currentIndex = 0;
    Serial.println("Buffer Cleared");
}

void showData()
{
    Serial.println("Showing Variables");
    Serial.println("Variable: tagData =");
    Serial.println(tagData);
    Serial.println(tagData, HEX);
    Serial.println("Variable: masterkey =");
    Serial.println(masterKey);
    Serial.println(masterKey, HEX);
    Serial.println("Variable: buffer =");
    for (int i = 0; i < 5; i++)
    {
        Serial.println(buffer[i]);
    }
    Serial.println("Variable: currentIndex =");
    Serial.println(currentIndex);

    Serial.println("Variable: Door Time to Close");
    Serial.println(doorTimeToClose);
    Serial.println("Variable: Door Idle Time");
    Serial.println(doorIdleTime);
    Serial.println("Variable: Door Increment");
    Serial.println(doorIncrement);
    Serial.println("Variable: Door Status");
    Serial.println(doorStatus);
    Serial.println("Variable: Door Trigger");
    Serial.println(doorTrigger);
    Serial.println("Variable: Hall Sensor 1");
    Serial.println(hallS1State);
    Serial.println("Variable: Hall Sensor 2");
    Serial.println(hallS2State);
    Serial.println("Variable: Hall Sensor 1 Init State");
    Serial.println(hallS1InitState);
    Serial.println("Variable: Hall Sensor 2 Init State");
    Serial.println(hallS2InitState);
}

/*
//////////////////////////////  Detailed description  //////////////////////////////

Components
a Proximity sensor (Hcsr 501 Pir Sensor Module)
two magnetic sensors (Digital Bipolar Magnetic Hall-effect Sensor SS41)
a registration button 
a manual opening button
an rfid card reader (RDM6300 this is important because allows external antenna)
two servo motors (any would work but i used MG90S)
an arduino of your choice but make sure is capable of PMW signaling to the servos and enough pins for all the sensors i used a WeMos D1 mini with ESP8266EX chip
 


Explanation:

they are 3 buttons, the debug button, the register button and the manual open button

the manual open button opens the door for 5 seconds and then closes it

the registration button wich should register a new card, it also detects the length of the key to save it in the EEPROM memory
this ensures that many different RFID cards can be registered
this is to avoid having to connect the arduino to the computer every time you want to register a card

the debug button shows all the variables and the EEPROM memory in the serial monitor



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


aditional notes:

im not a good programmer so i dont know if this code is the best way to do this but it works and took me a lot of time to make it work so i hope you like it
im a fanatic of creating unnecessarily complicated solutions to simple problems so i added a lot of features that are not necesary but i think are cool

to avoid the servo motors getting stuck the position has an ofset of 20 degrees
you should build your custom RFID antenna and i barely know how i do it so dont ask me how
if you have a problems with the eeprom memory (like i did) in any ESP32 make sure you are using the right library or the exact board when you are flashing the code
my problem was that i wasnt defining the correct amount of flash memory for the board so the eeprom memory wasnt working even if the code was working fine
also remember the eeprom has a limited amount of writes so be careful with that

you are free to use it and modify it as you want but please at least leave a thanks in the comments 
honestly i dont care if you dont do it but it would be nice to read it or at least 
give the thanks to my cat Eliott which is the reason to build this project.

[insert cat picture here]


Things to do:

feed my cat --done
add a buzzer to make a sound when the door is unlocked
use the Builit in led to show the status of the door or at least "im working"
add more memory to save multiple cards
add wifi funcionallity to send notifications to the phone (i used a Telegram bot in other project so maybe ill use it here too)
give usage to the inside and outside sensors (for now only works as door activity sensor)
build a unrelated to this project, automatic feeder for my cat
get someone as a human sacrifice as my cat demands



*/


//////////////////////////////  Funcionamiento  //////////////////////////////
/*

    hay un sensor de proximidad
    dos sensores magneticos
    un boton de registro
    un boton de apertura manual
    un lector de tarjetas rfid
    dos servo motores

    el sensor de proximidad detecta cuando alguien se acerca a la puerta y activa el lector de tarjetas
    esto es para no tener que estar activando el lector de tarjetas todo el tiempo

    el boton de registro registra una nueva tarjeta
    esto es para evitar tener que conectar el arduino a la computadora cada vez que se quiera registrar una tarjeta

    el lector de tarjetas lee la tarjeta y la compara con la tarjeta registrada
    la tarjeta registrada esta guardada en la memoria del arduino

    si la tarjeta es la correcta abre la puerta
    si la tarjeta no es la correcta no hace nada
    esto es para evitar que alguno dato incorrecto pueda abrir la puerta

    el boton de apertura manual deberia abrir la puerta por un tiempo y luego cerrarla automaticamente
    esto en caso de que alguien no tenga tarjeta o la tarjeta no funcione

    el servo deberia cerrarse automaticamente despues de un tiempo
    pero si el sensor magnetico detecta que la puerta esta abierta no deberia cerrarse

    si el sensor magnetico esta activo durante algunos segundos deberia cerrar la puerta automaticamente antes de que termine el tiempo
    esto es para evitar que la puerta quede abierta por mucho tiempo



planes pendientes:
alojar mas memoria para multiples tarjetas






*/