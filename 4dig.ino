#include <SimpleDHT.h>

#define DHT_PIN D5
#define DATA_PIN D6
#define CLOCK_PIN D7
#define LATCH_PIN D8

// sensor info
SimpleDHT11 dhtSensor(DHT_PIN);
int dhtFrequency = 1000;
int dhti = 0;
byte temperature = 1;
byte _humidity; // humidity throwaway


// display info
int digitPins[] = { D1, D2, D3, D4 };
int digits[] = { 252, 96, 218, 242, 102, 182, 190, 224, 254, 246 };
int valueDigits[4];
int blank = 0;
int error[] = { 10, 10, 158, blank };

void resetAll() {
  for (int j = 0; j < 4; j++) digitalWrite(digitPins[j], HIGH);
}

void writeDigit(int digit, int value) {
  resetAll();
  digitalWrite(digit, LOW);
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, value);
  digitalWrite(LATCH_PIN, HIGH);
  delay(1);
}

void setValueDigits(int value) {
  int remainingValue;
  int digitValue;
  
  // most significant digit first
  digitValue = value / 1000;
  // bail if the number's too big
  if (digitValue > 9) {
    memcpy(valueDigits, error, sizeof(valueDigits));
    return;
  }
  valueDigits[3] = digits[digitValue];

  remainingValue = value % 1000;
  digitValue = remainingValue / 100;
  valueDigits[2] = digits[digitValue];

  remainingValue = value % 100;
  digitValue = remainingValue / 10;
  valueDigits[1] = digits[digitValue];

  remainingValue = value % 10;
  digitValue = remainingValue;
  valueDigits[0] = digits[digitValue];

  int i = 3;
  while (valueDigits[i] == digits[0] && i > 1) {
    valueDigits[i] = blank;
    i--;
  }
}

void writeToDisplay(int value) {
  setValueDigits(value);
  writeDigit(digitPins[0], valueDigits[3]);
  writeDigit(digitPins[1], valueDigits[2]);
  writeDigit(digitPins[2], valueDigits[1]);
  writeDigit(digitPins[3], valueDigits[0]);
}
void updateTemperature() {
  Serial.println('Getting temperature...');
  dhtSensor.read(&temperature, &_humidity, NULL);
  Serial.println(temperature);
}

void setup() {
  Serial.begin(9600);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  for (int i = 0; i < 10; i++) {
    pinMode(digitPins[i], OUTPUT);
  }
}

void loop() {
  if (dhti == dhtFrequency) {
    dhti = 0;
    updateTemperature();
  }
  dhti++;
  writeToDisplay(temperature);
}
