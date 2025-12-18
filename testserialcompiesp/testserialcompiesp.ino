const int ledPin = 25;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600); // Inisialisasi komunikasi serial
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == '1') {
      // Nyalakan LED
      digitalWrite(ledPin, HIGH);
    } else if (command == '0') {
      // Matikan LED
      digitalWrite(ledPin, LOW);
    }
  }
}
