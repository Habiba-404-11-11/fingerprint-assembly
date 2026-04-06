/***************************************************
 * FINAL SKETCH: Direct Match to LED Control
 * * Goal: 
 * 1. Find a match using the sensor's internal memory.
 * 2. If MATCH is found: Green LED (Pin 10) is ON.
 * 3. If NO MATCH is found: Red LED (Pin 9) is ON.
 * 4. Loop speed is slowed down (1 second delay) for easy Serial Monitor reading.
 * * NOTE: Ensure you have enrolled at least one finger using the 'enroll' example first.
 ****************************************************/

#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h> // Keep for compatibility with Uno/Nano

// --- 1. LED and Sensor Pin Configuration ---
const int LED_RED = 9;   
const int LED_GREEN = 10; 

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(2, 3);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// --- Function Prototypes ---
void turnOffLEDs();
uint8_t getFingerprintID(); 


void setup()
{
    Serial.begin(9600);
    while (!Serial); // Wait for Serial Monitor to connect (helpful for debugging)
    delay(100);
    Serial.println("\n\n--- Fingerprint Match & LED Control Initialized ---");

    // --- 3. LED SETUP ---
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    turnOffLEDs(); // Ensure both are off at startup

    // --- 4. Sensor Initialization ---
    finger.begin(57600);
    delay(5);
    if (finger.verifyPassword()) {
        Serial.println("Found fingerprint sensor!");
    } else {
        Serial.println("Did not find fingerprint sensor :(");
        // Turn on red LED indefinitely if the sensor cannot be found
        digitalWrite(LED_RED, HIGH); 
        while (1) { delay(1); }
    }

    finger.getTemplateCount();

    if (finger.templateCount == 0) {
        Serial.println("Sensor is empty. Please run the 'enroll' example first.");
    }
    else {
        Serial.println("\nWaiting for finger scan...");
        Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
    }
}

void loop() // The main loop runs repeatedly
{
    getFingerprintID(); // Check for a finger
    
    // --- SLOW DOWN THE LOOP ---
    // Wait 1 second before checking again, making the Serial output readable.
    delay(1000); 
}

// Helper function to turn off both LEDs
void turnOffLEDs() {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
}

// Function to check for a finger, process it, search, and control LEDs
uint8_t getFingerprintID() {
    turnOffLEDs(); // Turn off any previous LED state before the new scan

    // --- Stage 1: Get Image ---
    uint8_t p = finger.getImage();
    switch (p) {
        case FINGERPRINT_OK:
            // Serial.println("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            // This is the normal state, no action needed, exit quickly.
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
        case FINGERPRINT_IMAGEFAIL:
            // Serial.println("Sensor Error or Communication Problem.");
            return p;
        default:
            return p;
    }

    // --- Stage 2: Convert Image to Template Features ---
    p = finger.image2Tz();
    switch (p) {
        case FINGERPRINT_OK:
            // Serial.println("Image converted");
            break;
        case FINGERPRINT_IMAGEMESS:
        case FINGERPRINT_FEATUREFAIL:
        case FINGERPRINT_INVALIDIMAGE:
            // Serial.println("Features failed. Try adjusting finger pressure.");
            return p;
        default:
            return p;
    }

    // --- Stage 3: Search Internal Database ---
    p = finger.fingerSearch();

    if (p == FINGERPRINT_OK) {
        // --- SUCCESS: MATCH FOUND ---
        // Serial.println(">> FINGERPRINT MATCH FOUND! <<");
        // Serial.print("Found ID #"); 
        Serial.print(50);
        Serial.print("-");
        Serial.print(finger.fingerID);
        Serial.print('\n');
        // Serial.print(" with confidence: "); Serial.println(finger.confidence);
        
        digitalWrite(LED_GREEN, HIGH); // Green LED ON (Access Granted)
        return finger.fingerID;

    } else if (p == FINGERPRINT_NOTFOUND) {
        // --- FAILURE: NO MATCH ---
        // Serial.println(">> NO MATCH FOUND. ACCESS DENIED. <<");
        Serial.print(60);
        Serial.print('\n');
        digitalWrite(LED_RED, HIGH); // Red LED ON (Access Denied)
        return p;

    } else {
        // --- COMMUNICATION OR UNKNOWN ERROR ---
        // Serial.println("Unknown search error.");
        return p;
    }
}