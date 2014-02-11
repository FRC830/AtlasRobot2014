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
	static const float WINCH_CIRCUMFERENCE = 12.75;//INCHES!!!!!
	static const float CATAPULT_ARM_LENGTH = 10; //length from catapult rotation point to pulling point
	static const float REST_ANGLE = 0.75*PI;//angle between horizontal and catapult (>90 degrees)
	
	static const float CATAPULT_X = 20;//Horizontal dist from winch tumbler's center to pivot point
	static const float CATAPULT_Y = 30;//Vertical dist from winch tumbler's center to pivot point
	
	float computeLengthFromAngle(float angle);//radians
	float computeEncoderStepsFromLength(float length);//inches
	float computeEcoderStepsFromAngle(float angle);//combines two previous functions
	
public:
	Winch(Victor * motor, Solenoid * sol, Encoder * encoder, DigitalInput * start_pos, DigitalInput * max_pos);
	void set_target_rotations(float n);
	float get_target_rotations();
	void wind_back(float angle);
	void fire();
	
};


#endif
