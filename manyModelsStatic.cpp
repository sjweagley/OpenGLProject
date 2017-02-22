/*
Jeremy Galarza Scott Weagley 12/10/2016
Warbird Simulation Phase 2 & 3
*/

// define your target operating system: __Windows__  __Linux__  __Mac__
#define __Windows__ 
#include "../includes465/include465.hpp"
#include "../includes465/texture.hpp"
#include "player.hpp"
#include "missile.hpp"

glm::mat4 starRotation = glm::mat4();
int n = 0, j  = 0;
const int X = 0, Y = 1, Z = 2, START = 0, STOP = 1;
Player *player;
//Constants for models:  file names, vertex count, model display size
const int nModels = 11, nCameras = 5; //Number of models in this scene & Number of cameras
int currentCamera = 0; //Current camera
char * modelFile[nModels] = { "spaceShip-bs100.tri","Ruber.tri","Unum.tri","Duo.tri","Primus.tri","Secundus.tri","obelisk-10-20-10.tri", "unum-missile-r25.tri", "secundus-missile-r25.tri", "unum-missile-site-r30.tri", "secundus-missile-site-r30.tri" };
float modelBR[nModels], scaleValue[nModels]; //Model's bounding radius & model's scaling "size" value
const int nVertices[nModels] = { 996 * 3 , 264 * 3,264 * 3,264 * 3,264 * 3,264 * 3,14 * 3, 14 * 3, 14 * 3, 12 * 3, 12 * 3 };
char * vertexShaderFile = "simpleVertex.glsl", * fragmentShaderFile = "simpleFragment.glsl";
GLuint shaderProgram, VAO[nModels], buffer[nModels]; //Vertex Array Objects & Vertex Buffer Objects
int nextWarp = 2, timerDelay = 5, frameCount = 0;
int timerDelayCounter = 1; //Counter for timer delay speed
double currentTime, lastTime, timeInterval;
bool idleTimerFlag = false;  //Interval or idle timer ?
bool gravity = false; //Boolean for Gravity
glm::mat4 identity(1.0f), tempMat;
glm::vec3 tempVec;
glm::vec3 pointLightIntensity(0.15f,0.15f,0.15f);
glm::vec3 headLightIntensity(0.125f,0.3f,0.1f);
glm::mat4 rotation[nModels] = { glm::mat4(),glm::mat4() ,glm::mat4() ,glm::mat4() ,glm::mat4() ,glm::mat4() ,glm::mat4(), glm::mat4(), glm::mat4(), glm::mat4(), glm::mat4() };
float rotateRadian[nModels] = { 0.0f,0.0f,0.004f,0.002f,0.004f,0.002f, 0.0f,0.004f, 0.002f, 0.004f, 0.002f }; //rotation rates for the orbiting
float currentRadian[nModels] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; //the current radians meant for rotating

//Shader handles, matrices, etc
GLuint MVP;  //Model View Projection matrix's handle
GLuint ModelViewMatrix, NormalMatrix;
GLuint vPosition[nModels], vColor[nModels], vNormal[nModels];   //vPosition, vColor, vNormal handles for models
int tarInt;

//Model, view, projection matrices and values to create modelMatrix.
float modelSize[nModels] = { 100.0f,2000.0f,200.0f,400.0f,100.0f, 150.0f, 25.0f, 25.0f, 25.0f, 30.0f, 30.0f };   // size of model
glm::vec3 scale[nModels];       // set in init() 
glm::vec3 translate[nModels] = { 
glm::vec3(5000.0f,1000.0f,5000.0f), //Spaceship
glm::vec3(0, 0, 0), //Ruber
glm::vec3(4000, 0, 0), //Unum
glm::vec3(9000, 0, 0), //Duo
glm::vec3(8100, 0, 0), //Primus
glm::vec3(7250, 0, 0), //Secundus
glm::vec3(5000.0f,1000.0f,5000.0f), //Missile position should be same as ship position
glm::vec3(4000, 210, 0), //Unum missile 
glm::vec3(7250, 160, 0), //Secundus missile
glm::vec3(4000, 195, 0), //Unum missile site
glm::vec3(7250, 145, 0) //Secundus missile site
}; //initial positions of models

glm::vec3 unumPos = translate[2], duoPos = translate[3]; //Initialize unum and duo positions to original translate from vec3 translate array
glm::vec3 curShipPos = translate[0]; //Initialize ship position to the ship model's translation vector

