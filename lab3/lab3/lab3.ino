#include <LiquidCrystal.h>

#define DDR_KEYPAD  DDRC
#define PORT_KEYPAD PORTC
#define PIN_KEYPAD  PINC
#define COURSOR_POS 3
#include "keypad4x4.h"

const int buzzerPin = 21;
char but;
byte mode = 0;
byte coursor_p = COURSOR_POS; //coursor start position
int startBuzzing = 0;

bool timer1 = false;
bool timer2 = false;
bool setTimer1 = false;
bool setTimer2 = false;

char inputTime[6]; // time entered by user
byte inputTime_p = 0;

int tempHour;
int tempMin;
int tempSec;
char temp[3];

const int rs = 49, rw = 48, en = 47, d4 = 46, d5 = 45, d6 = 44, d7 = 43;
LiquidCrystal lcd(rs, rw, en, d4, d5, d6, d7);

const PROGMEM char sixty[61][3] = {
  {"00"}, {"01"}, {"02"}, {"03"}, {"04"}, {"05"}, {"06"}, {"07"}, {"08"}, {"09"},
  {"10"}, {"11"}, {"12"}, {"13"}, {"14"}, {"15"}, {"16"}, {"17"}, {"18"}, {"19"},
  {"20"}, {"21"}, {"22"}, {"23"}, {"24"}, {"25"}, {"26"}, {"27"}, {"28"}, {"29"},
  {"30"}, {"31"}, {"32"}, {"33"}, {"34"}, {"35"}, {"36"}, {"37"}, {"38"}, {"39"},
  {"40"}, {"41"}, {"42"}, {"43"}, {"44"}, {"45"}, {"46"}, {"47"}, {"48"}, {"49"},
  {"50"}, {"51"}, {"52"}, {"53"}, {"54"}, {"55"}, {"56"}, {"57"}, {"58"}, {"59"}, {"_"}
};

struct Time
{
  unsigned char second, minute, hour;
};
Time Time1 = {0, 0, 0};
Time Time2 = {0, 0, 0};

void LCD_WriteStrPROGMEM(char *str, int n)
{
  for (int i = 0; i < n; i++)
    lcd.print( (char)pgm_read_byte( &(str[i]) ) );
}

ISR(TIMER5_COMPA_vect)
{
  if (timer1) {
    if (Time1.second != 0)
    {
      Time1.second--;
    } else {
      Time1.second = 60;
      if (Time1.minute != 0)
      {
        Time1.minute--;
      } else {
        if (Time1.hour != 0) {
          Time1.hour--;
        }
        else {
          timer1 = false;
          setTimer1 = false;
          startBuzzing = 1;
        }
      }
    }
  }
  if (timer2) {
    if (Time2.second != 0)
    {
      Time2.second--;
    } else {
      Time2.second = 60;
      if (Time2.minute != 0)
      {
        Time2.minute--;
      } else {
        if (Time2.hour != 0) {
          Time2.hour--;
        }
        else {
          timer2 = false;
          setTimer2 = false;
          startBuzzing = 1;
        }
      }
    }
  }
  if (startBuzzing != 0) {
    if (++startBuzzing % 2 == 0) {
      digitalWrite(buzzerPin, HIGH);
    } else if (startBuzzing == 30) {
      startBuzzing = 0;
      digitalWrite(buzzerPin, LOW);
    } else {
      digitalWrite(buzzerPin, LOW);
    }
  }
}

void setup() {
  noInterrupts();

  TCCR5A = 0x00;
  TCCR5B = (1 << WGM52) | (1 << CS52) | (1 << CS50); //CTC mode & Prescaler @ 1024
  TIMSK5 = (1 << OCIE5A); // дозвіл на переривання по співпадінню
  OCR5A = 0x3D08;// 1 секунда
  initKeyPad();

  lcd.begin(16, 2);
  interrupts();

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  temp[2] = '\0';
}

