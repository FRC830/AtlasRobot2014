#include "AerialAssistRobot.h"

AerialAssistRobot::AerialAssistRobot(void)	{

}

void AerialAssistRobot::RobotInit(void) {
	front_left = new Talon(FRONT_LEFT_DRIVE_PWM);
	front_right = new Talon(FRONT_RIGHT_DRIVE_PWM);
	rear_left = new Talon(REAR_LEFT_DRIVE_PWM);
	rear_right = new Talon(REAR_RIGHT_DRIVE_PWM);
	drive = new RobotDrive(front_left, rear_left, front_right, rear_right);
	drive->SetInvertedMotor(RobotDrive::kFrontLeftMotor, false);
	drive->SetInvertedMotor(RobotDrive::kRearLeftMotor, false);
	drive->SetInvertedMotor(RobotDrive::kFrontRightMotor, false);
	drive->SetInvertedMotor(RobotDrive::kRearRightMotor, false);
	old_speed = 0.0f;
	old_turn = 0.0f;

	roller = new Victor(ROLLER_PWM);
	arm_lift = new Victor(ARM_LIFT_PWM);
	arm_floor = new DigitalInput(ARM_FLOOR_SWITCH_DIO);
	arm_top = new DigitalInput(ARM_TOP_SWITCH_DIO);
	arm_ball = new DigitalInput(ARM_LINE_BREAK_DIO);
	arm_encoder = new Encoder(ARM_ENCODER_A_CHANNEL, ARM_ENCODER_B_CHANNEL, false);

	arm = new Arm(roller, arm_lift, arm_encoder, arm_floor, arm_top, arm_ball);

	winch_motor = new Victor(WINCH_PWM);
	winch_encoder = new Encoder(WINCH_ENCODER_A_CHANNEL, WINCH_ENCODER_B_CHANNEL);
	winch_max_switch = new DigitalInput (WINCH_MAX_LIMIT_DIO);

	led_red_channel = new DigitalOutput(RED_LED_DIO);
	led_green_channel = new DigitalOutput(GREEN_LED_DIO);
	led_blue_channel = new DigitalOutput(BLUE_LED_DIO);
	led = new DigitalLED(led_red_channel, led_green_channel, led_blue_channel);

	gear_shift = new DoubleSolenoid(GEAR_SHIFT_SOL_FORWARD, GEAR_SHIFT_SOL_REVERSE);
	clutch = new Solenoid(CLUTCH_SOL);

	winch = new Winch(winch_motor, clutch, winch_encoder, winch_max_switch);

	ultrasonic = new Ultrasonic(RANGE_FINDER_PING_CHANNEL_DIO, RANGE_FINDER_ECHO_CHANNEL_DIO);
	rangefinder = new Rangefinder(ultrasonic);

	//pressure_switch = new DigitalInput(PRESSURE_SWITCH_DIO);
	compressor = new Compressor(PRESSURE_SWITCH_DIO, COMPRESSOR_RELAY);
	lcd = DriverStationLCD::GetInstance();
	ds = DriverStation::GetInstance();
	if (ds->GetAlliance() == DriverStation::kBlue){
		alliance_color = DigitalLED::BLUE;
	} else {
		alliance_color = DigitalLED::RED;
	}

	//camera = &AxisCamera::GetInstance();

	timer = new Timer();

	pilot = new Gamepad(1);
	copilot = new Gamepad(2);

}

void AerialAssistRobot::DisabledInit(void) {
	lcd->Clear();
}

void AerialAssistRobot::AutonomousMainInit(void) {
	gear_shift->Set(HIGH_GEAR);
	winch->wind_back();
	timer->Reset();
	timer->Start();
	compressor->Start(); //required by rules
}

void AerialAssistRobot::AutonomousDriveForwardInit(void) {
	gear_shift->Set(HIGH_GEAR);
	timer->Reset();
	timer->Start();
	compressor->Start(); //required by rules
}

void AerialAssistRobot::AutonomousTwoBallInit(void) {
	gear_shift->Set(HIGH_GEAR);
	winch->wind_back();
	timer->Reset();
	timer->Start();
	compressor->Start(); //required by rules
}

void AerialAssistRobot::TeleopInit(void) {
	compressor->Start();
	firing = false;
}

void AerialAssistRobot::DisabledPeriodic(void)  {
	lcd->PrintfLine(DriverStationLCD::kUser_Line1, "disabled");
	led->Set(alliance_color);
	lcd->UpdateLCD();
}

