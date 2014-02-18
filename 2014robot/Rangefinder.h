#ifndef RANGEFINDER_H_
#define RANGEFINDER_H_

#include "WPILib.h"

class Rangefinder {
private:
	static const int ARRAY_LENGTH = 8;
	static const float SENSOR_DISTANCE = 1.0f; //distance between the two sensors, in inches
	Ultrasonic * ultrasonic;
    int distance_state;
    int counter;
    float distance;
    float distance_group[ARRAY_LENGTH];
    int current_array_point;
    int invalid_count;
public:
	Rangefinder(Ultrasonic * us);
	float robot_angle();
	float robot_distance();
	void update();
};

#endif
