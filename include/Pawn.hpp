#pragma once
#include "Actor.hpp"

// Objects non-controlled by player
class Pawn :public Actor {
public:
  enum TYPE
  {
    CAR=0,
    TRUCK=1
  };

  GLint assignedTo; // Which row is assigned to this pawn
  GLfloat velocity;
  DIRECTION direction;
  GLint color;
  TYPE type;
  GLint honked; // Is this pawn tried honking it's horn?

  Pawn(GLint difficulty, GLfloat scale, GLfloat offsetX, GLfloat offsetY, GLint assignedTo, DIRECTION direction);

  void move();

  void draw();

  // Pull the pawn a little back in the reverse direction of it's velocity, required for pawn-pawn collision handling
  void pullBack();

  vector<Vertex> getCollisionBound(COLLISION_TYPE collisionType);
};