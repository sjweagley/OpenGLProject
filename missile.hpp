/*
Jeremy Galarza Scott Weagley 12/10/2016
Warbird Simulation Phase 2 & 3
*/
# ifndef __INCLUDES465__
# include "../includes465/include465.hpp"
# define __INCLUDES465__
# endif

class Missile{
private:
	bool isLive = false;
	bool noMore = false;
	bool isFired = false;
	bool isCollided = false;
	bool isTracking = false;
	bool isSite;
	bool targetSet = false;
	bool hitTarget = false;
	int updates = 0;
	int step = 0;
	int stepDistance = 0;
	float angle = 0.0f;
	int amount;
	glm::mat4 target;
	glm::vec3 axis = glm::vec3(0.0f);
	glm::vec3 forward = glm::vec3(0.0f);
    glm::mat4 RM = glm::mat4();
	glm::mat4 OM = glm::mat4();
	glm::mat4 TM = glm::mat4();
	glm::mat4 SM = glm::mat4();
	glm::vec3 position;
	glm::vec3 T;
	glm::vec3 L;  
	float AORdirection;
	bool isWarping = false;

public:

	Missile(glm::vec3 translate , glm::vec3 scale, bool site, int a) {
		isSite = site;
		amount = a;
		if (site) {
			stepDistance = 5;
		}
		else {
			stepDistance = 20;
		}
		TM = glm::translate(TM, translate);
		SM = glm::scale(glm::mat4(), scale);
		OM = TM * RM * SM;
	}

	glm::mat4 getOM() {
		return OM;
	}

	void setTranslate(glm::vec3 translate) {
		TM = glm::translate(glm::mat4(), translate);
	}

	//Finish this method
	void setRotate(glm::mat4 inputRM) {
		RM = inputRM;
	}

	//Returns if target has been set
	bool getTargetSet() {
		return targetSet;
	}

	//Returns OM of target
	glm::mat4 getTarget() {
		return target;
	}

	//Returns OM of target
	glm::mat4 setTarget(glm::mat4 targetIn) {
		target = targetIn;
	}

	void updateTargetPos(glm::mat4 targetPos) {
		target = targetPos;
	}

	glm::mat4 getTM() {
		return TM;
	}

	void setTM(glm::mat4 inTM) {
		TM = inTM;
	}
	glm::mat4 getRM() {

		return RM;
	}

	//Maybe delete since glm has this
	//Get distance
	float getDistance(glm::vec3 pos1, glm::vec3 pos2) {
		return glm::distance(pos1, pos2);
	}

	//Method to fire missile
	bool getFired() {
		return isFired;
	}
	//Method to fire missile
	void fireMissile(glm::mat4 r, glm::mat4 t) {
			RM = r;
			TM = t;
			isFired = true;
			OM = TM * RM * SM;
	}

	//Will take source OM, target1 OM, target2 OM
	//Will return 0 if there is no target within range, 1 if first target is in range, and 	2 if second target is within range 
	int setClosestTarget(glm::mat4 target1, glm::mat4 target2, boolean target1Des, boolean target2Des) {
		int result = 0;
		float target1Distance = distance(getPosition(OM), getPosition(target1));
		float target2Distance = distance(getPosition(OM), getPosition(target2));

		if ((target1Distance <= 5000.0f && !target1Des) || (target2Distance <= 5000.0f && !target2Des)) {
			if ((target1Distance <= 5000.0f && !target1Des) && (target2Distance <= 5000.0f && !target2Des)) {
				if (target1Distance < target2Distance) {
					target = target1;
					targetSet = true;
					result = 1;
				}
				else  {
					target = target2;
					targetSet = true;
					result = 2;
				}
			} 
			else if (target1Distance <= 5000.0f && !target1Des) {
				result = 1;
			}
			else {
				result = 2;
			}
		}
		if (result > 0) {
			isTracking = true;
		}
		return result;
	}
	bool getTracking() {
		return isTracking;
	}
	void tracked() {
		isTracking = true;
	}
	//Will set target to ship
	void setClosestTarget(glm::mat4 target1) {
		float targetDistance = distance(getPosition(OM), getPosition(target1));
		if (targetDistance <= 5000.0f) {
			target = target1;
			targetSet = true;
			isFired = true;
		}
	}

	//Check if there is a collision
	bool checkCollision(glm::vec3 pos1, glm::vec3 pos2, float radius1, float radius2) {
		 
		if (distance(pos1, pos2) <= (radius1 + radius2)) {
			isCollided = true;
			hitTarget = true;
		}
		return isCollided;
	}

	//function to chase target
	void faceTarget(glm::mat4 target) {
		//Get the looking at vector from the the chaser
		L = glm::normalize(getIn(OM));

		//Get distance between the target and chaser
		T = glm::normalize(getPosition(target) - getPosition(OM));

		if (colinear(L, T, 0.1f)) {
			float CLATtoE = getDistance(getPosition(OM) + getIn(OM), getPosition(target));
			float CtoE = getDistance(getPosition(OM), getPosition(target));

			if (CtoE < CLATtoE) {
				axis = glm::normalize(glm::cross(T, getUp(OM))); //AOR
				RM = glm::rotate(RM, PI, axis);
				OM = glm::translate(TM, getPosition(OM)) * RM * SM;
			}	
		}
		else {
			axis = glm::normalize(glm::cross(T, L)); //AOR
						
			if (getDistance(getPosition(OM), getPosition(target)) > 0.5f) {
				angle = (2 * PI) - acos(glm::dot(T, L));
				RM = glm::rotate(RM, angle, axis);
				OM = glm::translate(TM, getPosition(OM)) * RM * SM;
			}			
		}		
	}

	glm::vec3 getPos() {
		return position;
	}
	glm::mat4 getSM() {

		return SM;
	}
	void setOM(glm::mat4 inOM) {
		OM = inOM;
	}
	void setRM(glm::mat4 r) {
		RM = r;
	}
	void setSM(glm::mat4 inSM) {
		SM = inSM;
	}
	glm::vec3 getForward() {
		return forward;
	}
	bool anyLeft() {
		return !noMore;
	}
	bool isAlive() {
		return isLive;
	}
    void update(){
		if (isFired) {
			updates++;
			forward = glm::normalize(getIn(OM)) *  (float)stepDistance;
			TM = glm::translate(TM, forward);
			OM = TM * RM * SM;
		}
		if (updates >= 200) {
			isLive = true;
		}
		//Check for max updates and detonate
		if (updates >= 2000 || hitTarget) {
			//isDetonated = true;
			hitTarget = false;
			isLive = false;
			amount--;
			updates = 0;
			isTracking = false;
			isCollided = false;
			isFired = false;
		}
		if (amount == 0) {
			noMore = true;
		}
		
    }

};



