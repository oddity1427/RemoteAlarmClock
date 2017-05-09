#define DIG_0 0x3F; 
#define DIG_1 0x06; 
#define DIG_2 0x5D; 
#define DIG_3 0x4F; 
#define DIG_4 0x66;
#define DIG_5 0x6D;
#define DIG_6 0x7D; 
#define DIG_7 0x07;
#define DIG_8 0x7F;
#define DIG_9 0x67;
#define DIG_A 0x77;
#define DIG_B 0x7B;
#define DIG_C 0x39;
#define DIG_D 0x5C;
#define DIG_E 0x7C;
#define DIG_F 0x71;
#define DIG_DOT 0x80;  

void setup() {
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(13, OUTPUT);
}

void loop() {
  for (int numberToDisplay = 0; numberToDisplay < 256; numberToDisplay++) {
    // take the latchPin low so 
    // the LEDs don't change while you're sending in bits:
    digitalWrite(8, LOW);
    // shift out the bits:
    shiftOut(13, 9, MSBFIRST, numberToDisplay);  

    //take the latch pin high so the LEDs will light up:
    digitalWrite(8, HIGH);
    // pause before next value:
    delay(500);
  }

}
