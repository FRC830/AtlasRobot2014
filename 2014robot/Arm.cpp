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

//if no ball is captured, lower the arm and roll in until one is.
//once a ball is captured, lift the arm to the top and stay there.
void Arm::load_sequence() {
	switch (arm_mode) {
		case FREE: 
			arm_mode = LOWERING;
		case LOWERING:
			if (at_bottom() || ball_captured())
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
				arm_mode = FREE;
			break;
		default:
			arm_mode = LOWERING;
	}
}

void Arm::override() {
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
		speed = -0.8f;
	} else if (pos > 15){
		speed = -0.6f;
	} else {
		speed = -0.3f;
	}
	pivot->Set(speed);
}

void Arm::move_down_interval(){
	int pos = encoder->Get();
	float speed = 0.0f;
	if (pos < 15) {
		speed = 0.5f;
	} else {
		speed = 0.3f;
	}
	pivot->Set(speed);
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
	pid->SetSetpoint(0.0f);
	pid->Enable();
	pivot_set = true;
}

void Arm::hold_at_top() {
	arm_mode = HOLDING_AT_TOP;
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
			roller->Set(0.3f); break;
		case EJECT: 
			roller->Set(-0.3f); break;
		case INTAKE:
			if (!ball_captured() || at_top())
				roller->Set(0.3f);
			else
				roller->Set(0.0f);
			break;
	}
	roller_mode = OFF; //roller must be reset continuously

	switch (arm_mode) {
		case FREE:
			if (!pivot_set) {
				pivot->Set(0.0f);
				if (at_top())
					arm_mode = HOLDING_AT_TOP;
				if (at_bottom())
					arm_mode = HOLDING_AT_BOTTOM;
			}
			pivot_set = false;
			break;
		case LOWERING:
			if (at_bottom())
				arm_mode = FREE;
		case HOLDING_AT_BOTTOM:
			if (!at_bottom())
				move_down_curved();
			else
				pivot->Set(0.0f);
			break;
		case RAISING:
			if (at_top())
				arm_mode = FREE;
		case HOLDING_AT_TOP:
			if (!at_top())
				move_up_curved();
			else
				pivot->Set(0.0f);
			break;
		default:
			pivot->Set(0.0f);
	}
	if (at_top()){
		encoder->Reset();
	}
}


void Arm::set_position(int pos){
	//pid->SetSetpoint((pos * TOP_POSITION) / 100);
}
