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
	pid = new PIDController(0.1, 0.0, 0.0, encoder, pivot);
	roller_set = false;
	pivot_set = false;
	arm_mode = FREE;
	roller_mode = OFF;
}

void Arm::run_roller_in() {
	roller_mode = INTAKE;
}

void Arm::run_roller_out() {
	roller_mode = EJECT;
}

void Arm::drop_ball_in() {//override for run_roller_in function
	roller_mode = DEPLOY;
}

void Arm::load_sequence() {
	switch (arm_mode) {
		case FREE: 
			arm_mode = LOWERING;
		case LOWERING:
			if (at_bottom())
				arm_mode = WAITING_FOR_BALL;
			else
				break;
		case WAITING_FOR_BALL:
			if (ball_captured()) {
				arm_mode = RAISING;
			} else {
				roller_mode = INTAKE;
				break;
			}
		case RAISING:
			if (at_top())
				arm_mode = WAITING_TO_DROP;
			else
				break;
		case WAITING_TO_DROP:
			roller_mode = DEPLOY;
			break;
	}
}

void Arm::end_sequence() {
	arm_mode = FREE;
}

void Arm::move_up(){
	pid->Disable();
	float speed = -0.7f;

	if (!ball_captured()){
		speed = -0.5;
	}
	pivot->Set(speed);

	pivot_set = true;
}

void Arm::move_down(){
	pid->Disable();
	pivot->Set(0.5f);
	if (ball_captured() && roller_mode == OFF){
		roller_mode = DEPLOY; //move the roller to help prevent the ball from being pulled down
	}
	pivot_set = true;
}

void Arm::move_up_curved(){
	move_up_interval();
	pivot_set = true;
}

void Arm::move_down_curved(){
	move_down_interval();
	pivot_set = true;
}

void Arm::move_up_interval(){
	int pos = encoder->Get();
	float speed = 0.0f;
	if (pos > 30){
		speed = -0.7f;
	} else if (pos > 15) {
		speed = -0.3f;
	} else {
		speed = -0.1f;
	}
}

void Arm::move_down_interval(){
	int pos = encoder->Get();
	float speed = 0.0f;
	if (pos < 15) {
		speed = 0.4f;
	} else {
		speed = 0.2f;
	}
}

void Arm::move_up_pid(){
	//pid->SetSetpoint(TOP_POSITION);
	pid->SetSetpoint(-1.0f * MOVEMENT_RATE);
	pid->Enable();
	pivot_set = true;
}

void Arm::move_down_pid(){
	//pid->SetSetpoint(FLOOR_POSITION);
	pid->SetSetpoint(MOVEMENT_RATE);
	pid->Enable();
	pivot_set = true;
	
}

void Arm::hold_position_pid(){
	//pid->SetSetpoint(encoder->Get());
	encoder->SetSetpoint(0.0f);
	pid->Enable();
	pivot_set = true;
}

void Arm::move_to_bottom() {
	arm_mode = LOWERING;
}

void Arm::move_to_top() {
	arm_mode = RAISING;
}

bool Arm::ball_captured(){
	return !ball_switch->Get();
}

bool Arm::at_top() {
	return !top_switch->Get();
}

bool Arm::at_bottom() {
	return encoder->Get() >= FLOOR_POSITION;
}

void Arm::update(){
	switch (roller_mode){
		case OFF: 
			roller->Set(0.0f); break;
		case DEPLOY: 
			roller->Set(1.0f); break;
		case EJECT: 
			roller->Set(-1.0f); break;
		case INTAKE:
			if (ball_switch->Get() || top_switch->Get())
				roller->Set(1.0f);
			else
				roller->Set(0.0f);
			break;
	}
	roller_mode = OFF; //roller must be reset continuously
	
	switch (arm_mode) {
		case FREE:
			if (!pivot_set)
				pivot->Set(0.0f);
			pivot_set = false;
			break;
		case LOWERING:
			if (!at_bottom()) {
				move_down_curved();
			} else {
				pivot->Set(0.0f);
				arm_mode = FREE;
			}
			break;
		case RAISING:
			if (!at_top()){
				move_up_curved();
			} else {
				pivot->Set(0.0f);
				arm_mode = FREE;
			}
			break;
		case WAITING_FOR_BALL:
		case WAITING_TO_DROP:
			pivot->Set(0.0f);
			break;
	}
	
	if (moving_to_bottom){
		if (!pivot_set){
			move_down_curved();
		}
	}
	
	if (!pivot_set) {
		pivot->Set(0.0f);
		pid->Disable();
	}
	
	if (at_top()){
		encoder->Reset();
		pid->Disable();
	}
	
	roller_set = false;
	pivot_set = false;
}


void Arm::set_position(int pos){
	//pid->SetSetpoint((pos * TOP_POSITION) / 100);
}
