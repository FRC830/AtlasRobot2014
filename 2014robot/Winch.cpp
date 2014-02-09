#include "Winch.h"

Winch::Winch(Victor * motor, Solenoid * sol, Encoder * encoder, DigitalInput * start_pos, DigitalInput * max_pos) {
	winch_motor = motor;
	clutch = sol;
	winch_encoder = encoder;
	zero_pt_lim_switch = start_pos;
	max_lim_switch = max_pos;
	clutch_position = CLUTCH_OUT;
	target_rotations = 5;
}

void Winch::wind_back(){
	if (clutch_position == CLUTCH_OUT){
		clutch->Set(CLUTCH_IN);
		clutch_position = CLUTCH_IN;
	}
	//once this switch is engaged, that means we can start the encoder
	if (zero_pt_lim_switch->Get()){
		winch_encoder->Reset();
		winch_encoder->Start();
	}
	if (winch_encoder->Get() < target_rotations && !max_lim_switch->Get()){
		winch_motor->Set(1.0f);
	} else {
		winch_motor->Set(0.0f);
	}
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
