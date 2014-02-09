#include "Arm.h"

Arm::Arm(Victor * roller_motor, Victor * pivot_motor, DigitalInput * floor, 
		DigitalInput * low, DigitalInput * top, DigitalInput * ball){
	roller = roller_motor;
	pivot = pivot_motor;
	floor_switch = floor;
	low_switch = low;
	top_switch = top;
	ball_switch = ball;
	roller_speed = 0.0f;
	below_low_goal = false;
}

void Arm::set_position(Position pos){
	target = pos;
}

void Arm::set_roller(float val){
	roller_speed = val;
}

float Arm::get_roller() {
	return roller_speed;
}

void Arm::update(){
	//don't let the roller keep pulling the ball in if it's pressing the switchs
	if (roller_speed > 0.0f && ball_switch->Get()){
		roller->Set(0.0f);
	} else {
		roller->Set(roller_speed);
	}
	
	if (low_switch->Get()){
		if (target == top){
			below_low_goal = false;
		} else if (target == floor) {
			below_low_goal = true;
		}
	}
	
	//assume positive input = up, negative = down
	switch(target){
	case floor:
		if (floor_switch->Get())
			pivot->Set(0.0f);
		else
			pivot->Set(-PIVOT_SPEED);
		break;
	case top:
		if (top_switch->Get())
			pivot->Set(0.0f);
		else
			pivot->Set(PIVOT_SPEED);
		break;
	case low_goal:
		if (low_switch->Get())
			pivot->Set(0.0f);
		else if (below_low_goal)
			pivot->Set(PIVOT_SPEED);
		else
			pivot->Set(-PIVOT_SPEED);
		break;
	case none:
		break; //allow main function to set the motor
	}
			
	
	
}
