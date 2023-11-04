// This file is only fot testing bits of code

#include <Arduino.h>
#include <rdm6300.h>

const int RDM6300_RX_PIN = 14;
const int debugButtonPin = 5;

int tagData = 0;
int buffer[5] = {0}; // Array to act as a buffer to store 5 values
int currentIndex = 0;         // Index to keep track of the current buffer position
int masterKey = 0;
bool debugMode = false;

Rdm6300 rdm6300;

void setup()
{
    Serial.begin(115200);

    rdm6300.begin(RDM6300_RX_PIN);
    rdm6300.set_tag_timeout(300);
    pinMode(debugButtonPin, INPUT_PULLUP);
    Serial.println("");
    Serial.println("Setuo Complete");
}

void loop()
{
    if (digitalRead(debugButtonPin) == LOW or debugMode == true)
    {
        Serial.println("Entering Debug mode");

        showData();

        Serial.println("Waiting");
        delay(10000);

        clearData();
        showData();

        Serial.println("returning to normal mode");
        delay(10000);
        debugMode = false;
    }

    delay(2000);
    Serial.println("Entering Loop");
    Serial.println("Waiting for card...");
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

        tagData = rdm6300.get_tag_id();
        delay(10);

        // Store the value in the buffer
        buffer[currentIndex] = tagData;
        currentIndex = (currentIndex + 1) % 5; // Wrap around to the start when full

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
            Serial.println("All buffer values are the same.");
            Serial.println("Stored Value: " + String(buffer[0]));
            Serial.println(tagData);
            Serial.println(tagData, HEX);
            Serial.print("After the loop:");
            Serial.println(i);
            Serial.println("Writing to masterkey");
            masterKey = tagData;
            clearData();
            break;
        }
    }
    Serial.println("im out the loop");
    delay(5000);
}

void clearData()
{
    Serial.println("Clearing Variables");
    tagData = 0;
    Serial.println("Variable: tagData =");
    Serial.println(tagData);
    Serial.println(tagData, HEX);
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
}