char toShow[6];
char digits[6];

void displayInit(byte *digitsIn);

void setTimeDigit(byte digit, byte time); // sets time digit, digit - 0 for hours, 1 for minutes and  2 for seconds

void setDisplayPort(char in);


void displayInit(byte *digitsIn) {
  DDR_DISPLAY = 0xFF;
  PORT_DISPLAY = 0;

  for (int i = 0; i < 6; i++) {
    digits[i] = digitsIn[i];
  }
}

void setTimeDigit(byte digit, byte time) {
  if ((digit == 0 && time >= 0 && time <= 24) ||
      ((digit == 1 || digit == 2) && time >= 0 && time <= 60)) {
    char s[3];
    sprintf(s, "%d", time);
    if (time < 10) {
      s[1] = s[0];
      s[0] = '0';
    }
    toShow[digit * 2] = s[0];
    toShow[digit * 2 + 1] = s[1];
  } else {
    toShow[digit * 2] = 'E';
    toShow[digit * 2 + 1] = 'E';
  }
}

void setDisplayPort(char in){
  if (in == '1') {
      PORT_DISPLAY = B11111001;
    } else if (in == '2') {
      PORT_DISPLAY = B10100100;
    } else if (in == '3') {
      PORT_DISPLAY = B10110000;
    } else if (in == '4') {
      PORT_DISPLAY = B10011001;
    } else if (in == '5') {
      PORT_DISPLAY = B10010010;
    } else if (in == '6') {
      PORT_DISPLAY = B10000010;
    } else if (in == '7') {
      PORT_DISPLAY = B11111000;
    } else if (in == '8') {
      PORT_DISPLAY = B10000000;
    } else if (in == '9') {
      PORT_DISPLAY = B10010000;
    } else if (in == '0') {
      PORT_DISPLAY = B11000000;
    } else {
      PORT_DISPLAY = B10000110;
    }
}
