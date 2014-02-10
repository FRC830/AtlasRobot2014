#include "Rangefinder.h"
#include <cmath>


Rangefinder::Rangefinder(Ultrasonic * us_left, Ultrasonic * us_right){
	left = us_left;
	right = us_right;
	left->SetAutomaticMode(true);
	right->SetAutomaticMode(true);
}

//angle in rads, relative to a position facing directly towards a surface
//positive angle = clockwise from vertical
float Rangefinder::robot_angle(){
	float diff = left->GetRangeInches() - right->GetRangeInches();
	float angle_rads = atan(diff / SENSOR_DISTANCE);
	return angle_rads; 
}

//return distance between robot and nearest surface
float Rangefinder::robot_distance(){
	//average both distances to find distance from center
	return (left->GetRangeInches() + right->GetRangeInches()) / 2;
}
