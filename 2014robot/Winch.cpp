#include "Winch.h"
#include <cmath>

Winch::Winch(Victor * motor, Solenoid * sol, Encoder * encoder, DigitalInput * start_pos, DigitalInput * max_pos) {
	winch_motor = motor;
	clutch = sol;
	winch_encoder = encoder;
	zero_pt_lim_switch = start_pos;
	max_lim_switch = max_pos;
	clutch_position = CLUTCH_OUT;
	winding_back = false;
	firing = false;
	timer = new Timer();
	
	target_rotations = 5;
}

void Winch::update(){
	if (winding_back){
		if (max_lim_switch->Get() && timer->Get() < 3.0){
			winch_motor->Set(-0.7f);
		} else {
			winding_back = false; //stop winding back if we've hit the switch
			timer->Stop();
			timer->Reset();
		}
	}
	
	if (!winding_back) {
		winch_motor->Set(0.0f);
	}
	
	if (!firing){
		clutch->Set(CLUTCH_IN);
	} else {
		firing = false; //so we can push the clutch back in when we stop firing
	}
	
	//checks to see if winch still needs to be turned and cataput is not at max
	/*
	if (winch_encoder->Get() < target_rotations && !max_lim_switch->Get()){
		winch_motor->Set(1.0f);
	} else {
		winch_motor->Set(0.0f);//stop winch
		
	}	
	*/
}

void Winch::wind_back() {
	winding_back = true;
	timer->Start();
}

void Winch::fire(){
	if (!max_lim_switch->Get()){
		clutch->Set(CLUTCH_OUT);
		firing = true;
	}
	
	/*
	set_target_rotations(0.0f);	
	winch_encoder->Stop();
	winch_encoder->Reset();
	*/
}

void Winch::wind_back_dist(float dist){	
	//winds back given a distance from the goal
	float angle = computeAngleFromDistance(dist);
	wind_back_rotations(computeEncoderStepsFromAngle(angle));
}

void Winch::wind_back_rotations(float n_rotations){
	//once this switch is engaged, that means we can start the encoder
	if (zero_pt_lim_switch->Get()){
		winch_encoder->Start();
	}
	//only engage clutch when catapult is at rest
	if (clutch_position == CLUTCH_OUT){
		clutch->Set(CLUTCH_IN);
		clutch_position = CLUTCH_IN;
	}
	
	set_target_rotations(n_rotations);
}

void Winch::set_target_rotations(float target){
	target_rotations = target;
}

float Winch::get_target_rotations(){
	return target_rotations;
}

float Winch::computeAngleFromDistance(float dist){
	float x = (dist)/3.28;//take input in feet, change to meters
	
	float theta = PI - REST_ANGLE;//Matt's equations require the acute angle from horiontal
	float a=GRAVITY;
	float k = SPRING_CONST*NUM_SPRINGS;
	float Mb = BALL_MASS;
	float Mc = CATAPULT_MASS;
	float h = BALL_HEIGHT;
	float y = GOAL_HEIGHT;
	
	float v = sqrtf(((0.5*a*pow(x, 2)) * tan(theta) * tan(theta+1)) / (y - h - (x*tan(theta))));
	
	float e = 0.5*(Mb + Mc) * pow(v, 2);
	
	float pull_back_angle = sqrtf(2*e / k);
	return pull_back_angle;
}

float Winch::computeLengthFromAngle(float angle){
	//the ammount of rope pulled in by the winch
	float r = CATAPULT_ARM_LENGTH;
	float x = CATAPULT_X;
	float y = CATAPULT_Y;
	float theta = angle+REST_ANGLE;
	float winch_radius = WINCH_CIRCUMFERENCE / (2 * PI);
	float rest_length = sqrtf( pow((r*cos(REST_ANGLE)+x), 2)  +  pow(r*sin(REST_ANGLE)+y, 2)  - pow(winch_radius,2));
	float pulled_length = sqrtf( pow((r*cos(theta)+x), 2)  +  pow(r*sin(theta)+y, 2)  - pow(winch_radius, 2));
	
	return(rest_length - pulled_length);//inches
}

float Winch::computeEncoderStepsFromLength(float length){
	float num_pulses = length / WINCH_CIRCUMFERENCE * PULSES_PER_REV;
	return num_pulses;
}

float Winch::computeEncoderStepsFromAngle(float angle){
	return computeEncoderStepsFromLength(computeLengthFromAngle(angle));
}
