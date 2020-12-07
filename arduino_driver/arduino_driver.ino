#include "Keyboard.h"
#include "Mouse.h"

const int buttonPin = 4;
int previousButtonState = HIGH;
char prevOut = 'w';

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(115200);
  Keyboard.begin();
  Mouse.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  //input coming in from buckler
  int buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    if (Serial1.available() > 0) {
      char buckler_out = Serial1.read();
      if (buckler_out != prevOut) {
          Keyboard.releaseAll();
          prevOut = buckler_out;
      }
      if (buckler_out == '+') {
        Mouse.move(0, 5, 0);
        Keyboard.press(KEY_LEFT_SHIFT);
      } else if (buckler_out == '-') {
        Mouse.move(0, -5, 0);
        Keyboard.press(' ');
      } else {
        //w, a, d
        Serial.print(buckler_out);
        Keyboard.press(buckler_out);
      }
    } else {
      Keyboard.releaseAll();
    }
  } else {
    Keyboard.releaseAll();
  }
  delay(10);
}
