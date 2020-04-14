#define BUT 34
#define delay_s 55

void setup() {
  DDRK = 0xFF;
  PORTK = 0;

  pinMode(BUT, INPUT_PULLUP);
}

void loop() {
  PORTK = 0;
  if (digitalRead(BUT) == LOW){
    PORTK = 0x18;
    delay(delay_s);
    PORTK = 0x24;
    delay(delay_s);
    PORTK = 0x42;
    delay(delay_s);
    PORTK = 0x81;
    delay(delay_s);
  }
}
