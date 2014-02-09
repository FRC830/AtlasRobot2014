#include "WPILib.h"
#include "Gamepad.h"
#include "Winch.h"
#include "Arm.h"
#include <cmath>

class AerialAssistRobot : public IterativeRobot
{
	
	static const int ROLLER_PWM = 3;
	static const int ARM_LIFT_PWM = 4;
	static const int LEFT_DRIVE_PWM = 10;
	static const int RIGHT_DRIVE_PWM = 1;
	static const int WINCH_PWM = 5;
	
	static const int PRESSURE_SWITCH_DIO = 1;
	static const int COMPRESSOR_RELAY_DIO = 2;
	static const int WINCH_MAX_LIMIT_DIO = 3;
	static const int WINCH_ZERO_POINT_DIO = 4;
	
	static const int ARM_FLOOR_SWITCH_DIO = 5;
	static const int ARM_LOW_SWITCH_DIO = 6;
	static const int ARM_TOP_SWITCH_DIO = 7;
	static const int ARM_BALL_SWITCH_DIO = 8;
	
	static const int ARM_ENCODER_A_CHANNEL = 5;
	static const int ARM_ENCODER_B_CHANNEL = 6;
	static const int WINCH_ENCODER_A_CHANNEL = 3;
	static const int WINCH_ENCODER_B_CHANNEL = 4;
	
	static const int GEAR_SHIFT_SOL = 1;
	static const int CLUTCH_SOL = 2;
	static const bool HIGH_GEAR = true; //TODO: determine which of these is which
	static const bool LOW_GEAR = false;
	static const bool CLUTCH_IN = true;
	static const bool CLUTCH_OUT = false;
	
    static const float MAX_ACCEL_TIME = 0.5f;        //how many seconds we want it to take to get to max speed
    float max_delta_speed;
    //max_delta_speed = 1.0 / (MAX_ACCEL_TIME * GetLoopsPerSec)
	
    float old_turn, old_forward;
    
	RobotDrive * drive;
	
	
	DigitalInput * arm_floor;
	DigitalInput * arm_low_goal;
	DigitalInput * arm_top;
	DigitalInput * arm_ball;
	Victor * roller;
	Victor * arm_lift;
	Encoder * arm_encoder;
	
	Arm * arm;
	
	Victor * winch_motor;
	Encoder * winch_encoder;
	Winch * winch;
	DigitalInput * winch_max_switch;
	DigitalInput * winch_zero_switch;
	
	Solenoid * gear_shift;
	Solenoid * clutch;
	Compressor * compressor;
	
	Gamepad * pilot;
	Gamepad * copilot;
	
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
		arm_encoder = new Encoder(ARM_ENCODER_A_CHANNEL, ARM_ENCODER_B_CHANNEL);
		arm_floor = new DigitalInput(ARM_FLOOR_SWITCH_DIO);
		arm_low_goal = new DigitalInput(ARM_LOW_SWITCH_DIO);
		arm_top = new DigitalInput(ARM_TOP_SWITCH_DIO);
		arm_ball = new DigitalInput(ARM_BALL_SWITCH_DIO);
		
		arm = new Arm(roller, arm_lift, arm_floor, arm_low_goal, arm_top, arm_ball);
		
		winch_motor = new Victor(WINCH_PWM);
		winch_encoder = new Encoder(WINCH_ENCODER_A_CHANNEL, WINCH_ENCODER_B_CHANNEL);
		winch_max_switch = new DigitalInput (WINCH_MAX_LIMIT_DIO);
		winch_zero_switch = new DigitalInput (WINCH_ZERO_POINT_DIO);
		
		gear_shift = new Solenoid(GEAR_SHIFT_SOL);
		clutch = new Solenoid(CLUTCH_SOL);
		
		winch = new Winch(winch_motor, clutch, winch_encoder, winch_zero_switch, winch_max_switch);
		
		compressor = new Compressor(PRESSURE_SWITCH_DIO, COMPRESSOR_RELAY_DIO);
		
		lcd = DriverStationLCD::GetInstance();
		
		pilot = new Gamepad(1);
		copilot = new Gamepad(2);
	}
	
	void DisabledInit(void) {
	}

	void AutonomousInit(void) {
	}

	void TeleopInit(void) {
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
		if (copilot->GetNumberedButton(5)) {
			arm->set_roller(0.4f);
		} else if (copilot->GetNumberedButton(6)){
			arm->set_roller(-0.4f);
		} else {
			arm->set_roller(0.0f);
		}
		
		if (copilot->GetNumberedButton(1)){
			arm->set_position(Arm::floor);
		} else if (copilot->GetNumberedButton(2)){
			arm->set_position(Arm::low_goal);
		} else if (copilot->GetNumberedButton(4)){
			arm->set_position(Arm::top);
		}
		
		float arm_move = copilot->GetLeftY();
		if (abs_float(arm_move) > 0.2f){
			arm->set_position(Arm::none);
			arm_lift->Set(arm_move);
		}
		
		//prime shooter to fire
		if (copilot->GetRightY() > 0.3f){
			winch->wind_back();
		}
		
		if (copilot->GetNumberedButton(5)){
			winch->fire();
		}
		
		arm->update();
		
		lcd->PrintfLine(DriverStationLCD::kUser_Line1, "teleop");
		lcd->UpdateLCD();
		
		
	}


};

START_ROBOT_CLASS(AerialAssistRobot);
