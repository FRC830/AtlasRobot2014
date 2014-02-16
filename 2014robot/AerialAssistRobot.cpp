#include "WPILib.h"
#include "Gamepad.h"
#include "Winch.h"
#include "Arm.h"
#include "Rangefinder.h"
#include <cmath>

class AerialAssistRobot : public IterativeRobot
{
	//PWM pins
	static const int ROLLER_PWM = 4; //4
	static const int ARM_LIFT_PWM  = 3;
	static const int LEFT_DRIVE_PWM = 8; //8
	static const int RIGHT_DRIVE_PWM = 1; //1
	static const int WINCH_PWM = 9;
	
	//relay
	static const int COMPRESSOR_RELAY = 1;
	
	//Digital IO pins
	static const int PRESSURE_SWITCH_DIO = 2;
	static const int WINCH_MAX_LIMIT_DIO = 5;
	static const int WINCH_ZERO_POINT_DIO = 8;
	
	static const int ARM_FLOOR_SWITCH_DIO = 4;
	static const int ARM_TOP_SWITCH_DIO = 7;
	static const int ARM_BALL_SWITCH_DIO = 3;
	
	static const int ARM_ENCODER_A_CHANNEL = 9;
	static const int ARM_ENCODER_B_CHANNEL = 10;
	
    static const int RANGE_FINDER_PING_CHANNEL_L_DIO = 11;
    static const int RANGE_FINDER_ECHO_CHANNEL_L_DIO = 12;
    //static const int RANGE_FINDER_PING_CHANNEL_R_DIO = 11;
    //static const int RANGE_FINDER_ECHO_CHANNEL_R_DIO = 12;
	
	static const int WINCH_ENCODER_A_CHANNEL = 13;
	static const int WINCH_ENCODER_B_CHANNEL = 14;
	
	//Solenoids
	static const int GEAR_SHIFT_SOL_FORWARD = 8;
	static const int GEAR_SHIFT_SOL_REVERSE = 1;
	static const int CLUTCH_SOL = 2;
	static const DoubleSolenoid::Value HIGH_GEAR = DoubleSolenoid::kForward;
	static const DoubleSolenoid::Value LOW_GEAR = DoubleSolenoid::kReverse;
	static const bool CLUTCH_IN = false;
	static const bool CLUTCH_OUT = true;
	
    static const float MAX_ACCEL_TIME = 0.3f;        //how many seconds we want it to take to get to max speed
    float max_delta_speed;
    //max_delta_speed = 1.0 / (MAX_ACCEL_TIME * GetLoopsPerSec)
	
	
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
	
	
	DoubleSolenoid * gear_shift;
	Solenoid * clutch;
	
	//DigitalInput * pressure_switch;
	Compressor * compressor;
	
	Ultrasonic * us_l;
	Ultrasonic * us_r;
	Rangefinder * rangefinder;
	
	Timer * timer;
	
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
		
		gear_shift = new DoubleSolenoid(GEAR_SHIFT_SOL_FORWARD, GEAR_SHIFT_SOL_REVERSE);
		clutch = new Solenoid(CLUTCH_SOL);
		
		winch = new Winch(winch_motor, clutch, winch_encoder, winch_zero_switch, winch_max_switch);
		
		us_l = new Ultrasonic(RANGE_FINDER_PING_CHANNEL_L_DIO, RANGE_FINDER_ECHO_CHANNEL_L_DIO);
		//us_r = new Ultrasonic(RANGE_FINDER_PING_CHANNEL_R_DIO, RANGE_FINDER_ECHO_CHANNEL_R_DIO);
		//rangefinder = new Rangefinder(us_l, us_r);
		
		//pressure_switch = new DigitalInput(PRESSURE_SWITCH_DIO);
		compressor = new Compressor(PRESSURE_SWITCH_DIO, COMPRESSOR_RELAY);
		compressor->Start();
		
		lcd = DriverStationLCD::GetInstance();
		
		//camera = &AxisCamera::GetInstance();
		
		timer = new Timer();
		
