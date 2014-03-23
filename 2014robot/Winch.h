#ifndef WINCH_H_
#define WINCH_H_

#include "WPILib.h"

class Winch {
private:
	typedef enum e_winch_mode {HOLDING, WINDING_BACK, FIRING, POST_FIRING} winch_mode;
	winch_mode mode;
	
	Victor * winch_motor;
	Solenoid * clutch;
	Timer * timer;
	bool clutch_position;
	Encoder * winch_encoder;
	DigitalInput * zero_pt_lim_switch;
	DigitalInput * max_lim_switch;
	static const bool CLUTCH_IN = true; //TODO: determine this
	static const bool CLUTCH_OUT = false;
	float target_rotations;
	
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
	Winch(Victor * motor, Solenoid * sol, Encoder * encoder, DigitalInput * start_pos, DigitalInput * max_pos);
	/*After this function is called once, the winch winds back until it hits the limit switch.*/
	void wind_back();
	/*
	 * After this function is called once, the clutch is released long enough for the catapult to fire
	 * then the clutch is pushed back and the winch is spun back briefly to allow it to reengage.
	 */
	void fire();
	/*
	 * This function MUST be called EVERY CYCLE in order for the effects of the wind_back and fire
	 * functions to occur.
	 */
	void update();
	
	//we don't actually use any of these
	void set_target_rotations(float n);
	float get_target_rotations();
	void wind_back_rotations(float n_rotations);
	void wind_back_dist(float dist);
	float computeAngleFromDistance(float dist);//input dist in feet
};


#endif
