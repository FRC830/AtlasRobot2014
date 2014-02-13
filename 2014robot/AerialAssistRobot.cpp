#include "WPILib.h"
#include "Gamepad.h"
#include "Winch.h"
#include "Arm.h"
#include "Rangefinder.h"
#include <cmath>

class AerialAssistRobot : public IterativeRobot
{
	//PWM pins
	static const int ROLLER_PWM = 3;
	static const int ARM_LIFT_PWM = 4;
	static const int LEFT_DRIVE_PWM = 10;
	static const int RIGHT_DRIVE_PWM = 1;
	static const int WINCH_PWM = 5;
	
	
	//Digital IO pins
	static const int PRESSURE_SWITCH_DIO = 1;
	static const int COMPRESSOR_RELAY = 1;
	static const int WINCH_MAX_LIMIT_DIO = 3;
	static const int WINCH_ZERO_POINT_DIO = 4;
	
	static const int ARM_FLOOR_SWITCH_DIO = 5;
	static const int ARM_TOP_SWITCH_DIO = 7;
	static const int ARM_BALL_SWITCH_DIO = 8;
	
	static const int ARM_ENCODER_A_CHANNEL = 9;
	static const int ARM_ENCODER_B_CHANNEL = 10;
	
	static const int WINCH_ENCODER_A_CHANNEL = 13;
	static const int WINCH_ENCODER_B_CHANNEL = 14;
	
	//Solenoids
	static const int GEAR_SHIFT_SOL = 1;
	static const int CLUTCH_SOL = 2;
	static const bool HIGH_GEAR = true; //TODO: determine which of these is which
	static const bool LOW_GEAR = false;
	static const bool CLUTCH_IN = true;
	static const bool CLUTCH_OUT = false;
	
    static const float MAX_ACCEL_TIME = 0.5f;        //how many seconds we want it to take to get to max speed
    float max_delta_speed;
    //max_delta_speed = 1.0 / (MAX_ACCEL_TIME * GetLoopsPerSec)
	
    static const int RANGE_FINDER_PING_CHANNEL_L_DIO = 9;
    static const int RANGE_FINDER_ECHO_CHANNEL_L_DIO = 10;
    static const int RANGE_FINDER_PING_CHANNEL_R_DIO = 11;
    static const int RANGE_FINDER_ECHO_CHANNEL_R_DIO = 12;
	
    float old_turn, old_forward;
    
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
	DigitalInput * winch_zero_switch;
	
	Solenoid * gear_shift;
	Solenoid * clutch;
	Compressor * compressor;
	
	Ultrasonic * us_l;
	Ultrasonic * us_r;
	Rangefinder * rangefinder;
	
	Gamepad * pilot;
	Gamepad * copilot;
	
	//AxisCamera * camera;
	
	DriverStationLCD * lcd;
	
	
	
	
	
    float abs_float(float input){
            return (input < 0.0f) ? -input : input;
    }
    
	float clamp(float input, float min){
		if (abs_float(input) < min){
			return 0.0f;
		} else {
			return input;
		}
	}
	
	
public:
	AerialAssistRobot(void)	{

	}
	
	
	/********************************** Init Routines *************************************/

	void RobotInit(void) {
		
		drive = new RobotDrive(new Victor(LEFT_DRIVE_PWM), new Victor (RIGHT_DRIVE_PWM));
		drive->SetInvertedMotor(RobotDrive::kFrontLeftMotor, true);
		drive->SetInvertedMotor(RobotDrive::kRearLeftMotor, true);
		drive->SetInvertedMotor(RobotDrive::kFrontRightMotor, false);
		drive->SetInvertedMotor(RobotDrive::kRearRightMotor, false);
		
		roller = new Victor(ROLLER_PWM);
		arm_lift = new Victor(ARM_LIFT_PWM);
		arm_floor = new DigitalInput(ARM_FLOOR_SWITCH_DIO);
		arm_top = new DigitalInput(ARM_TOP_SWITCH_DIO);
		arm_ball = new DigitalInput(ARM_BALL_SWITCH_DIO);
		arm_encoder = new Encoder(ARM_ENCODER_A_CHANNEL, ARM_ENCODER_B_CHANNEL);
		
		arm = new Arm(roller, arm_lift, arm_encoder, arm_floor, arm_top, arm_ball);
		
		winch_motor = new Victor(WINCH_PWM);
		winch_encoder = new Encoder(WINCH_ENCODER_A_CHANNEL, WINCH_ENCODER_B_CHANNEL);
		winch_max_switch = new DigitalInput (WINCH_MAX_LIMIT_DIO);
		winch_zero_switch = new DigitalInput (WINCH_ZERO_POINT_DIO);
		
		gear_shift = new Solenoid(GEAR_SHIFT_SOL);
		clutch = new Solenoid(CLUTCH_SOL);
		
		winch = new Winch(winch_motor, clutch, winch_encoder, winch_zero_switch, winch_max_switch);
		
		us_l = new Ultrasonic(RANGE_FINDER_PING_CHANNEL_L_DIO, RANGE_FINDER_ECHO_CHANNEL_L_DIO);
		us_r = new Ultrasonic(RANGE_FINDER_PING_CHANNEL_R_DIO, RANGE_FINDER_ECHO_CHANNEL_R_DIO);
		rangefinder = new Rangefinder(us_l, us_r);
		
		compressor = new Compressor(PRESSURE_SWITCH_DIO, COMPRESSOR_RELAY_DIO);
		
		lcd = DriverStationLCD::GetInstance();
		
		//camera = &AxisCamera::GetInstance();
		
		pilot = new Gamepad(1);
		copilot = new Gamepad(2);
		
	}
	
