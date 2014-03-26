#ifndef ARM_H_
#define ARM_H_

#include "WPILib.h"

/*
 * This is the class that controls the arm (including the roller)
 * Important to understand: it has some persistent states that stay without being refreshed
 * If move_to_top() or move_to_bottom() is called, it will keep doing that
 * load_sequence() does the same thing
 * In addition, if you move manually to the top or the bottom, the arm will maintain that position
 * If you want to move manually, you have to call override()
 * Also, call update() every cycle or nothing will work
 * This relies on the arm limit switch to calibrate the encoder and determine the top position
 */
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
	bool pivot_set;

	typedef enum arm_mode_e 
		{FREE, LOWERING, RAISING, WAITING_FOR_BALL, LOW_GOAL, HOLDING_AT_TOP, HOLDING_AT_BOTTOM} arm_mode_t;
	arm_mode_t arm_mode;

	typedef enum roller_mode_e {OFF, INTAKE, DEPLOY, EJECT} roller_mode_t;
	roller_mode_t roller_mode;

	//acceleration control based on assigning different speeds to different encoder regions
	void move_up_interval();
	void move_down_interval();

	//acceleration control based on PID
	void move_up_pid();
	void move_down_pid();

public:
	static const int TOP_POSITION = 0;
	static const int FLOOR_POSITION = 50;
	static const int LOW_GOAL_POSITION = 30; //TODO: determine this
	Arm(Victor * roller_motor, Victor * pivot_motor, Encoder * enc, 
			DigitalInput * floor, DigitalInput * top, DigitalInput * ball);
	/*
	 * Runs the roller in unless the linebreak has been hit
	 * If the arm limit switch is hit, this will override the linebreak and allow the roller to spin
	 * In theory this the only function you need for spinning the roller in
	 * Only lasts for this cycle, roller will not continue spinning unless you call this every cycle
	 * Does nothing unless update() called in the same cycle
	 */
	void run_roller_in();
	/*
	 * Runs the roller out, to eject/pass the ball
	 * Only lasts for this cycle, roller will not continue spinning unless you call this every cycle
	 * Does nothing unless update() called in the same cycle
	 */
	void run_roller_out();
	/*
	 * Runs the roller in, regardless of linebreak
	 * You shouldn't actually need this
	 * (wait for it)
	 * Only lasts for this cycle, roller will not continue spinning unless you call this every cycle
	 * Does nothing unless update() called in the same cycle
	 */
	void drop_ball_in();
	/*
	 * The fancy load sequence
	 * If a ball is not captured, moves the arm down and rolls in until one is
	 * Then raises the arm to the top
	 * Getting the ball into the shooter is your job
	 * It's kind of weird how this works, so pay attention
	 * If you stop calling this function while in the middle of moving up or down, it will continue
	 * To the top or bottom, like you had called the move_to_bottom or move_to_top function
	 * But if you stop in the middle of rolling in, it will just stop.
	 * Does nothing unless update() called in the same cycle
	 */
	void load_sequence();
	/*
	 * Overrides the arm's attempts to move or stay at the top or bottom.
	 * Will end whatever the action the arm is doing (including load_sequence)
	 * It will not return after you stop overriding, ie, it has no memory of what you were doing
	 * Before you called override
	 * Actually nothing to do with the roller or linebreak; you may be thinking of drop_ball_in()
	 */
	void override();
	/*
	 * Moves the arm up, adjusting speed according to the arm's position
	 * If you're calling this function, you probably want to call override() as well
	 * Otherwise whatever else the arm is doing will take precedence (probably)
	 */
	void move_up_curved();
	/*
	 * Moves the arm down, adjusting speed according to the arm's position
	 * If you're calling this function, you probably want to call override() as well
	 * Otherwise whatever else the arm is doing will take precedence (probably)
	 */
	void move_down_curved();
	/*
	 * Moves the arm until it hits the bottom 
	 * More specifically, once the encoder reads above a certain value
	 * This only needs to be called once to take effect
	 * Though repeat callings do no harm (or anything at all)
	 * Uses curved acceleration
	 * Does nothing unless update() called in the same cycle
	 */
	void move_to_bottom();
	/*
	 * Moves the arm until it hits the top
	 * More specifically, until the limit switch is hit
	 * This only needs to be called once to take effect
	 * Though repeat callings do no harm (or anything at all)
	 * Uses curved acceleration
	 * Does nothing unless update() called in the same cycle
	 */
	void move_to_top();
	/*
	 * Returns true if the arm is at the top (ie, the limit switch is hit)
	 */
	bool at_top();
	/*
	 * Returns true if the arm is at the bottom (ie, the encoder is past a designated value)
	 */
	bool at_bottom();
	/*
	 * Returns true if a ball is captured (ie, the linebreak is broken)
	 */
	bool ball_captured();
	/*
	 * Actually does pretty much everything
	 * Almost nothing will work if you don't call this every cycle
	 * So just stick it at the bottom of every periodic function and leave it alone
	 * If you've been reading all of these comments, you should know this by now
	 */
	void update();
	
	//deprecated
	//use the ultra-fancy move_up_curved and move_down_curved instead
	void move_up();
	void move_down();
	
	//these not implemented yet
	void hold_position_pid();
	void set_position(int pos); //position from 0 (top) to 60 (floor)
};


#endif