		pilot = new Gamepad(1);
		copilot = new Gamepad(2);
		
	}
	
	void DisabledInit(void) {
	}

	void AutonomousInit(void) {
		clutch->Set(CLUTCH_IN);
		gear_shift->Set(LOW_GEAR);
		timer->Reset();
		timer->Start();
		compressor->Start(); //required by rules
	}

	void TeleopInit(void) {
		compressor->Start(); //required by rules
		
		/*
		winch_rotations = Winch::STANDARD_ROTATIONS_TARGET;
		winch_rot_adjusted = false;
		winch_encoder->Reset();
		*/
	}

	/********************************** Periodic Routines *************************************/
	
	void DisabledPeriodic(void)  {
		lcd->PrintfLine(DriverStationLCD::kUser_Line1, "disabled");
		lcd->UpdateLCD();
	}

	void AutonomousPeriodic(void) {
		if (timer->Get() < 1.5){
			arm->move_down();
		}
		
		if (timer->Get() < 4) {
			drive->ArcadeDrive(0.1f, 0.5f, false);
		} else if (timer->Get() < 6){
			winch->fire();
		}
		
		arm->update();
		winch->update();
		lcd->PrintfLine(DriverStationLCD::kUser_Line1, "auton");
		lcd->UpdateLCD();
	}

	void TeleopPeriodic(void) {
		//standard arcade drive using left and right sticks
		//clamp the values so small inputs are ignored
		float speed = -pilot->GetLeftY();
		speed = clamp(speed, 0.05f);
		float turn = -pilot->GetRightX();
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
         
        //lcd->PrintfLine(DriverStationLCD::kUser_Line3, "%f %f", speed, turn);
         
        drive->ArcadeDrive(turn, speed);
        //drive->TankDrive(-pilot->GetLeftY(), pilot->GetRightY());
        old_turn = turn;
        old_forward = speed;
		
 		if (pilot->GetNumberedButton(5) || pilot->GetNumberedButton(6)){
 			gear_shift->Set(HIGH_GEAR);
 		} else if (pilot->GetNumberedButton(7) || pilot->GetNumberedButton(8)){
 			gear_shift->Set(LOW_GEAR);
 		}

		//spin the roller forward or back
		if ((copilot->GetNumberedButton(Gamepad::F310_Y))) {
			arm->run_roller_in();
		} else if (copilot->GetNumberedButton(Gamepad::F310_A)){
			arm->run_roller_out();
		} else if (copilot->GetNumberedButton(Gamepad::F310_X)){
			arm->drop_ball_in();
		}
		
		float left_y = clamp(copilot->GetRawAxis(Gamepad::F310_LEFT_Y), 0.05f);
		
		//lcd->PrintfLine(DriverStationLCD::kUser_Line6, "%d", arm_top->Get());
		if (left_y > 0.3f) {
			arm->move_up();
		} else if (left_y < -0.3f){
			arm->move_down();
		}
		
		if (copilot->GetNumberedButton(Gamepad::F310_B)){
			winch->wind_back();
		}
		
		if (copilot->GetNumberedButton(Gamepad::RIGHT_BUMPER)){
			winch->fire();
		}
		
		//camera->GetImage();
		
		arm->update();
		winch->update();
		
		lcd->PrintfLine(DriverStationLCD::kUser_Line1, "teleop");
		lcd->UpdateLCD();
		
	}

	//storing place for stuff not ready to be implemented yet
	void UnusedTeleopPeriodic() {
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
			winch->wind_back_rotations(winch_rotations);
		}
		
		float dpad = copilot->GetRawAxis(Gamepad::F310_DPAD_X_AXIS);
		
		if (dpad < -0.5f){
			arm->set_position(Arm::FLOOR_POSITION);
		} else if (dpad > 0.5f){
			arm->set_position(Arm::TOP_POSITION);
		} else if (false) {		//we don't have a control for this right now
			arm->set_position(Arm::LOW_GOAL_POSITION);
		}
		
		//Compressor on button 10
		//flagrantly illegal according to inspector
		/*
		if (pilot->GetNumberedButton(10) && !pressure_switch->Get()){
			compressor->Set(Relay::kOn);
		} else {
			compressor->Set(Relay::kOff);
		}
		*/
	}
	
	void TestInit() {
		TestAllInit();
	}
	
	void TestPeriodic() {
		TestAllPeriodic();
	}
	
	void TestOneGamepadInit() {
		
	}
	
	Victor * test_victors[10];
	DigitalOutput * test_dios[14];
	void TestAllInit() {
		for (int i = 0; i < 10; i++){
			test_victors[i] = new Victor(i);
		}
		for (int i = 0; i < 14; i++){
			test_dios[i] = new DigitalOutput(i);
		}
	}
	
	void TestAllPeriodic() {
		for (int i = 0; i < 10; i++){
			test_victors[i]->Set(1.0f);
		}
		for (int i = 0; i < 14; i++){
			test_dios[i]->Set(true);
		}
	}
	
};

START_ROBOT_CLASS(AerialAssistRobot);
