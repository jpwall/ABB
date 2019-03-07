#include <Bounce2.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LIS3DH.h>

#define LEFT_BUTTON 5
#define RIGHT_BUTTON 6
#define LEFT_LED 2
#define RIGHT_LED 3
#define BRAKE_LED 4

Bounce leftButton = Bounce();
Bounce rightButton = Bounce();

int leftBlinker = 0;
int rightBlinker = 0;
int brakeState = 0;
unsigned long blinkTime = 350;
unsigned long lastBlinkTime = 0;

Adafruit_LIS3DH accel = Adafruit_LIS3DH();
sensors_event_t accelEvent;

void setup() {
    // Start serial
    Serial.begin(9600);

    // Initialize pins
    pinMode(LEFT_BUTTON, INPUT_PULLUP);
    leftButton.attach(LEFT_BUTTON);
    leftButton.interval(5);
    pinMode(RIGHT_BUTTON, INPUT_PULLUP);
    rightButton.attach(RIGHT_BUTTON);
    rightButton.interval(5);
    pinMode(LEFT_LED, OUTPUT);
    pinMode(RIGHT_LED, OUTPUT);
    pinMode(BRAKE_LED, OUTPUT);

    // Find and initialize accelerometer
    if(!accel.begin(0x18)) {   // change this to 0x19 for alternative i2c address
        Serial.println("Couldnt start");
        while (1);
    }
    Serial.println("LIS3DH found!");
    accel.setRange(LIS3DH_RANGE_2_G);
}

void loop() {
    // Update accelerometer
    accel.getEvent(&accelEvent);
    Serial.println(accelEvent.acceleration.x);

    // Update button states
    leftButton.update();
    rightButton.update();

    // Update blinker states
    if(leftButton.fell()){
        leftBlinker = !leftBlinker;
        if(leftBlinker == HIGH && rightBlinker == HIGH){
            rightBlinker = LOW;
        }
    }
    if(rightButton.fell()){
        rightBlinker = !rightBlinker;
        if(rightBlinker == HIGH && leftBlinker == HIGH){
            leftBlinker = LOW;
        }   
    }

    // Flash blinker LEDs
    if(leftBlinker == HIGH){
        flashLED(LEFT_LED);
    } else {
        digitalWrite(LEFT_LED, LOW);
    }
    if(rightBlinker == HIGH){
        flashLED(RIGHT_LED);
    } else {
        digitalWrite(RIGHT_LED, LOW);
    }

    // Turn on/off brake LED
    digitalWrite(BRAKE_LED, brakeState);
}

// Flashes the given LED
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