void quit() {
  lcd.clear();
  mode = 0;
  coursor_p = 3;
  inputTime_p = 0;
  startBuzzing = 0;
  digitalWrite(buzzerPin, HIGH);
  delay(300);
  digitalWrite(buzzerPin, LOW);
}

bool isTimeCorrect(char *inputTime) {
  temp[0] = inputTime[0];
  temp[1] = inputTime[1];
  sscanf(temp, "%d", &tempHour);

  temp[0] = inputTime[2];
  temp[1] = inputTime[3];
  sscanf(temp, "%d", &tempMin);

  temp[0] = inputTime[4];
  temp[1] = inputTime[5];
  sscanf(temp, "%d", &tempSec);

  return !(tempHour > 24 || tempMin > 60 || tempSec > 60);
}

void writeBlank(byte pos) {
  lcd.setCursor(COURSOR_POS, pos);
  LCD_WriteStrPROGMEM(sixty[60], 1);
  LCD_WriteStrPROGMEM(sixty[60], 1);
  lcd.write(':');
  LCD_WriteStrPROGMEM(sixty[60], 1);
  LCD_WriteStrPROGMEM(sixty[60], 1);
  lcd.write(':');
  LCD_WriteStrPROGMEM(sixty[60], 1);
  LCD_WriteStrPROGMEM(sixty[60], 1);
}

void showTime1() {
  lcd.setCursor(COURSOR_POS, 0);
  LCD_WriteStrPROGMEM(sixty[Time1.hour], 2);
  lcd.write(':');
  LCD_WriteStrPROGMEM(sixty[Time1.minute], 2);
  lcd.write(':');
  LCD_WriteStrPROGMEM(sixty[Time1.second], 2);
}

void showTime2() {
  lcd.setCursor(COURSOR_POS, 1);
  LCD_WriteStrPROGMEM(sixty[Time2.hour], 2);
  lcd.write(':');
  LCD_WriteStrPROGMEM(sixty[Time2.minute], 2);
  lcd.write(':');
  LCD_WriteStrPROGMEM(sixty[Time2.second], 2);
}

void loop() {
  if (setTimer1) {
    showTime1();
  } else if (mode != 1) {
    writeBlank(0);
  }
  if (setTimer2) {
    showTime2();
  } else if (mode != 2) {
    writeBlank(1);
  }
  if ( isButtonPressed() ) {
    digitalWrite(buzzerPin, HIGH);
    but = readKeyFromPad4x4();
    delay(100);
    digitalWrite(buzzerPin, LOW);
    if (but == 'F') {
      quit();
    } else if (mode == 1 || mode == 2) {               // mode 1 - setting time1, mode 2 - setting time2
      inputTime[inputTime_p++] = but;
      if (coursor_p == 5 || coursor_p == 8)
        coursor_p++;
      lcd.setCursor(coursor_p++, mode - 1);
      lcd.write(but);
      if (coursor_p >= 11)
        but = 'E';
      if (but == 'E') {
        if (isTimeCorrect(inputTime)) {
          if (mode == 1) {
            Time1.second = tempSec;
            Time1.minute = tempMin;
            Time1.hour = tempHour;
            setTimer1 = true;
          } else {
            Time2.second = tempSec;
            Time2.minute = tempMin;
            Time2.hour = tempHour;
            setTimer2 = true;
          }
        }
        quit();
      }
    } else if (but == 'A') {
      mode = 1;
      writeBlank(mode - 1);
    } else if (but == 'B') {
      mode = 2;
      writeBlank(mode - 1);
    } else if (but == 'C') {
      if (timer1) {
        setTimer1 = false;
        Time1 = {0, 0, 0};
        timer1 = false;
      } else {
        timer1 = true;
      }
    } else if (but == 'D') {
      if (timer2) {
        setTimer2 = false;
        Time2 = {0, 0, 0};
        timer2 = false;
      } else {
        timer2 = true;
      }
    }
  }
}
