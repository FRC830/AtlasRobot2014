#ifndef RANGEFINDER_H_
#define RANGEFINDER_H_

#include "WPILib.h"
#define ARRAY_LENGTH 4
class Rangefinder {
private:
	static const float SENSOR_DISTANCE = 1.0f; //distance between the two sensors, in inches
	Ultrasonic * left;
	Ultrasonic * right;
    int distance_state_l;
    int counter_l;
    float distance;
    float distance_group[ARRAY_LENGTH];
    int current_array_point;
    int invalid_count;
public:
	Rangefinder(Ultrasonic * us_left, Ultrasonic * us_right);
	float robot_angle();
	float robot_distance();
	void update();
};

#endif
