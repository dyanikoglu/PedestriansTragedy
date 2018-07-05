#pragma once
#include "Actor.hpp"

// Objects controlled by player
class Agent : public Actor {
public:
  enum AGENT_STATUS
  {
    MOVING=1,
    IDLE=0,
  };

  GLfloat stepSize; // Should be equal to square size of world layout
  DIRECTION direction;
  GLfloat targetX;
  GLfloat targetY;
  GLfloat initialScale;
  GLfloat targetScale;
  GLint currentRow;
  AGENT_STATUS stateX;
  AGENT_STATUS stateY;
  vector<Vertex> blockedSquares; // Movement blocked squares on game layout
  GLint triedToGoBack; // Agent tried to go wrong direction?
  GLint deathAnimFlag;
  GLfloat ww;
  GLfloat wh;

  vector<Vertex> const rotate_up = { Vertex(0.2,0.1), Vertex(0.5,0.9), Vertex(0.8,0.1) };
  vector<Vertex> const rotate_down = { Vertex(0.2,0.9), Vertex(0.5,0.1), Vertex(0.8,0.9) };
  vector<Vertex> const rotate_left = { Vertex(0.9,0.2), Vertex(0.1,0.5), Vertex(0.9,0.8) };
  vector<Vertex> const rotate_right = { Vertex(0.1,0.2), Vertex(0.9,0.5), Vertex(0.1,0.8) };

  Agent(GLfloat stepSize, GLfloat scale, GLfloat offsetX, GLfloat offsetY, vector<Vertex> blockedSquares, GLfloat ww, GLfloat wh);

  GLint step(DIRECTION direction);

  // Move agent with a smooth animation
  void move();

  vector<Vertex> getCollisionBound(COLLISION_TYPE collisionType);

  void draw();
};