	void DisabledInit(void) {
	}

	void AutonomousInit(void) {
	}

	void TeleopInit(void) {
		winch_rotations = Winch::STANDARD_ROTATIONS_TARGET;
		winch_rot_adjusted = false;
		winch_encoder->Reset();
	}

	/********************************** Periodic Routines *************************************/
	
	void DisabledPeriodic(void)  {
		lcd->PrintfLine(DriverStationLCD::kUser_Line1, "disabled");
		lcd->UpdateLCD();
	}

	void AutonomousPeriodic(void) {
	}

	
	void TeleopPeriodic(void) {
		
		
		//standard arcade drive using left and right sticks
		//clamp the values so small inputs are ignored
		float speed = -pilot->GetLeftY();
		speed = clamp(speed, 0.05f);
		float turn = pilot->GetRightX();
		turn = clamp(turn, 0.05f);
		
        lcd->PrintfLine(DriverStationLCD::kUser_Line2, "%f %f", speed, turn);
        
		max_delta_speed = 1.0f / (MAX_ACCEL_TIME * GetLoopsPerSec()); //1.0 represents the maximum victor input
        //float delta_turn = turn - old_turn;
        float delta_speed = speed - old_forward;
        
        if (delta_speed > max_delta_speed){
			speed = old_forward + max_delta_speed;
        } else if (delta_speed < -max_delta_speed){
			speed = old_forward - max_delta_speed;
        }
        //if neither of these is the case, |delta_speed| is less than the max 
        //and we can just use the given value without modification.
         
        lcd->PrintfLine(DriverStationLCD::kUser_Line3, "%f %f", speed, turn);
         
        drive->ArcadeDrive(-turn, speed / 1.5); //turn needs to be reversed
        old_turn = turn;
        old_forward = speed;
		
 		if (pilot->GetNumberedButton(5) || pilot->GetNumberedButton(6)){
 			gear_shift->Set(HIGH_GEAR);
 		} else if (pilot->GetNumberedButton(7) || pilot->GetNumberedButton(8)){
 			gear_shift->Set(LOW_GEAR);
 		}

		//spin the roller forward or back
		if (copilot->GetNumberedButton(Gamepad::F310_Y)) {
			arm->set_roller(0.4f);
		} else if (copilot->GetNumberedButton(Gamepad::F310_A)){
			arm->set_roller(-0.4f);
		} else {
			arm->set_roller(0.0f);
		}
		
		float dpad = copilot->GetRawAxis(Gamepad::F310_DPAD_X_AXIS);
		if (dpad < -0.5f){
			arm->set_position(Arm::FLOOR_POSITION);
		} else if (dpad > 0.5f){
			arm->set_position(Arm::TOP_POSITION);
		} else if (false) {		//we don't have a control for this right now
			arm->set_position(Arm::LOW_GOAL_POSITION);
		} else {
			float left_y = -copilot->GetRawAxis(Gamepad::F310_LEFT_Y);
			if (abs_float(left_y) > 0.1){
				arm_lift->Set(left_y);
			}
		}
		
		//prime shooter to fire
		
		float right_y = -copilot->GetRawAxis(Gamepad::F310_RIGHT_Y);
		if (right_y > 0.3f && !winch_rot_adjusted){
			winch_rotations += Winch::STANDARD_ROTATIONS_INCREMENT;
			winch_rot_adjusted = true;
		} else if (right_y < -0.3f && !winch_rot_adjusted){
			winch_rotations -= Winch::STANDARD_ROTATIONS_INCREMENT;
		} else if (abs_float(right_y) <= 0.3 ){
			winch_rot_adjusted = false;
		}
		
		if (copilot->GetNumberedButton(Gamepad::F310_R_STICK)){
			winch_encoder->Start();
			winch->wind_back(winch_rotations);
		}
		
		if (copilot->GetNumberedButton(Gamepad::RIGHT_BUMPER)){
			winch->fire();
		}
		
		//lcd->PrintfLine(DriverStationLCD::kUser_Line4, "%f", us_l->GetRangeInches());
		//lcd->PrintfLine(DriverStationLCD::kUser_Line5, "%f", us_r->GetRangeInches());
		
		arm->update();
		if (copilot->GetNumberedButton(Gamepad::F310_B) && !winch_max_switch->Get()){
			winch_motor->Set(1.0);
		} else {
			winch->update();
		}
		
		lcd->PrintfLine(DriverStationLCD::kUser_Line5, "%f", winch_encoder->Get());
		lcd->PrintfLine(DriverStationLCD::kUser_Line6, "%f", winch->get_target_rotations());
		//Compressor on button 10
		if (pilot->GetNumberedButton(10)){
			compressor->Start();
		} else {
			compressor->Stop();
		}
		
		//camera->GetImage();
		
		lcd->PrintfLine(DriverStationLCD::kUser_Line1, "teleop");
		lcd->UpdateLCD();
		
	}
};

START_ROBOT_CLASS(AerialAssistRobot);