void AerialAssistRobot::AutonomousMainPeriodic(void) {
	double time_s = timer->Get();
	if (time_s < 2.5){
		arm->drop_ball_in();
	} else if (time_s < 4.5){
		arm->move_to_bottom();
	}

	if (time_s > 7.0 && time_s < 7.5){
		winch->fire();
		lcd->PrintfLine(DriverStationLCD::kUser_Line4, "firing");
	} else {
		lcd->PrintfLine(DriverStationLCD::kUser_Line4, "");
	}
	if (time_s < 6.0){
		drive->ArcadeDrive(0.5f, -0.2f); //TODO: May need to compensate for drive-train turn
		lcd->PrintfLine(DriverStationLCD::kUser_Line5, "left: %f", front_left->Get());
		lcd->PrintfLine(DriverStationLCD::kUser_Line6, "right: %f", front_right->Get());
	}

	//flash LEDs
	//flash red from (0.0 -> 0.5) and (1.0 -> 1.5), then hold green from 2.0s on
	if(time_s > 5.0 && time_s < 5.5 || time_s > 6.0 && time_s < 6.5){
		led->Set(DigitalLED::RED);
	} else if (time_s > 5.5 && time_s < 6.0 || time_s > 6.5 && time_s < 7.0){
		led->Set(DigitalLED::OFF);
	} else if (time_s > 7.0){
		led->Set(DigitalLED::GREEN);
	}

	arm->update();
	winch->update();
	rangefinder->update();
	lcd->PrintfLine(DriverStationLCD::kUser_Line1, "auton");
	lcd->PrintfLine(DriverStationLCD::kUser_Line2, "time: %f", time_s);
	lcd->PrintfLine(DriverStationLCD::kUser_Line3, "dist: %f", rangefinder->Get());
	lcd->UpdateLCD();
}

//TWO-BALL AUTON:
void AerialAssistRobot::AutonomousTwoBallPeriodic(void) {
	double time_s = timer->Get();
	if(time_s < 2.5){
		arm->drop_ball_in();
		drive->ArcadeDrive(-0.4f,0.0f);
	}
	
	if (time_s > 2.5 && time_s < 6.0){
		arm->load_sequence();
		if (arm->can_fire()){
			winch->fire();
		}
	}
	
	if (time_s > 6.0 && time_s < 7.5){
		arm->drop_ball_in(); //just to make sure
	}
	
	if (time_s > 7.5){
		arm->move_to_bottom();
		if (arm->can_fire()){
			winch->fire();
		}
	}
	
	if (time_s > 4.0){
		drive->ArcadeDrive(0.5, 0.0);
	}
	
	
	arm->update();
	winch->update();
	rangefinder->update();
	lcd->PrintfLine(DriverStationLCD::kUser_Line1, "auton");
	lcd->PrintfLine(DriverStationLCD::kUser_Line2, "time: %f", time_s);
	lcd->PrintfLine(DriverStationLCD::kUser_Line3, "dist: %f", rangefinder->Get());
	lcd->UpdateLCD();
}
void AerialAssistRobot::AutonomousDriveForwardPeriodic() {
	double time_s = timer->Get();
	
	drive->ArcadeDrive(0.5f, 0.0f);
	//drive->ArcadeDrive(0.5f, -0.4f);
	lcd->PrintfLine(DriverStationLCD::kUser_Line5, "left: %f", front_left->Get());
	lcd->PrintfLine(DriverStationLCD::kUser_Line6, "right: %f", front_right->Get());
	
	//flash LEDs
	//flash red from (0.0 -> 0.5) and (1.0 -> 1.5), then hold green from 2.0s on
	if(time_s < 0.5 || time_s > 1.0 && time_s < 1.5){
		led->Set(DigitalLED::RED);
	} else if (time_s > 0.5 && time_s < 1.0 || time_s > 1.5 && time_s < 2.0){
		led->Set(DigitalLED::OFF);
	} else {
		led->Set(DigitalLED::GREEN);
	}
	
	rangefinder->update();
	arm->update();
	winch->update();
	lcd->PrintfLine(DriverStationLCD::kUser_Line1, "auton drive");
	lcd->PrintfLine(DriverStationLCD::kUser_Line2, "time: %f", timer->Get());
	lcd->PrintfLine(DriverStationLCD::kUser_Line3, "dist: %f", rangefinder->Get());
	lcd->UpdateLCD();
}

