#include "Arm.h"

Arm::Arm(Victor * roller_motor, Victor * pivot_motor, Encoder * enc, DigitalInput * floor, DigitalInput * top, DigitalInput * ball){
	roller = roller_motor;
	pivot = pivot_motor;
	encoder = enc;
	floor_switch = floor;
	top_switch = top;
	ball_switch = ball;
	roller_speed = 0.0f;
	pid = new PIDController(0.1, 0.0, 0.0, encoder, pivot);
}

void Arm::set_position(int pos){
	pid->SetSetpoint((pos * TOP_POSITION) / 100);
}

void Arm::set_roller(float val){
	roller_speed = val;
}

float Arm::get_roller() {
	return roller_speed;
}

void Arm::update(){
	//don't let the roller keep pulling the ball in if it's pressing the switch
	if (roller_speed > 0.0f && !ball_switch->Get()){
		roller->Set(0.0f);
	} else {
		roller->Set(roller_speed);
	}
	if (false /*floor_switch->Get()*/){
		pid->Disable();
		encoder->Reset();
		pivot->Set(0.0f);
	} else if (false /*top_switch->Get()*/){
		pid->Disable();
		pivot->Set(0.0f);
	} else {
		//pid->Enable();
	}
}
