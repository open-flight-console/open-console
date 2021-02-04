#include "Keyboard.h"
#include "Mouse.h"

const int switch0Pin = 4; //on off
const int switch1Pin = 11; //autopilot
const int buttonR1Pin = 8; //R1
const int buttonR2Pin = 7; //R2
const int buttonR3Pin = 9; //R3
const int buttonL1Pin = 6; //L1
const int buttonL2Pin = 5; //L2
const int buttonL3Pin = 10; //L3

int prevR1State = HIGH;
int prevR2State = HIGH;
int prevR3State = HIGH;
int prevL1State = HIGH;
int prevL2State = HIGH;
int prevL3State = HIGH;

char prevOut = 'w';
char buckler_out = 'w';

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
  int switch0State = digitalRead(switch0Pin);
  int switch1State = digitalRead(switch1Pin);
  int buttonR1State = digitalRead(buttonR1Pin);
  int buttonR2State = digitalRead(buttonR2Pin);
  int buttonR3State = digitalRead(buttonR3Pin);
  int buttonL1State = digitalRead(buttonL1Pin);
  int buttonL2State = digitalRead(buttonL2Pin);
  int buttonL3State = digitalRead(buttonL3Pin);
  if (Serial1.available() > 0) {
    buckler_out = Serial1.read();
  }
  if (switch0State == HIGH) {
    if ((buttonR1State != prevR1State) && (buttonR1State == HIGH)) {
      Mouse.click(MOUSE_RIGHT);
    }
    if ((buttonL1State != prevL1State) && (buttonL1State == HIGH)) {
      Mouse.click(MOUSE_LEFT);
    }
    if ((switch1State == HIGH)) {
      if ((buttonR2State != prevR2State) && (buttonR2State == HIGH)) {
        Keyboard.press('3');
      } else if (buttonR2State != HIGH) {
        Keyboard.release('3');
      }
      if ((buttonR3State != prevR3State) && (buttonR3State == HIGH)) {
        Keyboard.press('4');
      } else if (buttonR3State != HIGH) {
        Keyboard.release('4');
      }
      if ((buttonL2State != prevL2State) && (buttonL2State == HIGH)) {
        Keyboard.press('1');
      } else if (buttonL2State != HIGH) {
        Keyboard.release('1');
      }
      if ((buttonL3State != prevL3State) && (buttonL3State == HIGH)) {
        Keyboard.press('2');
      } else if (buttonL3State != HIGH) {
        Keyboard.release('2');
      }
      if (Serial1.available() > 0) {
        if (buckler_out != prevOut) {
          Keyboard.releaseAll();
          prevOut = buckler_out;
        }
        if (buckler_out == '-') {
          Mouse.move(0, 4, 0);
          Keyboard.press(KEY_LEFT_SHIFT);
        } else if (buckler_out == '+') {
          Mouse.move(0, -4, 0);
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
      if ((buttonR2State != prevR2State) && (buttonR2State == HIGH)) {
        Keyboard.press(' ');
      } else if (buttonR2State != HIGH) {
        Keyboard.release(' ');
      }
      if ((buttonR3State != prevR3State) && (buttonR3State == HIGH)) {
        Keyboard.press('s');
      } else if (buttonR3State != HIGH) {
        Keyboard.release('s');
      }
      if ((buttonL2State != prevL2State) && (buttonL2State == HIGH)) {
        Keyboard.press(KEY_LEFT_SHIFT);
      } else if (buttonL2State != HIGH) {
        Keyboard.release(KEY_LEFT_SHIFT);
      }
      if ((buttonL3State != prevL3State) && (buttonL3State == HIGH)) {
        Keyboard.press('w');
      } else if (buttonL3State != HIGH) {
        Keyboard.release('w');
      }
      if (Serial1.available() > 0) {
          if (buckler_out == '+') {
            Mouse.move(0, -4, 0);
          } else if (buckler_out == '-') {
            Mouse.move(0, 4, 0);
          } else if (buckler_out == 'a') {
            Mouse.move(-4, 0, 0);
          } else if (buckler_out == 'd') {
            Mouse.move(4, 0, 0);
          }
       }
    }
    prevR1State = buttonR1State;
    prevR2State = buttonR2State;
    prevR3State = buttonR3State;
    prevL1State = buttonL1State;
    prevL2State = buttonL2State;
    prevL3State = buttonL3State;
  } else {
    Keyboard.releaseAll();
  }
  delay(10);
}
