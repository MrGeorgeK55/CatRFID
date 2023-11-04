#include <Arduino.h>

const int HALL_SENSOR_1_PIN = 13;
const int HALL_SENSOR_2_PIN = 15;
const int transistor = 16;

int hallS1State = 0;
int hallS2State = 0;
int hallS1InitState = 0;
int hallS2InitState = 0;

int doorTimeToClose = 300;
int doorIdleTime = 3000;

const int debugButtonPin = 3;

int doorIncrement = 0;
bool doorStatus = false; // false = closed, true = open
bool doorTrigger = false;
void setup()
{
    Serial.begin(115200);
    Serial.println("");
    Serial.println("Setup Started");
    pinMode(HALL_SENSOR_1_PIN, INPUT);
    pinMode(HALL_SENSOR_2_PIN, INPUT);

    delay(500);

    pinMode(transistor, OUTPUT);
    Serial.println("Initializating Sensors");
    delay(5000);
    digitalWrite(transistor, HIGH);
    delay(3000);
    hallS1InitState = digitalRead(HALL_SENSOR_1_PIN);
    hallS2InitState = digitalRead(HALL_SENSOR_2_PIN);

    Serial.println("Hall Sensor 1");
    Serial.println(hallS1InitState);
    Serial.println("Hall Sensor 2");
    Serial.println(hallS2InitState);

    Serial.println("Setup Finished");
}

void loop()
{
    if (digitalRead(debugButtonPin) == HIGH)
    {
        Serial.println("Entering Debug mode");

        showData();

        Serial.println("Waiting");
        delay(10000);

        clearData();
        showData();

        Serial.println("returning to normal mode");
        delay(10000);
    }

    doorTrigger = doorActivity(doorIdleTime);
    if (doorTrigger == true)
    {
        Serial.println("Activity Detected");
        while (1)
        {
            int hallS1State = digitalRead(HALL_SENSOR_1_PIN);
            int hallS2State = digitalRead(HALL_SENSOR_2_PIN);

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
            Serial.println(doorIncrement);
        }
    }
    else
    {
        Serial.println("No Activity, closing door");
        doorStatus = true;
    }

    if (doorStatus == true)
    {
        doorStatus = false;
        doorTrigger = false;
        Serial.println("Door Closed");
    }
}

bool doorActivity(int doorValue)
{
    Serial.println("Checking for activity");
    for (int i = 0; i < doorValue; i++)
    {
        int hallS1State = digitalRead(HALL_SENSOR_1_PIN);
        int hallS2State = digitalRead(HALL_SENSOR_2_PIN);

        if (hallS1State != hallS1InitState && hallS2State == hallS2InitState)

        {
            Serial.println("El Gato Entro");
            Serial.println(hallS1State);
            Serial.println(hallS2State);
            return true;
        }
        if (hallS2State != hallS2InitState && hallS1State == hallS1InitState)

        {
            Serial.println("El Gato Salio");
            Serial.println(hallS1State);
            Serial.println(hallS2State);
            return true;
        }
        delay(10);
    }
    return false;
}

void clearData()
{
    Serial.println("Clearing Variables");


    doorTimeToClose = 3000;
    doorIdleTime = 30000;
    doorIncrement = 0;
    doorStatus = false; // false = closed, true = open
    doorTrigger = false;
}

void showData()
{
    Serial.println("Showing Variables");
    Serial.println("Door Time to Close");
    Serial.println(doorTimeToClose);
    Serial.println("Door Idle Time");
    Serial.println(doorIdleTime);
    Serial.println("Door Increment");
    Serial.println(doorIncrement);
    Serial.println("Door Status");
    Serial.println(doorStatus);
    Serial.println("Door Trigger");
    Serial.println(doorTrigger);
    Serial.println("Hall Sensor 1");
    Serial.println(hallS1State);
    Serial.println("Hall Sensor 2");
    Serial.println(hallS2State);
    Serial.println("Hall Sensor 1 Init State");
    Serial.println(hallS1InitState);
    Serial.println("Hall Sensor 2 Init State");
    Serial.println(hallS2InitState);
}