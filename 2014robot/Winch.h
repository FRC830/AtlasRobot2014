#ifndef WINCH_H_
#define WINCH_H_

#include "WPILib.h"

class Winch {
private:
	Victor * winch_motor;
	Solenoid * clutch;
	bool clutch_position;
	Encoder * winch_encoder;
	DigitalInput * zero_pt_lim_switch;
	DigitalInput * max_lim_switch;
	static const bool CLUTCH_IN = true; //TODO: determine this
	static const bool CLUTCH_OUT = false;
	float target_rotations;
	
public:
	Winch(Victor * motor, Solenoid * sol, Encoder * encoder, DigitalInput * start_pos, DigitalInput * max_pos);
	void set_target_rotations(float n);
	float get_target_rotations();
	void wind_back();
	void fire();
	
};


#endif
