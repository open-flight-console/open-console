#include <math.h>
#include <stdio.h>
#include "steeringwheel.h"
#include "display.h"
#include "lsm9ds1.h"

float degree = 0;
float degree1 = 0;
steering_wheel_state_t controller(steering_wheel_state_t state) {
  char buf[16];
  switch(state) {
    case OFF: {
      lsm9ds1_stop_gyro_integration();
      lsm9ds1_start_gyro_integration();
      state = STRAIGHT;
    }
    case STRAIGHT: {
      degree = lsm9ds1_read_gyro_integration().x_axis;
      degree1 = lsm9ds1_read_gyro_integration().y_axis;
      snprintf(buf, 16, "%f", degree);
      display_write(buf, DISPLAY_LINE_1);
      display_write("STRAIGHT", DISPLAY_LINE_0);
      if (degree1 > 45) {
        state = PITCHDN;
      } else if (degree1 < -45) {
        state = PITCHUP;
      } else if (degree < -20) {
        state = LEFT;
      } else if (degree > 20) {
        state = RIGHT;
      } else {
        state = STRAIGHT;
      }
      break;
    }
    case LEFT: {
      degree = lsm9ds1_read_gyro_integration().x_axis;
      display_write("LEFT", DISPLAY_LINE_0);
      if (degree < -20) {
        state = LEFT;
      } else if (degree > 20) {
        state = RIGHT;
      } else {
        state = STRAIGHT;
      }
      break;
    }
    case RIGHT: {
      degree = lsm9ds1_read_gyro_integration().x_axis;
      display_write("RIGHT", DISPLAY_LINE_0);
      if (degree < -20) {
        state = LEFT;
      } else if (degree > 20) {
        state = RIGHT;
      } else {
        state = STRAIGHT;
      }
      break;
    }
    case PITCHUP: {
      degree1 = lsm9ds1_read_gyro_integration().y_axis;
      snprintf(buf, 16, "%f", degree1);
      display_write(buf, DISPLAY_LINE_1);
      display_write("+PITCH", DISPLAY_LINE_0);
      if (degree1 > 45) {
        state = PITCHDN;
      } else if (degree1 < -45) {
        state = PITCHUP;
      } else {
        state = STRAIGHT;
      }
      break;
    }
    case PITCHDN: {
      degree1 = lsm9ds1_read_gyro_integration().y_axis;
      snprintf(buf, 16, "%f", degree1);
      display_write(buf, DISPLAY_LINE_1);
      display_write("-PITCH", DISPLAY_LINE_0);
      if (degree1 > 45) {
        state = PITCHDN;
      } else if (degree1 < -45) {
        state = PITCHUP;
      } else {
        state = STRAIGHT;
      }
      break;
    }
  }
  return state;
}