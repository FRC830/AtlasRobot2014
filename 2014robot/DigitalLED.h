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
	
	static const rgb_color RED; //255, 0, 0
	static const rgb_color GREEN; //0, 255, 0
	static const rgb_color BLUE; //0, 0, 255
	static const rgb_color CYAN; //0, 255, 255
	static const rgb_color YELLOW; //255, 255, 0
	static const rgb_color MAGENTA; //255, 0, 255
	static const rgb_color WHITE; //255, 255, 255
	static const rgb_color OFF; //0, 0, 0
	DigitalLED(PWM * red, PWM * green, PWM * blue);
	void Set(rgb_color color);
	void Set(char red, char green, char blue);
};

#endif
