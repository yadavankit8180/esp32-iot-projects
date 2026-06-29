// ESP32 External LED Blink

const int ledPin = 2;   // GPIO connected to LED

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH);   // LED ON
  delay(1000);                  // Wait 1 second

  digitalWrite(ledPin, LOW);    // LED OFF
  delay(1000);                  // Wait 1 second
}

  delay(1000);
}