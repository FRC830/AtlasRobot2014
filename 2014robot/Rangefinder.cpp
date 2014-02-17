#include "Rangefinder.h"
#include <cmath>


Rangefinder::Rangefinder(Ultrasonic * us_left, Ultrasonic * us_right){
	left = us_left;
	right = us_right;
	us_left->SetEnabled(true);
	us_right->SetEnabled(true);
	Ultrasonic::SetAutomaticMode(true);
	invalid_count = 0;
	for(int i=0; i<ARRAY_LENGTH; i++){
		distance_group[1] = 0;
	}
	current_array_point = 0;
}

void Rangefinder::update(){
	if(distance_state_l==0){
		left->Ping();
		distance_state_l=1;
		counter_l=0;
	}else if(distance_state_l==1){
		if(left->IsRangeValid()){
			distance = left->GetRangeInches();
			distance_state_l=0;
			counter_l=0;
			
			if(distance>200){//throw out bad values
				invalid_count++;
			}else{
				distance_group[current_array_point] = distance;
				current_array_point++;
				if(current_array_point>=4){
					current_array_point = 0;
				}
			}
			
		}else if(counter_l++>3){
			distance_state_l=0;
		}
	}else{
		distance_state_l=0;
		counter_l=0;
	}
}

//angle in rads, relative to a position facing directly towards a surface
//positive angle = clockwise from vertical
float Rangefinder::robot_angle(){
	float diff = left->GetRangeInches() - right->GetRangeInches();
	float angle_rads = atan(diff / SENSOR_DISTANCE);
	return angle_rads;
}

//return distance between robot and nearest surface
float Rangefinder::robot_distance(){
	//average both distances to find distance from center
	//return (left->GetRangeInches() + right->GetRangeInches()) / 2;
	if(invalid_count>4){//cannot get good reading
		return -1.0;
	}else{//can get good reading
		//return avg from dist group
		float sum = 0;
		for (int i=0; i<ARRAY_LENGTH; i++){
			sum+=distance_group[i];
		}
		return sum/ARRAY_LENGTH;
	}
	
}
