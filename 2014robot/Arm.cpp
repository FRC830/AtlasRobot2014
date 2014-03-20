#include "Arm.h"
#include <cmath>

Arm::Arm(Victor * roller_motor, Victor * pivot_motor, Encoder * enc, DigitalInput * floor, DigitalInput * top, DigitalInput * ball){
	roller = roller_motor;
	pivot = pivot_motor;
	encoder = enc;
	encoder->SetPIDSourceParameter(Encoder::kRate); //use the rate of rotation as the pid input
	encoder->SetDistancePerPulse(90.0 / abs(TOP_POSITION - FLOOR_POSITION)); //correspond encoder ticks to degrees
	encoder->Start();
	floor_switch = floor;
	top_switch = top;
	ball_switch = ball;
	pid = new PIDController(0.0, 0.1, 0.0, encoder, pivot);
	roller_set = false;
	pivot_set = false;
}

void Arm::run_roller_in() {
	if (ball_switch->Get() || top_switch->Get()){//linebreak not hit, or arm up
		roller->Set(1.0f);
		roller_set = true;
	}
}

void Arm::run_roller_out() {
	roller->Set(-1.0f);
	roller_set = true;
}

void Arm::drop_ball_in() {//override for run_roller_in function
	roller->Set(1.0f);
	roller_set = true;
}

void Arm::load_sequence() {
	if (!ball_captured()){
		run_roller_in();
	} else if (top_switch->Get()){
		move_up(0.7);
	} else {
		drop_ball_in();
	}
}

void Arm::move_up(float val){
	pid->Disable();
	float speed = -fabs(val);
	if(speed < -0.75){
		speed = -0.75;
	} else if(speed > 0.0){
		speed = 0.0;
	}
	if (!ball_captured()){
		speed *= 0.5;
	}
	pivot->Set(speed);
	/*
	pid->Disable();
	pivot->Set(-.75f);
	if (top_switch->Get()){
		pivot->Set(-0.75f);
	} else {
		pivot->Set(0.0f);
	}*/
	pivot_set = true;
}

void Arm::move_down(){
	pid->Disable();
	pivot->Set(0.5f);
	if (!roller_set){
		roller->Set(1.0f); //move the roller to help prevent the ball from being pulled down
		roller_set = true;
	}
	pivot_set = true;
}

void Arm::move_up_pid(){
	
	if (!top_switch->Get()){
		//pid->SetSetpoint(TOP_POSITION);
		pid->SetSetpoint(-1.0f * MOVEMENT_RATE);
		pid->Enable();
		pivot_set = true;
	}  else {
		pid->Disable();
	}
	
}

void Arm::move_down_pid(){
	
	//pid->SetSetpoint(FLOOR_POSITION);
	pid->SetSetpoint(MOVEMENT_RATE);
	pid->Enable();
	pivot_set = true;
	
}

void Arm::hold_position_pid(){
	pid->SetSetpoint(encoder->Get());
	pid->Enable();
	pivot_set = true;
}

bool Arm::ball_captured(){
	return !ball_switch->Get();
}

void Arm::update(){
	if (!roller_set){
		roller->Set(0.0f);
	}
	if (!pivot_set) {
		pivot->Set(0.0f);
		pid->Disable();
	}
	
	if (top_switch->Get()){
		encoder->Reset();
	}
	
	roller_set = false;
	pivot_set = false;
	
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
	//pid->SetSetpoint((pos * TOP_POSITION) / 100);
}
