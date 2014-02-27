#include "DigitalLED.h"

DigitalLED::DigitalLED(DigitalOutput * red, DigitalOutput * green, DigitalOutput * blue) {
	red_channel = red;
	green_channel = green;
	blue_channel = blue;
}

void DigitalLED::Set(DigitalLED::rgb_color color) {
	red_channel->Set(color & RED);
	green_channel->Set(color & GREEN);
	blue_channel->Set(color & BLUE);
}
