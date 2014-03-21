#ifndef ARM_H_
#define ARM_H_

#include "WPILib.h"

class Arm {
private:
	static const float MOVEMENT_RATE = (90.0f / 4.0f); //going down
	Victor * roller;
	Victor * pivot;
	Encoder * encoder;
	DigitalInput * floor_switch;
	DigitalInput * top_switch;
	DigitalInput * ball_switch;
	PIDController * pid;
	bool roller_set;
	bool pivot_set;
	bool moving_to_bottom;
	bool moving_to_top;
	
	typedef enum arm_mode_e 
		{FREE, LOWERING, RAISING, WAITING_FOR_BALL, WAITING_TO_DROP} arm_mode_t;
	arm_mode_t arm_mode;
	
	typedef enum roller_mode_e {OFF, INTAKE, DEPLOY, EJECT} roller_mode_t;
	roller_mode_t roller_mode;
	
	//acceleration control based on assigning different speeds to different encoder regions
	void move_up_interval();
	void move_down_interval();
	
	//acceleration control based on PID
	void move_up_pid();
	void move_down_pid();

	bool at_top();
	bool at_bottom();
	
public:
	static const int TOP_POSITION = 0;
	static const int FLOOR_POSITION = 55;
	static const int LOW_GOAL_POSITION = 20; //TODO: determine this
	Arm(Victor * roller_motor, Victor * pivot_motor, Encoder * enc, DigitalInput * floor, DigitalInput * top, DigitalInput * ball);
	void set_position(int pos); //position from 0 (top) to 60 (floor)
	void run_roller_in();
	void run_roller_out();
	void drop_ball_in();
	void load_sequence();
	void end_sequence();
	void move_up();
	void move_down();
	void move_up_curved();
	void move_down_curved();
	void move_to_bottom();
	void move_to_top();
	void hold_position_pid();
	bool ball_captured();
	void update();
};


#endif
