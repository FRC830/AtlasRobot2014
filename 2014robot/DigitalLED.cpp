#include "DigitalLED.h"

const DigitalLED::rgb_color DigitalLED::RED = {255, 0, 0};
const DigitalLED::rgb_color DigitalLED::GREEN = {0, 255, 0};
const DigitalLED::rgb_color DigitalLED::BLUE = {0, 0, 255};
const DigitalLED::rgb_color DigitalLED::CYAN  = {0, 255, 255};
const DigitalLED::rgb_color DigitalLED::YELLOW = {255, 255, 0};
const DigitalLED::rgb_color DigitalLED::MAGENTA = {255, 0, 255};
const DigitalLED::rgb_color DigitalLED::WHITE = {255, 255, 255};
const DigitalLED::rgb_color DigitalLED::OFF = {0, 0, 0};

DigitalLED::DigitalLED(PWM * red, PWM * green, PWM * blue) {
	red_channel = red;
	green_channel = green;
	blue_channel = blue;
}

void DigitalLED::Set(char red, char green, char blue){
	red_channel->SetRaw(red);
	green_channel->SetRaw(green);
	blue_channel->SetRaw(blue);
}

void DigitalLED::Set(rgb_color color){
	Set(color.red, color.green, color.blue);
}