//glm::mat4 modelMatrix;          // set in display()
glm::mat4 viewMatrix;           // set in init()
glm::mat4 projectionMatrix;     // set in reshape()
glm::mat4 ModelViewProjectionMatrix; // set in display();
glm::mat4 modelViewMatrix;
glm::mat3 normalMatrix;

//array of look at matrices in order to make transitioning from easy
glm::mat4 camera[nCameras] = { 
glm::lookAt(glm::vec3(0.0f, 10000.0f, 20000.0f),glm::vec3(0), glm::vec3(0.0f, 1.0f, 0.0f)), //Front Camera
glm::lookAt(glm::vec3(0.0f, 20000.0f, 0.0f),glm::vec3(0),glm::vec3(0.0f, 0.0f, -1.0f)), //Top Camera 
glm::lookAt(glm::vec3(5000.0f, 1300.0f, 6000.0f), glm::vec3(5000.0f, 1300.0f, 5000.0f), glm::vec3(0.0f, 1.0f, 0.0f)), //Ship Camera
glm::lookAt(glm::vec3(4000.0f, 0.0f, -8000.0f), unumPos, glm::vec3(0.0f, 1.0f, 0.0f)), //Unum Camera
glm::lookAt(glm::vec3(9000.0f, 0.0f, -8000.0f), duoPos, glm::vec3(0.0f, 1.0f, 0.0f)) //Duo Camera
};

glm::mat4 modelMatrix[nModels] = {identity, identity,identity,identity,identity,identity,identity,identity,identity,identity,identity };
glm::mat4 lastOMShip, lastOMDuo, lastOMUnum;

//Window title string variables
char viewStr[6] = "Front";
int wMissileCt = 9, uMissileCt = 5, sMissileCt = 5;
bool wMissileFired = false, uMissileFired = false, sMissileFired = false;
bool uSiteDestroyed = false, sSiteDestroyed = false;
char titleStr[100], fpsStr[5] = { '0' }, timerStr[5] = { '0' };

//Missile variables
const int wMissilesTtl = 9, uMissilesTtl = 5, sMissilesTtl = 5;
bool wDestroyed = false, uMissilesOut = true, sMissilesOut = true;
int missileUpdates = 0, numUpdates = 0;
int currWMissile = 0, currUMissile = 0, currSMissile = 0;
Missile *wMissile, *uMissile, *sMissile;
GLuint NM, MV, HLP, HLI, PLP, PLI, PLO, HLO, AO;
bool pointLightOn = true, headLightOn = true, ambientOn = true;
float boxRotation = 0.0f;

//Box variables
GLuint IsTexture, textIBO, textBuf, textVao, texture, Texture, TexturePosition, vTextCoord;

// Texture files
int width = 512, height = 512;
char * textureFile = "purplenebula_bk.raw";
bool isTexture = false;	

glm::vec3 squareTranslationAmounts[6] = {
	glm::vec3(0.0f, 0.0f, -80000.0f), glm::vec3(0.0f, 0.0f, 80000.0f),
	glm::vec3(0.0f, -80000.0f, 0.0f), glm::vec3(0.0f, 80000.0f, 0.0f),
	glm::vec3(-80000.0f, 0.0f, 0.0f), glm::vec3(80000.0f, 0.0f, 0.0f)
};

static const GLfloat squareVertices[16] = {
	-80000.0f, -80000.0f, 0.0f, 1.0f,	// bottom left vertex
	80000.0f, -80000.0f, 0.0f, 1.0f,	// bottom right vertex
	80000.0f, 80000.0f, 0.0f, 1.0f,		// top right vertex
	-80000.0f, 80000.0f, 0.0f, 1.0f		// top left vertex
};

// texture coordinates
static const GLfloat textCoords[] = {0.0f,0.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1.0f};

// Square information used to draw the Square texture Box for the program //
glm::mat4 squareRotation = glm::mat4();
glm::mat4 translateSquare = glm::mat4();

glm::vec3 squareRotationAxis[6] = {
	glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
};

static const unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };


