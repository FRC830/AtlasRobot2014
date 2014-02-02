#include "WPILib.h"
#include "Gamepad.h"
#include <cmath>

class AerialAssistRobot : public IterativeRobot
{
	static const int ROLLER_PWM = 3;
	static const int ARM_LIFT_PWM = 4;
	static const int LEFT_DRIVE_PWM = 1;
	static const int RIGHT_DRIVE_PWM = 2;
	static const int WINCH_PWM = 5;
	
	static const int PRESSURE_SWITCH_DIO = 1;
	static const int COMPRESSOR_RELAY_DIO = 2;
	
	static const int GEAR_SHIFT_SOL = 1;
	static const int CLUTCH_SOL = 2;
	static const bool HIGH_GEAR = true; //TODO: determine which of these is which
	static const bool LOW_GEAR = false;
	static const bool CLUTCH_IN = true; //TODO: ditto
	static const bool CLUTCH_OUT = false;
	
    static const float MAX_ACCEL_TIME = 0.5f;        //how many seconds we want it to take to get to max speed
    float max_delta_speed;
    //max_delta_speed = 1 / (time-to-get-to-maximum-speed * cycles-in-one-second)
	
    float old_turn, old_forward;
    
	RobotDrive * drive;
	
	Victor * roller;
	Victor * arm_lift;
	Victor * winch;
	
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
			return input;
		} else {
			return 0.0f;
		}
	}
	
	
public:
	AerialAssistRobot(void)	{
		drive = new RobotDrive(new Victor(LEFT_DRIVE_PWM), new Victor (RIGHT_DRIVE_PWM));
		//drive->SetInvertedMotor(RobotDrive::kFrontLeftMotor, false);
		//drive->SetInvertedMotor(RobotDrive::kRearLeftMotor, false);
		//drive->SetInvertedMotor(RobotDrive::kFrontRightMotor, false);
		//drive->SetInvertedMotor(RobotDrive::kRearRightMotor, false);
		
		roller = new Victor(ROLLER_PWM);
		arm_lift = new Victor(ARM_LIFT_PWM);
		winch = new Victor(WINCH_PWM);
		
		gear_shift = new Solenoid(GEAR_SHIFT_SOL);
		clutch = new Solenoid(CLUTCH_SOL);
		
		compressor = new Compressor(PRESSURE_SWITCH_DIO, COMPRESSOR_RELAY_DIO);
		
		lcd = DriverStationLCD::GetInstance();
		
		pilot = new Gamepad(1);
		copilot = new Gamepad(2);
	}
	
	
	/********************************** Init Routines *************************************/

	void RobotInit(void) {
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
		float forward = -pilot->GetLeftY();
		forward = clamp(forward, 0.05f);
		float turn = pilot->GetRightX();
		turn = clamp(turn, 0.05f);
		
		max_delta_speed = 1.0f / (MAX_ACCEL_TIME * GetLoopsPerSec());
        //float delta_turn = turn - old_turn;
        float delta_forward = forward - old_forward;
        /*
        if (delta_turn > max_delta_speed && delta_turn > 0.0f){
        	turn = old_turn + max_delta_speed;
        } else if (delta_turn < -max_delta_speed && delta_turn < 0.0f){
			turn = old_turn - max_delta_speed;
        } 
        */
         //if neither of these is the case, |delta_turn| is less than the max 
         //and we can just use the turn without modification.
         
         if (delta_forward > max_delta_speed && delta_forward > 0.0f){
			forward = old_forward + max_delta_speed;
         } else if (delta_forward < -max_delta_speed && delta_forward < 0.0f){
			forward = old_forward - max_delta_speed;
         }
         
         lcd->PrintfLine(DriverStationLCD::kUser_Line2, "%f %f", forward, turn);
         
         drive->ArcadeDrive(turn, forward);
         old_turn = turn;
         old_forward = forward;
		
		
		//move the arm up or down
		float arm_move = copilot->GetRightY();
		arm_move = clamp(arm_move, 0.1f);
		arm_lift->Set(arm_move);

		//spin the roller forward or back
		if (copilot->GetNumberedButton(5)) {
			roller->Set(0.4f);
		} else if (copilot->GetNumberedButton(6)){
			roller->Set(-0.4f);
		} else {
			roller->Set(0.0f);
		}
		
		//prime shooter to fire
		if (copilot->GetNumberedButton(3)){
			winch->Set(1.0f); //TODO: determine proper direction, power 
		} else {
			winch->Set(0.0f);
		}
		
		if (copilot->GetNumberedButton(5)){
			clutch->Set(CLUTCH_OUT);	//fire the shooter 
		} else if (copilot->GetNumberedButton(7)){
			clutch->Set(CLUTCH_IN);
		}
		
		if (pilot->GetNumberedButton(5) || pilot->GetNumberedButton(6)){
			gear_shift->Set(HIGH_GEAR);
		} else if (pilot->GetNumberedButton(7) || pilot->GetNumberedButton(8)){
			gear_shift->Set(LOW_GEAR);
		}
		
		lcd->PrintfLine(DriverStationLCD::kUser_Line1, "teleop");
		lcd->UpdateLCD();
	}

			
};

START_ROBOT_CLASS(AerialAssistRobot);
