#ifndef WINCH_H_
#define WINCH_H_


#include "WPILib.h"

#define PI 3.1415926535

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
	
	static const int PULSES_PER_REV = 250;
	static const float WINCH_CIRCUMFERENCE = 12.875;//INCHES!!!!!
	static const float CATAPULT_ARM_LENGTH = 16.4; //Inches: length from catapult rotation point to pulling point
	static const float REST_ANGLE = 2.0;//angle between horizontal and catapult (>90 degrees)
	
	//TODO:verify these three
	static const float CATAPULT_X = 20;//Horizontal dist from winch tumbler's center to pivot point
	static const float CATAPULT_Y = 30;//Vertical dist from winch tumbler's center to pivot point
	static const float BALL_HEIGHT = 37;//Inches: ball height off the ground at rest (center of ball)
	
	static const float BALL_MASS = 1.247;//Kg
	static const float SPRING_CONST = 241;//newton meters per radian
	static const float GRAVITY = -9.8;//GRAVITY = 10
	
	float computeLengthFromAngle(float angle);//radians
	float computeEncoderStepsFromLength(float length);//inches
	float computeEcoderStepsFromAngle(float angle);//combines two previous functions
	
	
public:
	Winch(Victor * motor, Solenoid * sol, Encoder * encoder, DigitalInput * start_pos, DigitalInput * max_pos);
	void set_target_rotations(float n);
	float get_target_rotations();
	void wind_back(float angle);
	void fire();
	void update();
	void computeAngleFromDistance(float dist);
};


#endif
