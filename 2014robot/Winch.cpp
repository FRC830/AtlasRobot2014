#include "Winch.h"
#include <cmath>



Winch::Winch(Victor * motor, Solenoid * sol, Encoder * encoder, DigitalInput * start_pos, DigitalInput * max_pos) {
	winch_motor = motor;
	clutch = sol;
	winch_encoder = encoder;
	zero_pt_lim_switch = start_pos;
	max_lim_switch = max_pos;
	clutch_position = CLUTCH_OUT;
	target_rotations = 5;
}

void Winch::update(){
	//checks to see if winch still needs to be turned and cataput is not at max
	if (winch_encoder->Get() < target_rotations && !max_lim_switch->Get()){
		winch_motor->Set(1.0f);
	} else {
		winch_motor->Set(0.0f);//stop winch
	}	
}

void Winch::wind_back(float angle){
	//once this switch is engaged, that means we can start the encoder
	if (zero_pt_lim_switch->Get()){
		winch_encoder->Reset();
		winch_encoder->Start();
		//only engage clutch when catapult is at rest
		if (clutch_position == CLUTCH_OUT){
			clutch->Set(CLUTCH_IN);
			clutch_position = CLUTCH_IN;
		}
	}
	
	set_target_rotations(computeEcoderStepsFromAngle(angle));

}

void Winch::fire(){
	winch_encoder->Stop();
	clutch->Set(CLUTCH_OUT);
	clutch_position = CLUTCH_OUT;
}

void Winch::set_target_rotations(float target){
	target_rotations = target;
}

float Winch::get_target_rotations(){
	return target_rotations;
}

void Winch::computeAngleFromDistance(float dist){
	float theta = PI - REST_ANGLE;//Matt's equations require the acute angle from horiontal
	
}

float Winch::computeLengthFromAngle(float angle){
	float r = CATAPULT_ARM_LENGTH;
	float x = CATAPULT_X;
	float y = CATAPULT_Y;
	float theta = angle+REST_ANGLE;
	float WINCH_RADIUS = WINCH_CIRCUMFERENCE/(2*PI);
	
	float rest_length = sqrtf( pow((r*cos(REST_ANGLE)+x), 2)  +  pow(r*sin(REST_ANGLE)+y, 2)  - pow(WINCH_RADIUS,2));
	float pulled_length = sqrtf( pow((r*cos(theta)+x), 2)  +  pow(r*sin(theta)+y, 2)  - pow(WINCH_RADIUS, 2));
	
	return(rest_length - pulled_length);//inches
}

float Winch::computeEncoderStepsFromLength(float length){
	float num_pulses = length / WINCH_CIRCUMFERENCE * PULSES_PER_REV;
	return num_pulses;
}

float Winch::computeEcoderStepsFromAngle(float angle){
	return computeEncoderStepsFromLength(computeLengthFromAngle(angle));
}
