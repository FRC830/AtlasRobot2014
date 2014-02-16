#ifndef WINCH_H_
#define WINCH_H_

#include "WPILib.h"

class Winch {
private:
	Victor * winch_motor;
	Solenoid * clutch;
	bool clutch_position;
	Encoder * winch_encoder;
	DigitalInput * zero_pt_lim_switch;
	DigitalInput * max_lim_switch;
	static const bool CLUTCH_IN = true; //TODO: determine this
	static const bool CLUTCH_OUT = false;
	float target_rotations;
	bool winding_back;
	bool firing;
	
	static const double PI = 3.1415926535;
	static const int PULSES_PER_REV = 250;
	static const float WINCH_CIRCUMFERENCE = 12.875;//INCHES!!!!!
	//static const float WINCH_RADIUS = WINCH_CIRCUMFERENCE/(2*PI);
	static const float CATAPULT_ARM_LENGTH = 16.4; //Inches: length from catapult rotation point to pulling point
	static const float REST_ANGLE = 2.0;//angle between horizontal and catapult (>90 degrees)
	
	//TODO:verify these three
	static const float CATAPULT_X = 9.5f;//Horizontal dist from winch tumbler's center to pivot point
	static const float CATAPULT_Y = 19.0f;//Vertical dist from winch tumbler's center to pivot point
	static const float BALL_HEIGHT = 37;//Inches: ball height off the ground at rest (center of ball)
	
	static const float BALL_MASS = 1.247;//Kg
	static const float SPRING_CONST = 60.25;//newton meters per radian (per spring)
	static const float GRAVITY = -9.8;//GRAVITY = 10
	static const int NUM_SPRINGS = 3;
	static const int CATAPULT_MASS = 1;//(kg) mass of catapult arm, we can play with this value
	static const int GOAL_HEIGHT = 2;//Meters - height to center of goal
	
	float computeLengthFromAngle(float angle);//radians
	float computeEncoderStepsFromLength(float length);//inches
	float computeEncoderStepsFromAngle(float angle);//combines two previous functions
	
	
public:
	static const float STANDARD_ROTATIONS_TARGET = 2.0f; //TODO: determine this; rotation target for normal shot
	static const float STANDARD_ROTATIONS_INCREMENT = 0.2; //TODO: determine a good value for this
	Winch(Victor * motor, Solenoid * sol, Encoder * encoder, DigitalInput * start_pos, DigitalInput * max_pos);
	void set_target_rotations(float n);
	float get_target_rotations();
	void wind_back_rotations(float n_rotations);
	void wind_back_dist(float dist);
	void wind_back();
	void fire();
	void update();
	float computeAngleFromDistance(float dist);//input dist in feet
};


#endif