void specialKeyEvent(int key, int x, int y)
{
if (key == GLUT_KEY_UP && glutGetModifiers() != GLUT_ACTIVE_CTRL)
	player->setMove(1);
else if (key == GLUT_KEY_DOWN && glutGetModifiers() != GLUT_ACTIVE_CTRL)
	player->setMove(-1);
else if (key == GLUT_KEY_RIGHT && glutGetModifiers() != GLUT_ACTIVE_CTRL)
	player->setYaw(1);
else if (key == GLUT_KEY_LEFT && glutGetModifiers() != GLUT_ACTIVE_CTRL)
	player->setYaw(-1);

if (key == GLUT_KEY_UP && glutGetModifiers() == GLUT_ACTIVE_CTRL)
	player->setPitch(-1);
else if (key == GLUT_KEY_DOWN && glutGetModifiers() == GLUT_ACTIVE_CTRL)
	player->setPitch(1);
else if (key == GLUT_KEY_RIGHT && glutGetModifiers() == GLUT_ACTIVE_CTRL)
	player->setRoll(1);
else if (key == GLUT_KEY_LEFT && glutGetModifiers() == GLUT_ACTIVE_CTRL)
	player->setRoll(-1);

}

//Load the shader programs, vertex data from model files, create the solids, set initial view
void init() {
	//Load the shader programs
	shaderProgram = loadShaders(vertexShaderFile, fragmentShaderFile);
	glUseProgram(shaderProgram);

	//Generate VAOs and VBOs
	glGenVertexArrays(nModels, VAO);
	glGenBuffers(nModels, buffer);

	//Load the buffers from the model files
	for (int i = 0; i < nModels; i++) {
		modelBR[i] = loadModelBuffer(modelFile[i], nVertices[i], VAO[i], buffer[i], shaderProgram,
			vPosition[i], vColor[i], vNormal[i], "vPosition", "vColor", "vNormal");

		//Set scale for models given bounding radius  
		scale[i] = glm::vec3(modelSize[i] * 1.0f / modelBR[i]);
	}
	
	//Set render state values
	glEnable(GL_DEPTH_TEST);
	
	// set up the indices buffer
	glGenBuffers(1, &textIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// set up the vertex attributes
	glGenVertexArrays(1, &textVao);
	glBindVertexArray(textVao);

	//  initialize a buffer object
	glGenBuffers(1, &textBuf);
	glBindBuffer(GL_ARRAY_BUFFER, textBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices) + sizeof(textCoords), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(squareVertices), squareVertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(squareVertices), sizeof(textCoords), textCoords);

	// set up vertex arrays
	TexturePosition = glGetAttribLocation(shaderProgram, "vPosition");
	glVertexAttribPointer(TexturePosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(TexturePosition);

	vTextCoord = glGetAttribLocation(shaderProgram, "vTextCoord");
	glVertexAttribPointer(vTextCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(squareVertices)));
	glEnableVertexAttribArray(vTextCoord);

	MVP = glGetUniformLocation(shaderProgram, "MVP");
	NM = glGetUniformLocation(shaderProgram, "NormalMatrix");
	MV = glGetUniformLocation(shaderProgram, "ModelView");
	HLI = glGetUniformLocation(shaderProgram, "HeadLightIntensity");
	HLP = glGetUniformLocation(shaderProgram, "HeadLightPosition");
	PLP = glGetUniformLocation(shaderProgram, "PointLightPosition");
	PLI= glGetUniformLocation(shaderProgram, "PointLightIntensity");
	PLO = glGetUniformLocation(shaderProgram, "PointLightOn");
	HLO = glGetUniformLocation(shaderProgram, "HeadLightOn");
	AO = glGetUniformLocation(shaderProgram, "AmbientOn");
	
	IsTexture = glGetUniformLocation(shaderProgram, "IsTexture");
	glUniform1ui(IsTexture, false);
	// load texture
	texture = loadRawTexture(texture, textureFile, width, height);
	if (texture != 0) {
		Texture = glGetUniformLocation(shaderProgram, "Texture");
		printf("texture file, read, texture %1d generated and bound  \n", Texture);
	}
	else  // texture file loaded
		printf("Texture in file %s NOT LOADED !!! \n");

	player = new Player(curShipPos, scale[0]);
	wMissile = new Missile(translate[6], scale[6], false, 9);
	uMissile = new Missile(translate[7], scale[7], true, 5);
	sMissile = new Missile(translate[8], scale[8], true, 5);

	viewMatrix = camera[0];
	lastOMShip = player->getOM();
	lastOMUnum = rotation[2] * glm::translate(glm::mat4(), translate[2]) * glm::scale(glm::mat4(), glm::vec3(scale[2]));
	lastOMDuo = rotation[3] * glm::translate(glm::mat4(), translate[3]) * glm::scale(glm::mat4(), glm::vec3(scale[3]));
	
	lastTime = glutGet(GLUT_ELAPSED_TIME);  // get elapsed system time	
}

