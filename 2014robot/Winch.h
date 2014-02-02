#ifndef WINCH_H_
#define WINCH_H_

#include "WPILib.h"

class Winch {
private:
	Victor * winch_motor;
	Solenoid * clutch;
	bool clutch_position;
	Encoder * winch_encoder;
	static const bool CLUTCH_IN = true; //TODO: determine this
	static const bool CLUTCH_OUT = false;
	static const int MAX_ROTATIONS = 10; //todo: determine this
	
public:
	Winch(Victor * motor, Solenoid * sol, Encoder * encoder);
	void wind_back();
	void fire();
	
};


#endif
