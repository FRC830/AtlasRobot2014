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
<<<<<<< HEAD
	roller_set = false;
=======
>>>>>>> 8953aa1d2f8568eab543727f959ccf431a266b92
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
<<<<<<< HEAD
	if (!ball_captured()){
		run_roller_in();
	} else if (top_switch->Get()){
		move_up();
	} else {
		drop_ball_in();
	}
}

void Arm::move_up(){
	pid->Disable();
	float speed = ball_captured() ? -0.5f : -0.7f;
	
=======
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
>>>>>>> 8953aa1d2f8568eab543727f959ccf431a266b92
	pivot->Set(speed);

	pivot_set = true;
}

void Arm::move_down(){
	pid->Disable();
<<<<<<< HEAD
	pivot->Set(0.4f);
	if (!roller_set){
		roller->Set(1.0f); //move the roller to help prevent the ball from being pulled down
		roller_set = true;
=======
	pivot->Set(0.5f);
	if (ball_captured() && roller_mode == OFF){
		roller_mode = DEPLOY; //move the roller to help prevent the ball from being pulled down
>>>>>>> 8953aa1d2f8568eab543727f959ccf431a266b92
	}
	pivot_set = true;
}

<<<<<<< HEAD
void Arm::move_up_curved() {
	float pos = encoder->Get();
	float speed = 0.0f;
	//speed = (10.0 + pos) * (70.0 - pos) / 1600;
	//speed = (-0.16091489743f) * pow(pos - 30.0f, (1/3)) + 0.5f;
	if (pos > 40){
		speed = 0.7f;
	} else if (pos > 20){
		speed = 0.4f;
	} else {
		speed = 0.2f;
	}
	if (!top_switch->Get()){
		pivot->Set(-speed);
		pivot_set = true;
	}
}

void Arm::move_down_curved(){
	float enc = encoder->Get();
	float speed = (10.0 + enc) * (70.0 - enc) / 1600;
	pivot->Set(speed);
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
=======
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
>>>>>>> 8953aa1d2f8568eab543727f959ccf431a266b92
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
	if (encoder->Get() < TOP_POSITION){
		pid->SetSetpoint(MOVEMENT_RATE);
		pid->Enable();
		pivot_set = true;
	} else {
		pid->Disable();
	}
	
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
	
<<<<<<< HEAD
	if (!top_switch->Get()){
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
=======
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
>>>>>>> 8953aa1d2f8568eab543727f959ccf431a266b92
	}
}


void Arm::set_position(int pos){
	//pid->SetSetpoint((pos * TOP_POSITION) / 100);
}
