/*

    USECASE:
        - User needs to put 3 coins before the sensor and dispensers run.
        - If User put 3 coins, there is only 1 time mask can be dispense (servo rotates 460deg only once)
          and only 1 time alcohol dispense (servo motor pushes through alcohol dispense).

        - If User used both mask and alcohol dispenser once, the coin slot needs to
        filled again with 3 coins to run.
*/

#include <Servo.h>
#define coinReq 3

int coinReaderIsReady = 0;

int coinInserted = 0;

// ULTSNC Props for coin sensor
int
const coin_trigPin = 12;
int
const coin_echoPin = 13;

// for mask sensor
int
const mask_trigPin = 10;
int
const mask_echoPin = 11;

// for alcohol sensor
int
const alc_trigPin = 8;
int
const alc_echoPin = 9;

int maskDispensed = 0, alcDispensed = 0;

int distanceCalculator(int duration) {
    return (0.034 * duration);
    // return ((340 * duration) / 2); // To get millimeter results
}

// Led lights
int
const ledLight = 2;

// mask led light
int
const maskLed = 3;

// alc led light
int
const alcLed = 4;

int coin_duration, coin_distance;
int maskDuration, maskSensorDistance;
int alcDuration, alcSensorDistance;

// Servo motors variables
int maskServoPin = A0;

Servo maskServo;

void dispenseEvent() {
    // Set mask trigger pin to LOW
    digitalWrite(mask_trigPin, HIGH);
    delay(10);
    digitalWrite(mask_trigPin, LOW);

    Serial.println("Dispense event running.");

    // Calculate: mask sensor
    maskDuration = pulseIn(mask_echoPin, HIGH);
    maskSensorDistance = distanceCalculator(maskDuration);

    Serial.print("\nMask Sensor Distance: ");
    Serial.println(maskSensorDistance);

    if (maskSensorDistance < 50 && maskSensorDistance > 0 && maskDispensed == 0) {
        maskDispensed = 1;

        // Turn on led
        digitalWrite(maskLed, HIGH);

        // Perform dispense
        Serial.println("Mask dispensed only once.");
    }

    // Set alcohol trigger pin to LOW.
    digitalWrite(alc_trigPin, HIGH);
    delay(10);
    digitalWrite(alc_trigPin, LOW);

    // Calculate: alcohol sensor
    alcDuration = pulseIn(alc_echoPin, HIGH);
    alcSensorDistance = distanceCalculator(alcDuration);

    Serial.print("\nAlcohol Sensor Distance: ");
    Serial.println(alcSensorDistance);

    if (alcSensorDistance < 50 && alcSensorDistance > 0 && alcDispensed == 0) {
        alcDispensed = 1;

        // Turn on led
        digitalWrite(alcLed, HIGH);

        // Perform dispense
        Serial.println("Alcohol dispensed only once.");
    }

    // If this is true, it means user has already dispense both mask and alcohol
    if (maskDispensed == 1 && alcDispensed == 1) {
        Serial.println("Both mask and alcohol already dispensed, THANK YOU!");

        maskDispensed = 0;
        alcDispensed = 0;

        // Set coin inserted to 0
        coinInserted = 0;

        // Turn off both LED
        digitalWrite(maskLed, LOW);
        digitalWrite(alcLed, LOW);

        digitalWrite(ledLight, LOW);

        delay(500);
    }
}

void setup() {

    Serial.begin(9600);

    maskServo.attach(maskServoPin);

    pinMode(coin_trigPin, OUTPUT);
    pinMode(coin_echoPin, INPUT);

    pinMode(mask_trigPin, OUTPUT);
    pinMode(mask_echoPin, INPUT);

    pinMode(alc_trigPin, OUTPUT);
    pinMode(alc_echoPin, INPUT);

    // Set up led lights.
    pinMode(ledLight, OUTPUT);
    pinMode(maskLed, OUTPUT);
    pinMode(alcLed, OUTPUT);
}

void loop() {

    // set coin trigger pin to LOW
    digitalWrite(coin_trigPin, HIGH);
    delay(10);
    digitalWrite(coin_trigPin, LOW);

    coin_duration = pulseIn(coin_echoPin, HIGH);
    coin_distance = distanceCalculator(coin_duration);

    Serial.print("Coin distance: ");
    Serial.println(coin_distance);

    // For every coin sensed, coin++;
    if (coin_distance < 50 && coin_distance > 0 && coinReaderIsReady == 1) {
        coinInserted++;
        Serial.print("Coin inserted: ");
        Serial.println(coinInserted);
    }

    // If coin is greater or equal to 3, run dispose event.
    if (coinInserted >= 3) {
        // Dispense on sense.
        digitalWrite(ledLight, HIGH);
        coinInserted = 3;

        Serial.println("Running 2000");
        maskServo.writeMicroseconds(2000);
        delay(950);
      
        Serial.println("Running 1500");
        maskServo.writeMicroseconds(1500);
        delay(500);

        dispenseEvent();
    }

    Serial.println("\nEnd of loop.\n");
    coinReaderIsReady = 1;
    delay(500);
}