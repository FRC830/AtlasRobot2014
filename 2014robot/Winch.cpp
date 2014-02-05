#include "Winch.h"

Winch::Winch(Victor * motor, Solenoid * sol, Encoder * encoder) {
	winch_motor = motor;
	clutch = sol;
	winch_encoder = encoder;
	clutch_position = CLUTCH_OUT;
}

void Winch::wind_back(){
	if (clutch_position == CLUTCH_OUT){
		clutch->Set(CLUTCH_IN);
		clutch_position = CLUTCH_IN;
		winch_encoder->Reset();
		winch_encoder->Start();
	}
	if (winch_encoder->Get() < MAX_ROTATIONS){
		winch_motor->Set(1.0f);
	}
}

void Winch::fire(){
	winch_encoder->Stop();
	clutch->Set(CLUTCH_OUT);
	clutch_position = CLUTCH_OUT;
}
