#pragma once
#include "Actor.hpp"

// Coin class
class Coin :public Actor {
public:
  GLint timeout;
  GLfloat initialX;
  GLfloat targetX;
  GLfloat targetY;
  GLfloat targetRadius;
  GLint points;
  GLint isTaken;
  Coin(GLfloat radius, GLfloat offsetX, GLfloat offsetY, GLint timeout, GLint points);

  // Idle animation of coin
  void idle();

  // Pick up animation of coin
  void taken();

  void draw();
};