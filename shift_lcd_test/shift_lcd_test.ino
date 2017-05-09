#define DIG_0 0x3F 
#define DIG_1 0x06 
#define DIG_2 0x5B 
#define DIG_3 0x4F 
#define DIG_4 0x66
#define DIG_5 0x6D
#define DIG_6 0x7D 
#define DIG_7 0x07
#define DIG_8 0x7F
#define DIG_9 0x67
#define DIG_A 0x77
#define DIG_B 0x7C
#define DIG_C 0x39
#define DIG_D 0x5E
#define DIG_E 0x79
#define DIG_F 0x71
#define DIG_DOT 0x80  

int displayCodes[17] = { 
    DIG_0, DIG_1, DIG_2, DIG_3, DIG_4, DIG_5, DIG_6, DIG_7, DIG_8, DIG_9, DIG_A, DIG_B, DIG_C, DIG_D, DIG_E, DIG_F, DIG_DOT 
    } ;

void setup() {
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(13, OUTPUT);

  
  
}

void loop() {
  for (int i = 0; i < 17 ; i++) {
    // take the latchPin low so 
    // the LEDs don't change while you're sending in bits:
    digitalWrite(8, LOW);
    // shift out the bits:
    shiftOut(13, 9, MSBFIRST, displayCodes[i]);
    shiftOut(13, 9, MSBFIRST, displayCodes[i]);
    shiftOut(13, 9, MSBFIRST, displayCodes[i]);  

    //take the latch pin high so the LEDs will light up:
    digitalWrite(8, HIGH);
    // pause before next value:
    delay(500);
  }

}
