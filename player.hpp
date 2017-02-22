/*
Jeremy Galarza Scott Weagley 12/10/2016
Warbird Simulation Phase 2 & 3
*/
# ifndef __INCLUDES465__
# include "../includes465/include465.hpp"
# define __INCLUDES465__
# endif

class Player{
private:
    int step = 0;
	int speed[3] = {10,50,200};
	int currentSpeed = 0;
	int stepDistance;
	int pitch = 0, roll = 0 , yaw = 0;
    float radians = 0.02f;
	float angle = 0.0f;
	glm::vec3 axis = glm::vec3(0.0f);
	glm::vec3 forward = glm::vec3(0.0f);
    glm::mat4 RM = glm::mat4();
	glm::mat4 RM2 = glm::mat4();
	glm::mat4 OM = glm::mat4();
	glm::mat4 TM = glm::mat4();
	glm::mat4 SM = glm::mat4();
	glm::vec3 position;
	glm::vec3 T;
	glm::vec3 L;
	float AORdirection;
	bool isGravity = false;
	float distance;

public:

	Player(glm::vec3 t , glm::vec3 s) {
		position = t;
		TM = glm::translate(glm::mat4(),t);
		SM = glm::scale(glm::mat4(), s);
		OM = TM * RM * SM;
		stepDistance = speed[currentSpeed];
	}
	void changeSpeed() {
		currentSpeed++;
		if (currentSpeed >= 3) {
			currentSpeed = 0;
		}
		stepDistance = speed[currentSpeed];
	}
	void setMove(int i){
		step = i;
    }
    void setPitch(int i){
        pitch = i;
    }
    void setRoll(int i){
        roll = i;
    }
	void setYaw(int i) {
		yaw = i;
	}
	glm::mat4 getOM() {
		return OM;
	}
	void changeGravity() {
		isGravity = !isGravity;
	}
	//function to warp
	void warp(glm::mat4 p, glm::mat4 t) {
		//Use translation vector to get transformation matrix
		TM = t;
		//create a matrix with translation vector in order to get hypothetical OM
		//use this hypothetical OM to get the lat vector of where the ship would be
		//after it changes position
		L = getIn(t);
		//get Target vector which comes from the position the ship is going to be at
		//after warping and the position of the planet
		T = getPosition(p) - getPosition(t);
		//normalize both vectors
		T = glm::normalize(T);
		L = glm::normalize(L);
		//cross product of T and L to get orthogonal vector 
		//this is the axis of rotation
		axis = glm::cross(T, L);
		//normalize axis
		axis = glm::normalize(axis);
		//to determine the direction of the rotation
		//acos gives the radians for rotation but only from 0 to PI
		//however since glm::rotate adjust the rotation if the axis is negative
		//it is not necessary to do anything to acos other than subtract it from 2PI
		angle = (2.0f*PI) - acos(glm::dot(T,L));
		//use the found rotational angle and axis to rotate the space ship
		//we rotate it around the identity because we do not need take the current
		//RM into account
		RM = glm::rotate(glm::mat4(), angle, axis);
	}

	glm::vec3 getPos() {
		return position;
	}
	glm::mat4 getRM() {

		return RM;
	}
	void setRM(glm::mat4 r) {

		RM = r;
	}
	glm::vec3 getForward() {
		return forward;
	}

	glm::mat4 getTM() {
		return TM;
	}
	glm::mat4 getSM() {

		return SM;
	}
    void update(){
		if (pitch != 0){
			RM = glm::rotate(RM,pitch*radians,glm::vec3(1,0,0));
		}
		else if (yaw != 0) {
			RM = glm::rotate(RM, yaw*radians, glm::vec3(0, 1, 0));
			
		}
		else if (roll != 0 ) {
			RM = glm::rotate(RM, roll*radians, glm::vec3(0, 0, 1));
		}
		if (isGravity) {
			distance = glm::distance(glm::vec3(0.0f),getPosition(OM));
			float temp = 90000000.0f / (distance * distance);
			glm::vec3 tempVec = glm::normalize( -1.0f * getPosition(OM));
			TM = glm::translate(TM, temp * tempVec );
		}
		forward = getIn(OM) * (float)step * (float)stepDistance;
		TM = glm::translate(TM, forward);
		OM = TM * RM * SM;
		step = 0;
		yaw = 0;
		roll = 0;
		pitch = 0;	
    }
};



