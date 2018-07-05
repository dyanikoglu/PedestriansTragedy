
#include "Tools.hpp"
#include "Agent.hpp"

Agent::Agent(GLfloat stepSize, GLfloat scale, GLfloat offsetX, GLfloat offsetY, vector<Vertex> blockedSquares, GLfloat ww, GLfloat wh) : Actor(scale, offsetX, offsetY)
{
  this->wh = wh;
  this->ww = ww;
  this->targetX = offsetX;
  this->targetY = offsetY;
  this->initialScale = scale;
  this->stepSize = stepSize;
  this->direction = DIRECTION::UP;
  this->vertices = rotate_up;
  this->blockedSquares = blockedSquares;
  this->targetScale = scale * 1.75;
  this->deathAnimFlag = 0;
  this->currentRow = 0;
  this->stateX = AGENT_STATUS::IDLE;
  this->stateY = AGENT_STATUS::IDLE;
  this->triedToGoBack = 0;
}

GLint Agent::step(DIRECTION direction)
{
  if (direction == DIRECTION::LEFT) 
  {
    // Decline input if if target location is blocked
    for (vector<Vertex>::iterator move = this->blockedSquares.begin(); move != this->blockedSquares.end(); ++move)
    {
      if (move->x == offsetX - stepSize && move->y == this->offsetY)
      {
        return 0;
      }
    }

    if (this->stateX != AGENT_STATUS::MOVING && this->offsetX > 0)
    {
      this->vertices = rotate_left;
      this->targetX = this->offsetX - stepSize;
      this->stateX = AGENT_STATUS::MOVING;
      return 1;
    }
  }
  else if (direction == DIRECTION::RIGHT)
  {
    // Decline input if if target location is blocked
    for (vector<Vertex>::iterator move = this->blockedSquares.begin(); move != this->blockedSquares.end(); ++move) 
    {
      if (move->x == offsetX + stepSize && move->y == this->offsetY) 
      {
        return 0;
      }
    }

    if (this->stateX != AGENT_STATUS::MOVING && this->offsetX < ww - stepSize) 
    {
      this->vertices = rotate_right;
      this->targetX = this->offsetX + stepSize;
      this->stateX = AGENT_STATUS::MOVING;
      return 1;
    }
  }
  else if (direction == DIRECTION::UP)
  {
    // Decline input if if target location is blocked
    for (vector<Vertex>::iterator move = this->blockedSquares.begin(); move != this->blockedSquares.end(); ++move)
    {
      if (move->x == offsetX && move->y == this->offsetY + stepSize) 
      {
        return 0;
      }
    }

    if (this->stateY != AGENT_STATUS::MOVING && this->offsetY < wh - stepSize) 
    {
      this->vertices = rotate_up;
      this->targetY = this->offsetY + stepSize;
      // Time to change direction of agent
      if (this->targetY == wh - stepSize) 
      {
        this->vertices = rotate_down;
        this->direction = DIRECTION::DOWN;
      }
      this->stateY = AGENT_STATUS::MOVING;
      this->currentRow++;
      return 1;
    }
  }
  else if (direction == DIRECTION::DOWN)
  {
    // Decline input if if target location is blocked
    for (vector<Vertex>::iterator move = this->blockedSquares.begin(); move != this->blockedSquares.end(); ++move)
    {
      if (move->x == offsetX && move->y == this->offsetY - stepSize) 
      {
        return 0;
      }
    }

    if (this->stateY != AGENT_STATUS::MOVING && this->direction == DIRECTION::DOWN && this->offsetY > 0) 
    {
      this->vertices = rotate_down;
      this->targetY = this->offsetY - stepSize;
      // Time to change direction of agent
      if (this->targetY == 0) 
      {
        this->vertices = rotate_up;
        this->direction = DIRECTION::UP;
      }
      this->stateY = AGENT_STATUS::MOVING;
      this->currentRow--;
      return 1;
    }
  }
}

void Agent::move()
{
  if (this->stateY == AGENT_STATUS::MOVING) 
  {
    if (abs(this->offsetY - this->targetY) > 1)
    {
      this->offsetY = Tools::lerp(this->offsetY, this->targetY, 0.4);
    }
    else 
    {
      this->offsetY = this->targetY;
      this->stateY = AGENT_STATUS::IDLE;
    }
  }
  if (this->stateX == AGENT_STATUS::MOVING) 
  {
    if (abs(this->offsetX - this->targetX) > 1)
    {
      this->offsetX = Tools::lerp(this->offsetX, this->targetX, 0.4);
    }
    else
    {
      this->offsetX = this->targetX;
      this->stateX = AGENT_STATUS::IDLE;
    }
  }
}

vector<Vertex> Agent::getCollisionBound(COLLISION_TYPE collisionType)
{
  vector<Vertex> tempVertices;
  for (vector<Vertex>::iterator vertex = this->vertices.begin(); vertex != this->vertices.end(); ++vertex)
  {
    if (collisionType == COLLISION_TYPE::LOOSE) 
    {
      tempVertices.push_back(Vertex((vertex->x * (this->scale + 12.5)) + this->offsetX - 6.25, (vertex->y * this->scale) + this->offsetY));
    }
    else if (collisionType == COLLISION_TYPE::TIGHT)
    {
      tempVertices.push_back(Vertex((vertex->x * this->scale) + this->offsetX, (vertex->y * this->scale) + this->offsetY));
    }
  }
  return tempVertices;
}

void Agent::draw()
{
  glColor3f(0.8, 0.0, 0);
  Actor::draw();
}