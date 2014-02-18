#ifndef DIGITALLED_H_
#define DIGITALLED_H_

#include "WPILib.h"

class DigitalLED {
private:
	DigitalOutput * red_channel;
	DigitalOutput * green_channel;
	DigitalOutput * blue_channel;
public:
	//colors are represented as a bitmap
	//first bit is red on/off, second is green, third is blue
	//combination is represented by adding
	static const unsigned int RED = 1;
	static const unsigned int GREEN = 1 << 1;
	static const unsigned int BLUE = 1 << 2;
	static const unsigned int YELLOW = RED + GREEN;
	static const unsigned int MAGENTA = RED + BLUE;
	static const unsigned int CYAN = GREEN + BLUE;
	static const unsigned int WHITE = RED + GREEN + BLUE;
	static const unsigned int OFF = 0;
	DigitalLED(DigitalOutput * red, DigitalOutput * green, DigitalOutput * blue);
	void Set(unsigned int color);
};

#endif
