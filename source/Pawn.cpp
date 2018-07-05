#include "Pawn.hpp"

Pawn::Pawn(GLint difficulty, GLfloat scale, GLfloat offsetX, GLfloat offsetY, GLint assignedTo, DIRECTION direction) : Actor(scale, offsetX, offsetY)
{
  this->assignedTo = assignedTo;
  this->direction = direction;
  this->velocity = ((rand() % 6) + difficulty) * direction;
  this->color = rand() % 6;
  this->type = rand() % 2 == 0 ? TYPE::CAR : TYPE::TRUCK;

  this->honked = 0;

  if (type == TYPE::CAR) 
  {
    if (direction == DIRECTION::RIGHT) 
    {
      this->vertices = { Vertex(0, 0.25), Vertex(0, 0.75), Vertex(0.05, 0.80), Vertex(0.6, 0.80), Vertex(1, 0.70), Vertex(1, 0.30), Vertex(0.6, 0.20), Vertex(0.05, 0.20) };
    }
    else if (direction == DIRECTION::LEFT) 
    {
      this->vertices = { Vertex(1, 0.25), Vertex(1, 0.75), Vertex(0.95, 0.80), Vertex(0.4, 0.80), Vertex(0, 0.70), Vertex(0, 0.30), Vertex(0.4, 0.20), Vertex(0.95, 0.20) };
    }
  }
  else if (type == TYPE::TRUCK)
  {
    if (direction == DIRECTION::RIGHT)
    {
      this->vertices = { Vertex(0, 0.25), Vertex(0, 0.75), Vertex(0.05, 0.80), Vertex(1.6, 0.80), Vertex(2, 0.70), Vertex(2, 0.30), Vertex(1.6, 0.20), Vertex(0.05, 0.20) };
    }
    else if (direction == DIRECTION::LEFT)
    {
      this->vertices = { Vertex(2, 0.25), Vertex(2, 0.75), Vertex(1.95, 0.80), Vertex(0.4, 0.80), Vertex(0, 0.70), Vertex(0, 0.30), Vertex(0.4, 0.20), Vertex(1.95, 0.20) };
    }
  }
}

void Pawn::move()
{
  this->offsetX += velocity;
}

void Pawn::draw() 
{
  switch (this->color)
  {
  case 0:
    glColor3f(0.0, 0.0, 1.0);
    break;
  case 1:
    glColor3f(0.0, 0.0, 0.0);
    break;
  case 2:
    glColor3f(1.0, 1.0, 1.0);
    break;
  case 3:
    glColor3f(1.0, 0.75, 0.0);
    break;
  case 4:
    glColor3f(1.0, 0.0, 1.0);
    break;
  case 5:
    glColor3f(0.0, 1.0, 1.0);
    break;
  }
  Actor::draw();
}

void Pawn::pullBack()
{
  this->offsetX -= this->velocity;
}

vector<Vertex> Pawn::getCollisionBound(COLLISION_TYPE collisionType) 
{
  vector<Vertex> tempVertices;
  for (vector<Vertex>::iterator vertex = this->vertices.begin(); vertex != this->vertices.end(); ++vertex) 
  {
    if (collisionType == COLLISION_TYPE::LOOSE)
    {
      if (this->type == TYPE::CAR)
        tempVertices.push_back(Vertex((vertex->x * (this->scale + 25)) + this->offsetX - 12.5, (vertex->y * this->scale) + this->offsetY));
      else if (this->type == TYPE::TRUCK)
        tempVertices.push_back(Vertex((vertex->x * (this->scale + 12.5)) + this->offsetX - 6.25, (vertex->y * this->scale) + this->offsetY));
    }
    else if (collisionType == COLLISION_TYPE::TIGHT) 
    {
      tempVertices.push_back(Vertex((vertex->x * this->scale) + this->offsetX, (vertex->y * this->scale) + this->offsetY));
    }
    else if (collisionType == COLLISION_TYPE::LONG) 
    {
      if (this->type == TYPE::CAR)
      {
        if (this->direction == DIRECTION::LEFT) 
        {
          tempVertices.push_back(Vertex((vertex->x * (this->scale + 50)) + this->offsetX - 100, (vertex->y * this->scale) + this->offsetY));
        }
        else
        {
          tempVertices.push_back(Vertex((vertex->x * (this->scale + 50)) + this->offsetX + 100, (vertex->y * this->scale) + this->offsetY));
        }
      }
      else if (this->type == TYPE::TRUCK)
      {
        if (this->direction == DIRECTION::LEFT)
        {
          tempVertices.push_back(Vertex((vertex->x * (this->scale + 25)) + this->offsetX - 50, (vertex->y * this->scale) + this->offsetY));
        }
        else 
        {
          tempVertices.push_back(Vertex((vertex->x * (this->scale + 25)) + this->offsetX + 50, (vertex->y * this->scale) + this->offsetY));
        }
      }
    }
  }
  return tempVertices;
}
