#ifndef DIGITALLED_H_
#define DIGITALLED_H_

#include "WPILib.h"

class DigitalLED {
private:
	DigitalOutput * red_channel;
	DigitalOutput * green_channel;
	DigitalOutput * blue_channel;
public:
	typedef char rgb_color;
	//colors are represented as a bitmap
	//first bit is red on/off, second is green, third is blue
	//combination is represented by adding
	static const rgb_color RED = 1;
	static const rgb_color GREEN = 1 << 1;
	static const rgb_color BLUE = 1 << 2;
	static const rgb_color YELLOW = RED + GREEN;
	static const rgb_color MAGENTA = RED + BLUE;
	static const rgb_color CYAN = GREEN + BLUE;
	static const rgb_color WHITE = RED + GREEN + BLUE;
	static const rgb_color OFF = 0;
	DigitalLED(DigitalOutput * red, DigitalOutput * green, DigitalOutput * blue);
	void Set(rgb_color color);
};

#endif
