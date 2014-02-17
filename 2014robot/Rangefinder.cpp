#include "Rangefinder.h"
#include <cmath>


Rangefinder::Rangefinder(Ultrasonic * us){
	ultrasonic = us;
	ultrasonic->SetEnabled(true);
	Ultrasonic::SetAutomaticMode(true);
	invalid_count = 0;
	for(int i=0; i<ARRAY_LENGTH; i++){
		distance_group[1] = 0;
	}
	current_array_point = 0;
}

void Rangefinder::update(){
	if(distance_state==0){
		ultrasonic->Ping();
		distance_state=1;
		counter=0;
	}else if(distance_state==1){
		if(ultrasonic->IsRangeValid()){
			distance = ultrasonic->GetRangeInches();
			distance_state=0;
			counter=0;
			
			if(distance>200){//throw out bad values
				invalid_count++;
			}else{
				distance_group[current_array_point] = distance;
				current_array_point++;
				if(current_array_point>=4){
					current_array_point = 0;
				}
			}
			
		}else if(counter++>3){
			distance_state=0;
		}
	}else{
		distance_state=0;
		counter=0;
	}
}

//return distance between robot and nearest surface
float Rangefinder::robot_distance(){
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
