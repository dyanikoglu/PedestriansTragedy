#include "Tools.hpp"
#include "WorldObject.hpp"

WorldObject::WorldObject(vector<Vertex> vertices, TYPE objectType)
{
  this->objectType = objectType;
  if (objectType == TYPE::PAVEMENT)
  {
    this->vertices = vertices;
  }
  else if (objectType == TYPE::ROAD)
  {
    this->vertices = vertices;
  }
  else if (objectType == TYPE::FLORA_TREE_1 || objectType == TYPE::FLORA_TREE_2)
  {
    this->offsetX = vertices.at(0).x;
    this->offsetY = vertices.at(0).y;
  }
}

void WorldObject::draw() 
{
  if (objectType == TYPE::PAVEMENT)
  {
    glColor3f(0.0f, 1.0f, 0.0f);
  }
  else if (objectType == TYPE::ROAD)
  {
    glColor3f(0.5f, 0.5f, 0.5f);
  }
  else if (objectType == TYPE::FLORA_TREE_1)
  {
    GLint r = 20;
    glColor3f(0, 0.5f, 0);
    Tools::drawCircle(this->offsetX, this->offsetY, r);

    glBegin(GL_TRIANGLES);
    glVertex2f(this->offsetX + r / 2, this->offsetY + 0.8*r);
    glVertex2f(this->offsetX + r / 2, this->offsetY - 0.8*r);
    glVertex2f(this->offsetX + 2 * r, this->offsetY);

    glVertex2f(this->offsetX - r / 2, this->offsetY - 0.8*r);
    glVertex2f(this->offsetX - r / 2, this->offsetY + 0.8*r);
    glVertex2f(this->offsetX - 2 * r, this->offsetY);

    glVertex2f(this->offsetX + 0.8*r, this->offsetY + r / 2);
    glVertex2f(this->offsetX - 0.8*r, this->offsetY + r / 2);
    glVertex2f(this->offsetX, this->offsetY + 2 * r);

    glVertex2f(this->offsetX - 0.8*r, this->offsetY - r / 2);
    glVertex2f(this->offsetX + 0.8*r, this->offsetY - r / 2);
    glVertex2f(this->offsetX, this->offsetY - 2 * r);
    glEnd();

    glColor3f(0.2f, 0.0f, 0);
    glBegin(GL_POINTS);
    glVertex2f(this->offsetX, this->offsetY);
    glEnd();
    return;
  }

  else if (objectType == TYPE::FLORA_TREE_2)
  {
    GLint r = 20;
    glColor3f(0, 0.5f, 0);
    Tools::drawCircle(this->offsetX, this->offsetY, r);

    Tools::drawCircle(this->offsetX + r, this->offsetY, r / 2);
    Tools::drawCircle(this->offsetX + r, this->offsetY + r / 1.2, r / 2);
    Tools::drawCircle(this->offsetX - r, this->offsetY, r / 2);
    Tools::drawCircle(this->offsetX - r, this->offsetY + r / 1.2, r / 2);
    Tools::drawCircle(this->offsetX, this->offsetY + r, r / 2);
    Tools::drawCircle(this->offsetX + r, this->offsetY - r / 1.2, r / 2);
    Tools::drawCircle(this->offsetX - r, this->offsetY - r / 1.2, r / 2);
    Tools::drawCircle(this->offsetX, this->offsetY - r, r / 2);

    glColor3f(0.2f, 0.0f, 0);
    glBegin(GL_POINTS);
    glVertex2f(this->offsetX, this->offsetY);
    glEnd();
    return;
  }

  glBegin(GL_POLYGON);
  for (vector<Vertex>::iterator vertex = this->vertices.begin(); vertex != this->vertices.end(); ++vertex) 
  {
    glVertex2f(vertex->x, vertex->y);
  }
  glEnd();
}