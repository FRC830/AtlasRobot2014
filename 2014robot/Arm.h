#ifndef ARM_H_
#define ARM_H_

#include "WPILib.h"

class Arm{
private:
	static const float PIVOT_SPEED = 0.5f;
	Victor * roller;
	Victor * pivot;
	float roller_speed;
	bool below_low_goal;
	DigitalInput * floor_switch;
	DigitalInput * low_switch;
	DigitalInput * top_switch;
	DigitalInput * ball_switch;
	
public:
	enum Position {floor, low_goal, top, none};
	Position target;
	Arm(Victor * roller_motor, Victor * pivot_motor, DigitalInput * floor, DigitalInput * low, DigitalInput * top, DigitalInput * ball);
	void set_position(Position pos);
	void set_roller(float val);
	float get_roller();
	void update();
};

#endif
