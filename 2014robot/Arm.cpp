#include "Arm.h"

Arm::Arm(Victor * roller_motor, Victor * pivot_motor, Encoder * enc, DigitalInput * floor, DigitalInput * top, DigitalInput * ball){
	roller = roller_motor;
	pivot = pivot_motor;
	encoder = enc;
	floor_switch = floor;
	top_switch = top;
	ball_switch = ball;
	pid = new PIDController(0.1, 0.0, 0.0, encoder, pivot);
	roller_set = false;
	pivot_set = false;
}

void Arm::run_roller_in() {
	if (ball_switch->Get()){
		roller->Set(1.0f);
	}
}

void Arm::run_roller_out() {
	roller->Set(-1.0f);
	roller_set = true;
}

void Arm::drop_ball_in() {
	roller->Set(1.0f);
	roller_set = true;
}

void Arm::move_up(){
	if (!top_switch->Get()){
		pivot->Set(0.5f);
	}
}

void Arm::move_down(){
	pivot->Set(-0.5f);
}

void Arm::update(){
	if (!roller_set){
		roller->Set(0.0f);
	}
	if (!pivot_set) {
		pivot->Set(0.0f);
	}
	
	/*
	if (floor_switch->Get()){
		pid->Disable();
		encoder->Reset();
		pivot->Set(0.0f);
	} else if (top_switch->Get()){
		pid->Disable();
		pivot->Set(0.0f);
	} else {
		//pid->Enable();
	}
	*/
}


void Arm::set_position(int pos){
	pid->SetSetpoint((pos * TOP_POSITION) / 100);
}
