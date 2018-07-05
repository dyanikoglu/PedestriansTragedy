#include "Coin.hpp"
#include "Tools.hpp"

Coin::Coin(GLfloat radius, GLfloat offsetX, GLfloat offsetY, GLint timeout, GLint points) : Actor(radius, offsetX, offsetY) 
{
  this->initialX = offsetX;
  this->targetX = offsetX + 10;
  this->timeout = timeout;
  this->points = points;
  this->isTaken = 0;
  this->targetRadius = radius + 10;
}

void Coin::idle()
{
  if (abs(this->offsetX - this->targetX) > 1) 
  {
    this->offsetX = Tools::lerp(this->offsetX, this->targetX, 0.01);
  }
  else if (this->targetX == this->initialX + 10)
  {
    this->targetX = this->initialX;
  }
  else 
  {
    this->targetX = this->initialX + 10;
  }
}

void Coin::taken() 
{
  this->offsetX = Tools::lerp(this->offsetX, this->targetX, 0.05);
  this->offsetY = Tools::lerp(this->offsetY, this->targetY, 0.05);
  this->scale = Tools::lerp(this->scale, this->targetRadius, 0.05);
}

void Coin::draw()
{
  glColor3f(1.0, 1.0, 0.0);
  Tools::drawCircle(this->offsetX, this->offsetY, this->scale);
}