#include <LiquidCrystal.h>

// LCD pin setup: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// LED pin definitions
const int redLED    = 6;   // Alert for wrong posture or smoke
const int yellowLED = 7;   // Alert for high heat/stress
const int greenLED  = 8;   // System Status: Normal
const int buzzerPin = A2;  // Audible alert pin (Buzzer)

// Sensor pin definitions
const int gasSensorPin  = A0;
const int tempSensorPin = A1;
const int trigPin       = 9;
const int echoPin       = 10;

// Threshold values for Exam Hall Monitoring
const float MIN_SAFE = 15.0; // Minimum safe distance in cm
const float MAX_SAFE = 30.0; // Maximum safe distance in cm
const int   SMOKE_LIMIT = 490; 
const float STRESS_TEMP = 38.0; 

void setup() {
  // Pin modes configuration
  pinMode(redLED,    OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED,  OUTPUT);
  pinMode(buzzerPin, OUTPUT); // Buzzer pin configuration
  pinMode(trigPin,   OUTPUT);
  pinMode(echoPin,   INPUT);

  // Initialize Serial Monitor for debugging
  Serial.begin(9600);

  // Initialize LCD display
  lcd.begin(16, 2);
  lcd.print("Exam Hall Guard");
  lcd.setCursor(0, 1);
  lcd.print("   System ON   ");
  delay(2000);
  lcd.clear();
}

// Function to calculate distance using Ultrasonic sensor
float getDistance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  if (duration == 0) return 400.0; // Return high value if no object detected
  return duration * 0.034 / 2.0;
}

// Function to read temperature from TMP36 sensor
float getTemperature() {
  int reading = analogRead(tempSensorPin);
  float voltage = reading * 5.0 / 1024.0;
  return (voltage - 0.5) * 100.0;
}

void loop() {
  
  // Reading data from all sensors
  int   smokeVal = analogRead(gasSensorPin);
  float tempC    = getTemperature();
  float distCm   = getDistance();

  // Condition Logic: Safe range is between 15cm and 30cm
  bool suspiciousPose = (distCm < MIN_SAFE || distCm > MAX_SAFE);
  bool smokeDetected  = (smokeVal >= SMOKE_LIMIT);
  bool highHeat       = (tempC >= STRESS_TEMP);

  // Debugging info on Serial Monitor
  Serial.print("Distance: "); Serial.print(distCm);
  Serial.print(" | Temp: ");   Serial.print(tempC);
  Serial.print(" | Smoke: ");  Serial.println(smokeVal);

  // --- Alert System Logic (LEDs & Buzzer) ---
  
  if (suspiciousPose || smokeDetected) {
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(buzzerPin, HIGH); // Activate buzzer for cheating or smoke
  } else {
    digitalWrite(redLED, LOW);
  }

  if (highHeat) {
    digitalWrite(yellowLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(buzzerPin, HIGH); // Activate buzzer for heat alert
  } else {
    digitalWrite(yellowLED, LOW);
  }

  // System status is GREEN and BUZZER is OFF if all conditions are normal
  if (!suspiciousPose && !smokeDetected && !highHeat) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(buzzerPin, LOW); // Turn off buzzer
  }

  // Update LCD Display: Line 1
  lcd.setCursor(0, 0);
  lcd.print("D:"); lcd.print((int)distCm); lcd.print("cm ");
  lcd.print("T:"); lcd.print((int)tempC); lcd.print("C  ");

  // Update LCD Display: Line 2
  lcd.setCursor(0, 1);
  if (suspiciousPose) {
    lcd.print("WRONG POSTURE!  ");
  } else if (smokeDetected) {
    lcd.print("ENV. DISTURBANCE");
  } else if (highHeat) {
    lcd.print("STRESS ALERT!!  ");
  } else {
    lcd.print("Status: NORMAL  ");
  }

  delay(500); // Wait for half a second before next loop
}