//Method to update objects to new screen size
void reshape(int width, int height) {
	float aspectRatio = (float)width / (float)height, FOVY = glm::radians(60.0f);
	glViewport(0, 0, width, height);
	//printf("reshape: FOVY = %5.2f, width = %4d height = %4d aspect = %5.2f \n", FOVY, width, height, aspectRatio);
	projectionMatrix = glm::perspective(FOVY, aspectRatio, 1.0f, 100000.0f);
}

//Method to update window title text
void updateTitle() {

	if (uSiteDestroyed && sSiteDestroyed) {
		sprintf(titleStr, "Cadet passes flight training!");
	}
	else if (wDestroyed) {
		sprintf(titleStr, "Cadet resigns from War College!");
	}
	else if (wMissileCt > 0) {
		sprintf(titleStr, "Warbird %d  Unum %d  Secundus %d  U/S %s  F/S %s  View %s", wMissileCt, uMissileCt, sMissileCt, timerStr, fpsStr, viewStr);		
	}
	else if (wMissileCt <= 0){
		sprintf(titleStr, "Cadet resigns from War College!");
	}

	glutSetWindowTitle(titleStr);
}

//Method to display items in window
void display() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	viewMatrix = camera[currentCamera];
	glUniform1ui(IsTexture, true);	
	for (int i = 0; i < 6; i++) {
		if (i > 1) { boxRotation = PI /2; }
		else { boxRotation = 0.0f; }

		modelViewMatrix = viewMatrix * glm::translate(translateSquare, squareTranslationAmounts[i])
			* glm::rotate(squareRotation, boxRotation, squareRotationAxis[i]);

		glUniformMatrix4fv(MV, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
		normalMatrix = glm::mat3(modelViewMatrix);
		glUniformMatrix3fv(NM, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		ModelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
		glBindVertexArray(textVao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textIBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	}
	glUniform1ui(IsTexture, false);

	//Update model matrix
	for (int m = 0; m < nModels; m++) {
		//Set the view matrix here so cameras can be dynamic
		
		normalMatrix = glm::mat3(viewMatrix * modelMatrix[m]);
		ModelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix[m];
		glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
		glUniformMatrix3fv(NM, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glUniformMatrix4fv(MV, 1, GL_FALSE, glm::value_ptr(viewMatrix * modelMatrix[m]));
		glUniform3fv(PLP, 1, glm::value_ptr(glm::vec3(0)));
		glUniform3fv(PLI, 1, glm::value_ptr(pointLightIntensity));
		glUniform3fv(HLP, 1, glm::value_ptr(getPosition(viewMatrix)));
		glUniform3fv(HLI, 1, glm::value_ptr(headLightIntensity));
		glUniform1f(PLO, pointLightOn);
		glUniform1f(HLO, headLightOn);
		glUniform1f(AO, ambientOn);
		glBindVertexArray(VAO[m]);
		glDrawArrays(GL_TRIANGLES, 0, nVertices[m]);
	}

	glutSwapBuffers();
	frameCount++;

	//See if a second has passed to set estimated fps information
	currentTime = glutGet(GLUT_ELAPSED_TIME);  // get elapsed system time
	timeInterval = currentTime - lastTime;

	if (timeInterval >= 1000) {
		sprintf(fpsStr, "%4d", (int)(frameCount / (timeInterval / 1000.0f)));
		if (idleTimerFlag)
			sprintf(timerStr, "%s", fpsStr);
		lastTime = currentTime;
		frameCount = 0;
		updateTitle();
	}
}

void update(int i) {
	
	glutTimerFunc(timerDelay, update, 1);
	player->update();
	wMissile->update();
	uMissile->update();
	sMissile->update();
	//Create rotation matrices for each model
	for (int m = 0; m < nModels; m++) { 
		rotation[m] = glm::rotate(rotation[m], rotateRadian[m], glm::vec3(0, 1, 0));
		if (m == 0) {
			modelMatrix[m] = player->getOM(); //Get player's OM matrix and update			
		}
		else if (m == 4 || m == 5) { //for the moons to rotate around Duo equation is different than orbiting around the y axis

			glm::vec3 temp = getPosition(modelMatrix[3]);
			modelMatrix[m] = glm::translate(identity, temp) * rotation[m] * glm::translate(identity, translate[m]) * glm::translate(identity, -1.0f * translate[3]) * glm::scale(glm::mat4(), glm::vec3(scale[m]));
		
		}
		else if (m == 6) {
			if (!wMissile->getFired() ){
				modelMatrix[m] = player->getOM();
			}
			else {
				if (!wMissile->isAlive()) {
					//do nothing
				}
				else {
					if (!wMissile->getTracking()) {
						tarInt = wMissile->setClosestTarget(modelMatrix[9], modelMatrix[10],uSiteDestroyed,sSiteDestroyed);
					}
					else {
						int tempIndex = 8 + tarInt;
						if (wMissile->checkCollision(getPosition(wMissile->getOM()), getPosition(modelMatrix[tempIndex]), modelSize[6], modelSize[tempIndex])) {
							if (tarInt == 1) {
								uSiteDestroyed = true;
							}
							else {
								sSiteDestroyed = true;
							}
						}
						else {
							wMissile->faceTarget(modelMatrix[tempIndex]);
						}
					}
				}
				modelMatrix[m] = wMissile->getOM();
			}
		}
		else if (m == 7) {
			if (!uMissile->getFired()) {
					if (distance(getPosition(player->getOM()), getPosition(modelMatrix[9])) <= 5000.0f) {
						uMissile->fireMissile(identity, glm::translate(identity, getPosition(modelMatrix[9])));
						uMissileCt--;
						uMissile->faceTarget(player->getOM());
					}
				modelMatrix[m] =  glm::translate(identity, getPosition(modelMatrix[9])) * glm::scale(identity,scale[m]);
			}
			else {
				uMissile->faceTarget(player->getOM());
				
				if (uMissile->checkCollision(getPosition(player->getOM()),getPosition(uMissile->getOM()),modelSize[0],modelSize[7])) {
					wDestroyed = true;
				}
				modelMatrix[m] = uMissile->getOM();
			}
		}
		else if (m == 8) {
			if (!sMissile->getFired()) {
				if (distance(getPosition(player->getOM()), getPosition(modelMatrix[10])) <= 5000.0f) {
					sMissile->fireMissile(identity, glm::translate(identity, getPosition(modelMatrix[10])));
					sMissile->faceTarget(player->getOM());
					sMissileCt--;
				}
				modelMatrix[m] = glm::translate(identity, getPosition(modelMatrix[10])) * glm::scale(identity, scale[m]);
			}
			else {
				sMissile->faceTarget(player->getOM());

				if (sMissile->checkCollision(getPosition(player->getOM()), getPosition(sMissile->getOM()), modelSize[0], modelSize[8])) {
					wDestroyed = true;
				}
				modelMatrix[m] = sMissile->getOM();
			}
		}
		else {//Regular equation for rotating around the y-axis
			modelMatrix[m] = rotation[m] * glm::translate(glm::mat4(), translate[m]) * glm::scale(glm::mat4(), scale[m]);
		}
	}
	if (currentCamera == 2) {
		//matrix subtraction: find the difference between the last orientation matrix and the current one of warship
		//then take that difference and multiply it with the camera matrix so the camera follows it 
		camera[2] = camera[2] * (lastOMShip * glm::inverse(player->getOM()));
		lastOMShip = player->getOM();
	}
	//Using rotations to calculate the position and look at of the camera
	if (currentCamera == 3) {
		camera[3] = camera[3] * (lastOMUnum * glm::inverse(modelMatrix[2]));
		lastOMUnum = modelMatrix[2];
	}
	if (currentCamera == 4) {
		camera[4] = camera[4] * (lastOMDuo * glm::inverse(modelMatrix[3]));
		lastOMDuo = modelMatrix[3];
	}
}

//pressing v adds one to the index 
//pressing x subtracts from the index
void keyboard(unsigned char key, int x, int y) {
	
	if (timerDelayCounter >= 4) {
		timerDelayCounter = 0;
	}

	switch (key) {
	case 033: case 'q':  case 'Q': exit(EXIT_SUCCESS); break;
	case 'f': case 'F': //Launch ship missile
		//If in cadet mode, the current fired missile must detonate or collide before firing again
		if (!wMissile->getFired() && wMissile->anyLeft()) {			 
			wMissile->fireMissile(player->getRM(),player->getTM());	
			wMissileCt--;
		} 
		break;
	case 'a': case 'A':
		ambientOn = !ambientOn;
		break;
	case 'h': case 'H':
		headLightOn = !headLightOn;
		break;
	case 'p': case 'P':
		pointLightOn = !pointLightOn;
		break;
	case 'g': case 'G': //Toggle gravity
		player->changeGravity();
		break;

	case 's': case 'S': //For next ship speed % nSpeeds
		player->changeSpeed();
		break;

	case 't': case 'T':  //Change animation timer for pilot mode
		switch (timerDelayCounter) {
		case 0: //Ace Mode
			timerDelay = 5;
			sprintf(timerStr, "%4d", 1000 / timerDelay);
			break;
		case 1: //Pilot Mode
			timerDelay = 40;
			sprintf(timerStr, "%4d", 1000 / timerDelay);
			break;
		case 2: //Trainee Mode
			timerDelay = 100;
			sprintf(timerStr, "%4d", 1000 / timerDelay);
			break;
		case 3: //Debug Mode
			timerDelay = 500;			
			sprintf(timerStr, "%4d", 1000 / timerDelay);			
			break;
		}
		glutIdleFunc(display);
		timerDelayCounter++;
		if (idleTimerFlag) idleTimerFlag = false;
		break;

	case 'w': case 'W': //Warp ship % nPlanets		
		tempVec = glm::normalize(getIn(modelMatrix[nextWarp]));
		tempMat = glm::translate(identity, tempVec * 8000.0f);
		player->warp(modelMatrix[nextWarp],tempMat);
		nextWarp++;
		if (nextWarp >= 4) {
			nextWarp = 2;
		}
		break;

	case 'v': case 'V':  //Front view
		currentCamera++;
		break;
	case 'x': case 'X':  //Bottom view
		currentCamera--;
		break;
	}

	//If currentCamera index is less than 0 set it to the last index if larger than last index go to zero
	if (currentCamera < 0) {currentCamera = nCameras - 1;}
	else if (currentCamera >= nCameras) {currentCamera = 0;}
		if (currentCamera == 0)	{strcpy(viewStr, "Front");}
		else if (currentCamera == 1){ strcpy(viewStr, "Top");}
		else if (currentCamera == 2){ strcpy(viewStr, "Ship");}
		else if (currentCamera == 3){ strcpy(viewStr, "Unum");}
		else if (currentCamera == 4){ strcpy(viewStr, "Duo");}

	updateTitle();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);

	# ifdef __Mac__
		// Can't change the version in the GLUT_3_2_CORE_PROFILE
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
	# endif

	# ifndef __Mac__
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	# endif

	glutInitWindowSize(800, 600);

	//Set OpenGL and GLSL versions to 3.3 for Comp 465/L, comment to see highest versions
	# ifndef __Mac__
		glutInitContextVersion(3, 3);
		glutInitContextProfile(GLUT_CORE_PROFILE);
	# endif

	sprintf(titleStr, "Warbird %d  Unum %d  Secundus %d  U/S %s  F/S %s  View %s", wMissileCt, uMissileCt, sMissileCt, timerStr, fpsStr, viewStr);
	glutCreateWindow(titleStr);
	
	//Initialize and verify glew
	glewExperimental = GL_TRUE;  // needed my home system 
	GLenum err = glewInit();

	if (GLEW_OK != err)
		printf("GLEW Error: %s \n", glewGetErrorString(err));
	else {
		printf("Using GLEW %s \n", glewGetString(GLEW_VERSION));
		printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
	}

	//Initialize scene
	init();

	//Set title string to start with 
	sprintf(timerStr, "%4d", 1000 / timerDelay);

	//Set glut callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyEvent);

	//I still need to see exactly what we need to update here
	glutTimerFunc(timerDelay, update, 1);
	glutIdleFunc(display);
	glutMainLoop();

	printf("done\n");
	return 0;
}





