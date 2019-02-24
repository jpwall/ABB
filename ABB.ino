#include <Adafruit_Sensor.h>
#include <Adafruit_LIS3DH.h>

int leftButton = 0;
int rightButton = 1;
int leftButtonState = 0;
int lastLeftButtonState = 0;
int rightButtonState = 0;
int lastRightButtonState = 0;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

int leftLED = 2;
int rightLED = 3;
int brakeLED = 4;
int leftBlinker = 0;
int rightBlinker = 0;
int brakeState = 0;
unsigned long blinkTime = 350;
unsigned long lastBlinkTime = 0;

Adafruit_LIS3DH accel = Adafruit_LIS3DH();
sensors_event_t accelEvent;

void setup() {
    pinMode(leftButton, INPUT_PULLUP);
    pinMode(rightButton, INPUT_PULLUP);
    pinMode(leftLED, OUTPUT);
    pinMode(rightLED, OUTPUT);
    pinMode(brakeLED, OUTPUT);

    leftBlinker = HIGH;
    lastBlinkTime = millis();

    Serial.begin(9600);
}

void loop() {
    //accel.getEvent(&accelEvent);
    // accelEvent.acceleration.x, etc

    leftButtonState = debounce(leftButton, &leftButtonState, &lastLeftButtonState);
    rightButtonState = debounce(rightButton, &rightButtonState, &lastRightButtonState);
    
    if(leftButtonState == HIGH){
        leftBlinker = !leftBlinker;
        if(leftBlinker == HIGH && rightBlinker == HIGH){
            rightBlinker = LOW;
        }
    }
    if(rightButtonState == HIGH){
        rightBlinker = !rightBlinker;
        if(rightBlinker == HIGH && leftBlinker == HIGH){
            leftBlinker = LOW;
        }
    }
    
    if(leftBlinker == HIGH){
        flashLED(leftLED);
    } else if(rightBlinker == HIGH){
        flashLED(rightLED);
    }

    digitalWrite(brakeLED, brakeState);
}

void flashLED(int LEDPin){
    unsigned long diff = millis() - lastBlinkTime;
    if(diff <= blinkTime){
        digitalWrite(LEDPin, HIGH);
    } else if(diff > blinkTime && diff < 2 * blinkTime){
        digitalWrite(LEDPin, LOW);
    } else if(diff >= 2 * blinkTime){
        lastBlinkTime = millis();
    }
}

void debounce(int buttonPin, int* buttonState, int* lastButtonState){
    int reading = digitalRead(buttonPin);

    if (reading != *lastButtonState) {
        // reset the debouncing timer
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        // whatever the reading is at, it's been there for longer than the debounce
        // delay, so take it as the actual current state:

        // if the button state has changed:
        if (reading != *buttonState) {
            *buttonState = reading;
        }
    }
    *lastButtonState = reading;
}

