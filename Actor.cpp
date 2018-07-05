#include "Actor.hpp"

Actor::Actor() 
{

};


Actor::Actor(GLfloat scale, GLfloat offsetX, GLfloat offsetY)
{
  this->scale = scale;
  this->offsetX = offsetX;
  this->offsetY = offsetY;
};

vector<Vertex> Actor::getVertices() 
{
  return this->vertices;
};

Vertex Actor::getVertex(GLint i)
{
  return (this->vertices)[i];
};

void Actor::draw() 
{
  glBegin(GL_POLYGON);
  for (vector<Vertex>::iterator vertex = this->vertices.begin(); vertex != this->vertices.end(); ++vertex)
  {
    glVertex2f((vertex->x * this->scale) + offsetX, (vertex->y * this->scale) + offsetY);
  }
  glEnd();
}