void AerialAssistRobot::TeleopPeriodic(void) {
	//standard arcade drive using left and right sticks
	//clamp the values so small inputs are ignored
	float speed = -pilot->GetLeftY();
	speed = clamp(speed, 0.05f);
	float turn = -pilot->GetRightX();
	turn = clamp(turn, 0.05f);

	//lcd->PrintfLine(DriverStationLCD::kUser_Line2, "%f %f", speed, turn);

	max_delta_speed = 1.0f / (MAX_ACCEL_TIME * GetLoopsPerSec()); //1.0 represents the maximum victor input
	//float delta_turn = turn - old_turn;
	float delta_speed = speed - old_speed;

	if (delta_speed > max_delta_speed){
		speed = old_speed + max_delta_speed;
	} else if (delta_speed < -max_delta_speed){
		speed = old_speed - max_delta_speed;
	}
	//if neither of these is the case, |delta_speed| is less than the max 
	//and we can just use the given value without modification.

	lcd->PrintfLine(DriverStationLCD::kUser_Line2, "%f %f", speed, turn);
	//lcd->PrintfLine(DriverStationLCD::kUser_Line3, "%f %f", left_drive->Get(), right_drive->Get());

	drive->ArcadeDrive(speed, turn);
	//drive->TankDrive(-pilot->GetLeftY(), pilot->GetRightY());
	old_turn = turn;
	old_speed = speed;

	if (pilot->GetNumberedButton(5) || pilot->GetNumberedButton(6) 
			|| pilot->GetNumberedButton(7) || pilot->GetNumberedButton(8)) {
		gear_shift->Set(LOW_GEAR);
	} else {
		gear_shift->Set(HIGH_GEAR);
	}

	if (copilot->GetNumberedButton(Gamepad::F310_X)){
		arm->load_sequence();
	} else if (copilot->GetNumberedButtonReleased(Gamepad::F310_X)){
		arm->override(); //break out of the load sequence, so we don't keep moving up/down
	}

	if (copilot->GetNumberedButton(Gamepad::F310_A)){
		arm->drop_ball_in();
	}

	if (copilot->GetNumberedButton(Gamepad::F310_LB) || copilot->GetNumberedButton(Gamepad::F310_RB)
			|| fabs(copilot->GetRawAxis(Gamepad::F310_TRIGGER_AXIS)) > 0.2){
		arm->run_roller_out();
	}

	float left_y = copilot->GetRawAxis(Gamepad::F310_LEFT_Y);
	//lcd->PrintfLine(DriverStationLCD::kUser_Line4, "%arm top: %d", arm_top->Get());
	if (left_y > 0.2f) {
		arm->override();
		arm->move_up_curved();
	} else if (left_y < -0.2f){
		arm->override();
		arm->move_down_curved();
	}

	float left_x = copilot->GetRawAxis(Gamepad::F310_LEFT_X);
	if (fabs(left_x) > 0.2f){
		arm->move_to_top();
	} 
	
	if (copilot->GetNumberedButton(Gamepad::F310_Y)){
		arm->override();
		arm->move_towards_low_goal();
	}
	
	lcd->PrintfLine(DriverStationLCD::kUser_Line5, "winch: %d", winch_max_switch->Get());
	
	if (copilot->GetNumberedButtonPressed(Gamepad::F310_B)){
		firing = true;
	}
	
	if (firing) {
		if (arm->can_fire()){
			winch->fire();
			firing = false;
		} else {
			arm->override();
			arm->move_down_curved();
		}
	}
	
	//secret button for winding back
	if (copilot->GetNumberedButton(8)){
		winch->wind_back();
	}

	if (!winch->wound_back()){
		led->Set(DigitalLED::YELLOW);
	} else if (arm->ball_captured()){
		led->Set(DigitalLED::GREEN); 
	} else {
		led->Set(alliance_color);
	}

	//camera->GetImage();

	arm->update();
	winch->update();
	rangefinder->update();
	
	lcd->PrintfLine(DriverStationLCD::kUser_Line1, "teleop");
	lcd->PrintfLine(DriverStationLCD::kUser_Line3, "enc: %d", arm_encoder->Get());
	lcd->PrintfLine(DriverStationLCD::kUser_Line4, "arm: %d", arm_top->Get());
	lcd->PrintfLine(DriverStationLCD::kUser_Line6, "wv: %f", winch_motor->Get());
	/*
	if (arm->ball_captured()){
		lcd->PrintfLine(DriverStationLCD::kUser_Line6, "lb broken");
	} else {
		lcd->PrintfLine(DriverStationLCD::kUser_Line6, "lb not broken");
	}
	*/
	//lcd->PrintfLine(DriverStationLCD::kUser_Line6, "distance: %f", rangefinder->Get());
	lcd->UpdateLCD();	
}

void AerialAssistRobot::TestInit() {
	SafetyTestInit();
}
void AerialAssistRobot::TestPeriodic(){
	SafetyTestPeriodic();
}

void AerialAssistRobot::ColorTestInit() {
	lcd->Clear();
	red = false;
	green = false;
	blue = false;
}

void AerialAssistRobot::ColorTestPeriodic() {
	if (copilot->GetNumberedButtonPressed(Gamepad::F310_B)){
		red = !red;
	}
	if (copilot->GetNumberedButtonPressed(Gamepad::F310_A)){
		green = !green;
	}
	if (copilot->GetNumberedButtonPressed(Gamepad::F310_X)){
		blue = !blue;
	}
	led->Set(red, green, blue);

	drive->ArcadeDrive(0.0f, 0.0f);
	
	lcd->PrintfLine(DriverStationLCD::kUser_Line1, "test");
	lcd->PrintfLine(DriverStationLCD::kUser_Line2, "r: %d", red);
	lcd->PrintfLine(DriverStationLCD::kUser_Line3, "g: %d", green);
	lcd->PrintfLine(DriverStationLCD::kUser_Line4, "b: %d", blue);
	lcd->UpdateLCD();

}

