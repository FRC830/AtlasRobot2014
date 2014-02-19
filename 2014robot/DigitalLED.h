#ifndef DIGITALLED_H_
#define DIGITALLED_H_

#include "WPILib.h"

class DigitalLED {
private:
	PWM * red_channel;
	PWM * green_channel;
	PWM * blue_channel;
public:
	typedef struct rgb_color_struct {char red, green, blue;} rgb_color;
	
	static const rgb_color RED;
	static const rgb_color GREEN;
	static const rgb_color BLUE;
	static const rgb_color CYAN;
	static const rgb_color YELLOW;
	static const rgb_color MAGENTA;
	static const rgb_color WHITE;
	static const rgb_color OFF;
	DigitalLED(PWM * red, PWM * green, PWM * blue);
	void Set(rgb_color color);
};

#endif
