void setup() {
  pinMode(7, OUTPUT);  // Use pin 7 to send signal
}

void loop() {
  digitalWrite(7, HIGH);
  delay(500); // 0.5s HIGH
  digitalWrite(7, LOW);
  delay(500); // 0.5s LOW
}
