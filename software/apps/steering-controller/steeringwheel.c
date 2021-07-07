#include <math.h>
#include <stdio.h>
#include "steeringwheel.h"
#include "display.h"
#include "lsm9ds1.h"

float degree = 0;
float degree1 = 0;
float offset = 0;
float offset1 = 0;
static float read_y_tilt() {

	lsm9ds1_measurement_t meas = lsm9ds1_read_accelerometer();
	float x_val = meas.x_axis;
	float y_val = meas.y_axis;
	float z_val = meas.z_axis;

	float ydeg = atan(x_val/sqrt(y_val*y_val + z_val*z_val))*180 / M_PI;
	return ydeg;
}
static float read_x_tilt() {

	lsm9ds1_measurement_t meas = lsm9ds1_read_accelerometer();
	float x_val = meas.x_axis;
	float y_val = meas.y_axis;
	float z_val = meas.z_axis;

	float xdeg= atan(y_val/sqrt(x_val*x_val + z_val*z_val)) * 180/ M_PI;
	return xdeg;
}
steering_wheel_state_t controller(steering_wheel_state_t state) {
  char buf[16];
  switch(state) {
    case OFF: {
      //lsm9ds1_stop_gyro_integration();
      //lsm9ds1_start_gyro_integration();
      offset = read_x_tilt();
      offset1 = read_y_tilt();
      state = STRAIGHT;
    }
    case STRAIGHT: {
      //degree = lsm9ds1_read_gyro_integration().x_axis;
      //degree1 = lsm9ds1_read_gyro_integration().y_axis;
      degree = read_x_tilt() - offset;
      degree1 = read_y_tilt() - offset1;
      snprintf(buf, 16, "%f", degree);
      display_write(buf, DISPLAY_LINE_1);
      display_write("STRAIGHT", DISPLAY_LINE_0);
      if (degree1 > 25) {
        state = PITCHUP;
      } else if (degree1 < -35) {
        state = PITCHDN;
      } else if (degree < -25) {
        state = RIGHT;
      } else if (degree > 25) {
        state = LEFT;
      } else {
        state = STRAIGHT;
      }
      break;
    }
    case LEFT: {
      //degree = lsm9ds1_read_gyro_integration().x_axis;
      degree = read_x_tilt() - offset;
      snprintf(buf, 16, "%f", degree);
      display_write(buf, DISPLAY_LINE_1);
      display_write("LEFT", DISPLAY_LINE_0);
      if (degree < -25) {
        state = RIGHT;
      } else if (degree > 25) {
        state = LEFT;
      } else {
        state = STRAIGHT;
      }
      break;
    }
    case RIGHT: {
      //degree = lsm9ds1_read_gyro_integration().x_axis;
      degree = read_x_tilt() - offset;
      snprintf(buf, 16, "%f", degree);
      display_write(buf, DISPLAY_LINE_1);
      display_write("RIGHT", DISPLAY_LINE_0);
      if (degree < -25) {
        state = RIGHT;
      } else if (degree > 25) {
        state = LEFT;
      } else {
        state = STRAIGHT;
      }
      break;
    }
    case PITCHUP: {
      //degree1 = lsm9ds1_read_gyro_integration().y_axis;
      degree1 = read_y_tilt() - offset1;
      snprintf(buf, 16, "%f", degree1);
      display_write(buf, DISPLAY_LINE_1);
      display_write("+PITCH", DISPLAY_LINE_0);
      if (degree1 > 25) {
        state = PITCHUP;
      } else if (degree1 < -35) {
        state = PITCHDN;
      } else {
        state = STRAIGHT;
      }
      break;
    }
    case PITCHDN: {
      //degree1 = lsm9ds1_read_gyro_integration().y_axis;
      degree1 = read_y_tilt() - offset1;
      snprintf(buf, 16, "%f", degree1);
      display_write(buf, DISPLAY_LINE_1);
      display_write("-PITCH", DISPLAY_LINE_0);
      if (degree1 > 25) {
        state = PITCHUP;
      } else if (degree1 < -35) {
        state = PITCHDN;
      } else {
        state = STRAIGHT;
      }
      break;
    }
  }
  return state;
}