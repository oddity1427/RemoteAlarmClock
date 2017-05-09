#include <Wire.h>
#include <SparkFunDS1307RTC.h>

void setup() {
  // put your setup code here, to run once:
  rtc.begin();
  rtc.autoTime();
  rtc.set12Hour();
  

}

void loop() {
  // put your main code here, to run repeatedly:
  rtc.update();
  Serial.begin(9600);
  Serial.print(rtc.hour());
  Serial.print(":");
  Serial.print(rtc.minute());
  Serial.print(":");
  Serial.print(rtc.second());
  Serial.println();
  Serial.end();
  delay(750);

}
