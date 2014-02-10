#ifndef ARM_H_
#define ARM_H_

#include "WPILib.h"

class Arm{
private:
	static const float PIVOT_SPEED = 0.5f;
	static const int ARM_TOP_POSITION = 100; //encoder ticks; TODO: determine this
	Victor * roller;
	Victor * pivot;
	float roller_speed;
	Encoder * encoder;
	DigitalInput * floor_switch;
	DigitalInput * top_switch;
	DigitalInput * ball_switch;
	PIDController * pid;
	
public:
	static const int TOP_POSITION = 100; //encoder ticks; TODO: determine this
	static const int FLOOR_POSITION = 0;
	static const int LOW_GOAL_POSITION = 20; //TODO: determine this
	Arm(Victor * roller_motor, Victor * pivot_motor, Encoder * enc, DigitalInput * floor, DigitalInput * top, DigitalInput * ball);
	void set_position(int pos); //position from 0 (floor) to 100 (maximum)
	void set_roller(float val);
	float get_roller();
	void update();
};


#endif
