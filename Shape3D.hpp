/*
Shape3D.cpp

Represent the scale, translation, and rotation of a 3D shape.
If you overload the constructor you can create a shape with
arguments for scale, translation, and rotation.

Mike Barnes
8/24/2014
*/

# ifndef __INCLUDES465__
# include "../includes465/include465.hpp"
# define __INCLUDES465__
# endif

class Shape3D {

private :

  int id;
  glm::mat4 rotationMatrix;
  glm::mat4 scaleMatrix;
  glm::mat4 translationMatrix;
  glm::vec3 rotationAxis; 
  float radians; 
  bool orbital;

public:

  Shape3D(int number) {
    id = number;  // for debugging
    int random = rand();   // random num 0..MAXINT
    switch(random %4) { // make model larger
      case 0  : scaleMatrix = glm::scale(glm::mat4(), glm::vec3(20, 20, 20));  break;
      case 1  : scaleMatrix = glm::scale(glm::mat4(), glm::vec3(20, 30, 40));  break;
      case 2  : scaleMatrix = glm::scale(glm::mat4(), glm::vec3(30, 40, 20));  break;
      case 3  : scaleMatrix = glm::scale(glm::mat4(), glm::vec3(40, 20, 30));  break;
      default : printf("Shape:: randomRotate selection error\n"); 
                exit(-1); break;
      }
    rotationMatrix = glm::mat4();  // no initial orientation
    //set cube's  rotation axis and rotation radians
    switch (random % 3) {
      case 0  : rotationAxis = glm::vec3(1,  0,  0); break;
      case 1  : rotationAxis = glm::vec3(0,  1,  0); break;
      case 2  : rotationAxis = glm::vec3(0,  0, -1); break;
      default : printf("Shape:: randomRotate selection error\n"); 
                exit(-1); break;
      }
    // rotate between 0.1 and 2.0 degrees
    radians = glm::radians(0.1f + (random % 20) / 10.0f );
    // random initial placement +/- 500 from origin in X, Y, Z
    translationMatrix = glm::translate(glm::mat4(), 
      glm::vec3(-500 + rand() % 1000, -500 + rand() % 1000, -500 + rand() % 1000));
    // determine rotation type
    if (random %2 == 0)
        orbital = true;
      else
        orbital = false;
    }

  glm::mat4 getModelMatrix() {
    if (orbital) // orbital rotation
        return(rotationMatrix * translationMatrix * scaleMatrix);
      else  // center rotation
        return(translationMatrix * rotationMatrix * scaleMatrix);
    }

  void update() {
    rotationMatrix = glm::rotate(rotationMatrix, radians, rotationAxis);
    //translationMatrix = glm::translate(translationMatrix, translation);
    }   
  };  