#define BUT0 34
#define BUT1 32
#define delay_s 550

void setup() {
  Serial.begin(9600);
  DDRK = 0xFF;
  PORTK = 0;

  pinMode(BUT0, INPUT_PULLUP);
  pinMode(BUT1, INPUT_PULLUP);
}

void loop() {
  if (Serial.available()) {
    int val = Serial.read();
    if (val == 0xA1) {
      algo1();
    }
    if (val == 0xB1) {
      algo2();
    }
  }
  if (digitalRead(BUT0) == LOW) {
    Serial.write(0xA1);
    delay(200);
  }
  if (digitalRead(BUT1) == LOW) {
    Serial.write(0xB1);
    delay(200);
  }
}

void algo1() {
  PORTK = B00011000;
  delay(delay_s);
  PORTK = B00100100;
  delay(delay_s);
  PORTK = B01000010;
  delay(delay_s);
  PORTK = B10000001;
  delay(delay_s);
  PORTK=0;
}

void algo2() {
  for (int i = 7; i >= 0; i--) {
    if ((i % 2) == 1) {
      PORTK |= (1 << i);
      delay(delay_s);
      PORTK &= ~(1 << i);
    }
  }
  for (int i = 7; i >= 0; i--) {
    if ((i % 2) == 0) {
      PORTK |= (1 << i);
      delay(delay_s);
      PORTK &= ~(1 << i);
    }
  }
}
