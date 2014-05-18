#ifndef AERIAL_ASSIST_ROBOT_H
#define AERIAL_ASSIST_ROBOT_H

#include "Gamepad.h"
#include "Winch.h"
#include "Arm.h"
#include "Rangefinder.h"
#include "DigitalLED.h"
#include <cmath>

class AerialAssistRobot : public IterativeRobot
{
private:
	//PWM pins
	static const int ROLLER_PWM = 1;
	static const int ARM_LIFT_PWM  = 2;
	static const int FRONT_LEFT_DRIVE_PWM = 3;
	static const int REAR_LEFT_DRIVE_PWM = 4;
	static const int FRONT_RIGHT_DRIVE_PWM = 7;
	static const int REAR_RIGHT_DRIVE_PWM = 8;
	static const int WINCH_PWM = 9;
	//relay
	static const int COMPRESSOR_RELAY = 1;
	
	//Digital IO pins
	static const int PRESSURE_SWITCH_DIO = 6;
	static const int WINCH_MAX_LIMIT_DIO = 4;
	
	static const int ARM_FLOOR_SWITCH_DIO = 5; //not used
	static const int ARM_TOP_SWITCH_DIO = 7;
	static const int ARM_LINE_BREAK_DIO = 9;
 	
	static const int ARM_ENCODER_A_CHANNEL = 1;
	static const int ARM_ENCODER_B_CHANNEL = 2;
	
    static const int RANGE_FINDER_PING_CHANNEL_DIO = 13;
    static const int RANGE_FINDER_ECHO_CHANNEL_DIO = 14;
    
    static const int RED_LED_DIO = 11;
    static const int GREEN_LED_DIO = 10;
    static const int BLUE_LED_DIO = 12;
    
	static const int WINCH_ENCODER_A_CHANNEL = 5; //not used
	static const int WINCH_ENCODER_B_CHANNEL = 8; //not used
	
	//Solenoids
	static const int GEAR_SHIFT_SOL_FORWARD = 8;
	static const int GEAR_SHIFT_SOL_REVERSE = 1;
	static const int CLUTCH_SOL = 2;
	static const DoubleSolenoid::Value LOW_GEAR = DoubleSolenoid::kForward;
	static const DoubleSolenoid::Value HIGH_GEAR = DoubleSolenoid::kReverse;
	static const bool CLUTCH_IN = true;
	static const bool CLUTCH_OUT = false;
	
    static const float MAX_ACCEL_TIME = 0.3f;        //how many seconds we want it to take to get to max speed
    float max_delta_speed;
    //max_delta_speed = 1.0 / (MAX_ACCEL_TIME * GetLoopsPerSec)
    
    static const float FIRING_DISTANCE = 180.0f; //TODO: determine this for real
	
	
    float old_turn, old_speed;
    
    Talon * front_left;
    Talon * front_right;
    Talon * rear_left;
    Talon * rear_right;
	RobotDrive * drive;
	
	DigitalInput * arm_floor;
	DigitalInput * arm_top;
	DigitalInput * arm_ball;
	Victor * roller;
	Victor * arm_lift;
	Encoder * arm_encoder;
	
	Arm * arm;
	
	Victor * winch_motor;
	Encoder * winch_encoder;
	Winch * winch;
	float winch_rotations;
	bool winch_rot_adjusted;
	DigitalInput * winch_max_switch;
	
	DigitalOutput * led_red_channel;
	DigitalOutput * led_green_channel;
	DigitalOutput * led_blue_channel;
	DigitalLED * led;
	DigitalLED::rgb_color alliance_color;
	
	DoubleSolenoid * gear_shift;
	Solenoid * clutch;
	
	//DigitalInput * pressure_switch;
	Compressor * compressor;
	
	Ultrasonic * ultrasonic;
	Rangefinder * rangefinder;
	
	Timer * timer;
	
	Gamepad * pilot;
	Gamepad * copilot;
	
	//AxisCamera * camera;
	
	DriverStationLCD * lcd;
	DriverStation * ds;
	
	bool firing;
	
	bool red, green, blue; //for led testing control

	inline float clamp(float input, float min){ return fabs(input) < fabs(min) ? 0.0f : input; }
	
	void AutonomousDriveForwardInit(void);
	void AutonomousTwoBallInit(void);
	void AutonomousMainInit(void);
	void SafetyTestInit(void);
	void ColorTestInit(void);
	
	void AutonomousMainPeriodic(void);
	void AutonomousTwoBallPeriodic(void);
	void AutonomousDriveForwardPeriodic(void);
	void SafetyTestPeriodic(void);
	void ColorTestPeriodic(void);
	
public:
	AerialAssistRobot(void);
	void RobotInit(void);
	
	void DisabledInit(void);
	void AutonomousInit(void) { AutonomousTwoBallInit(); }
	void TeleopInit(void);
	void TestInit(void);
	
	void DisabledPeriodic(void);
	void AutonomousPeriodic(void) { AutonomousTwoBallPeriodic(); }
	void TeleopPeriodic(void);
	void TestPeriodic(void);
};

START_ROBOT_CLASS(AerialAssistRobot);

#endif
