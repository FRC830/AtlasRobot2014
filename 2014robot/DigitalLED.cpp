#include "DigitalLED.h"

DigitalLED::DigitalLED(DigitalOutput * red, DigitalOutput * green, DigitalOutput * blue) {
	red_channel = red;
	green_channel = green;
	blue_channel = blue;
}

void DigitalLED::Set(DigitalLED::rgb_color color) {
	Set((color & RED) != 0,(color & GREEN) != 0, (color & BLUE) != 0);
}

void DigitalLED::Set(bool red, bool green, bool blue){
	red_channel->Set(red);
	green_channel->Set(green);
	blue_channel->Set(blue);
}
