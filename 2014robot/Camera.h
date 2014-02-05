#ifndef CAMERA_H_
#define CAMERA_H_

class Camera
{
private:
	//Structure to represent the scores for the various tests used for target identification
	struct Scores {
		double rectangularity;
		double aspectRatioInner;
		double aspectRatioOuter;
		double xEdge;
		double yEdge;

	};
	Scores *scores;
public:
	float getDistance();
	
};

#endif
