#ifndef RANGEFINDER_H_
#define RANGEFINDER_H_

#include "WPILib.h"

class Rangefinder {
private:
	static const float SENSOR_DISTANCE = 1.0f; //distance between the two sensors, in inches
	Ultrasonic * left;
	Ultrasonic * right;
	
public:
	Rangefinder(Ultrasonic * us_left, Ultrasonic * us_right);
	float robot_angle();
	float robot_distance();
};

#endif