void AerialAssistRobot::SafetyTestInit(){
	lcd->Clear();
	compressor->Start();
	firing = false;	
	gear_shift->Set(HIGH_GEAR);
}

void AerialAssistRobot::SafetyTestPeriodic(){
	/*
	 * HEY YALL!
	 * THIS IS SAFETY MODE
	 */
	
	//standard arcade drive using left and right sticks
	//clamp the values so small inputs are ignored
	//also limit the speed to 25% power cuz this safety mode!
	float speed = 0.5*(-pilot->GetLeftY());
	speed = clamp(speed, 0.05f);
	float turn = 0.6*(-pilot->GetRightX());
	turn = clamp(turn, 0.05f);

	max_delta_speed = 1.0f / (MAX_ACCEL_TIME * GetLoopsPerSec()); //1.0 represents the maximum victor input

	float delta_speed = speed - old_speed;

	if (delta_speed > max_delta_speed){
		speed = old_speed + max_delta_speed;
	} else if (delta_speed < -max_delta_speed){
		speed = old_speed - max_delta_speed;
	}

	lcd->PrintfLine(DriverStationLCD::kUser_Line2, "%f %f", speed, turn);

	drive->ArcadeDrive(speed, turn);

	old_turn = turn;
	old_speed = speed;


	if (copilot->GetNumberedButton(Gamepad::F310_X)){
		arm->load_sequence();
	} else if (copilot->GetNumberedButtonReleased(Gamepad::F310_X)){
		arm->override(); //break out of the load sequence, so we don't keep moving up/down
	}

	if (copilot->GetNumberedButton(Gamepad::F310_A)){
		arm->drop_ball_in();
	}

	if (copilot->GetNumberedButton(Gamepad::F310_LB) || copilot->GetNumberedButton(Gamepad::F310_RB)
			|| fabs(copilot->GetRawAxis(Gamepad::F310_TRIGGER_AXIS)) > 0.2){
		arm->run_roller_out();
	}

	float left_y = copilot->GetRawAxis(Gamepad::F310_LEFT_Y);
	//lcd->PrintfLine(DriverStationLCD::kUser_Line4, "%arm top: %d", arm_top->Get());
	if (left_y > 0.2f) {
		arm->override();
		arm->move_up_curved();
	} else if (left_y < -0.2f){
		arm->override();
		arm->move_down_curved();
	}

	float left_x = copilot->GetRawAxis(Gamepad::F310_LEFT_X);
	if (fabs(left_x) > 0.2f){
		arm->move_to_top();
	} 
	
	if (copilot->GetNumberedButton(Gamepad::F310_Y)){
		arm->override();
		arm->move_towards_low_goal();
	}
	
	lcd->PrintfLine(DriverStationLCD::kUser_Line5, "winch: %d", winch_max_switch->Get());
	
	if (copilot->GetNumberedButtonPressed(Gamepad::F310_B)){
		firing = true;
	}
	
	if (firing) {
		if (arm->can_fire()){
			winch->fire();
			firing = false;
		} else {
			arm->override();
			arm->move_down_curved();
		}
	}
	
	//secret button for winding back
	if (copilot->GetNumberedButton(8)){
		winch->wind_back();
	}

	if (!winch->wound_back()){
		led->Set(DigitalLED::YELLOW);
	} else if (arm->ball_captured()){
		led->Set(DigitalLED::GREEN); 
	} else {
		led->Set(alliance_color);
	}

	//camera->GetImage();

	arm->update();
	winch->safety_update();
	rangefinder->update();
	
	lcd->PrintfLine(DriverStationLCD::kUser_Line1, "Safety Mode!!!!");
	lcd->PrintfLine(DriverStationLCD::kUser_Line3, "enc: %d", arm_encoder->Get());
	lcd->PrintfLine(DriverStationLCD::kUser_Line4, "arm: %d", arm_top->Get());
	lcd->PrintfLine(DriverStationLCD::kUser_Line6, "wv: %f", winch_motor->Get());
	/*
	if (arm->ball_captured()){
		lcd->PrintfLine(DriverStationLCD::kUser_Line6, "lb broken");
	} else {
		lcd->PrintfLine(DriverStationLCD::kUser_Line6, "lb not broken");
	}
	*/
	//lcd->PrintfLine(DriverStationLCD::kUser_Line6, "distance: %f", rangefinder->Get());
	lcd->UpdateLCD();
}
