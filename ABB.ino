#include <Bounce2.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LIS3DH.h>

#define LEFT_BUTTON 5
#define RIGHT_BUTTON 6
#define LEFT_LED 2
#define RIGHT_LED 3
#define BRAKE_LED 4
#define ACCEL_RATE 500
#define ACCEL_READINGS 20

Bounce leftButton = Bounce();
Bounce rightButton = Bounce();

int leftBlinker = 0;
int rightBlinker = 0;
int brakeState = 0;
unsigned long blinkTime = 350;
unsigned long lastBlinkTime = 0;
unsigned long lastAccelTime = 0;
float readings[ACCEL_READINGS];
float normal = 0.0f;

Adafruit_LIS3DH accel = Adafruit_LIS3DH();
sensors_event_t accelEvent;

void setup() {
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
        // Add error/status led or something?
        while (1);
    }
    accel.setRange(LIS3DH_RANGE_2_G);
}

void loop() {
    // Update accelerometer at certain rate
    if(millis() - lastAccelTime > ACCEL_RATE){
        accel.getEvent(&accelEvent);
        lastAccelTime = millis();
        addReading(accelEvent.acceleration.y);
    }
    // Update normal based on readings buffer
    updateNormal();
    
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

    // Update brake state
    brakeState = HIGH;
    for(int i = 0; i < 2; i++){
        if(readings[i] > normal - 0.1){
            brakeState = LOW;
        }
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

// Adds a new accelerometer reading to the readings buffer
void addReading(float reading){
    for(int i = 1; i < ACCEL_READINGS; i++){
        readings[i] = readings[i - 1];
    }
    readings[0] = reading;
}

// Checks for a new normal acceleration value
// Only updates normal once most recent buffer is stable
void updateNormal(){
    float min = readings[0];
    float max = readings[0];
    float avg = readings[0];
    for(int i = 1; i < ACCEL_READINGS; i++){
        if(readings[i] < min){
            min = readings[i];
        } else if (readings[i] > max){
            max = readings[i];
        }
        avg += readings[i];
    }
    avg /= ACCEL_READINGS;
    if(abs(max - min) < 0.1){
        normal = avg;
    }
}

