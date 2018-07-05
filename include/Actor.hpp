#pragma once
#include <vector>
#include <GL/glut.h>
#include "Vertex.hpp"

using namespace std;

// Parent class for dynamic objects in game world
class Actor {
public:
  enum DIRECTION
  {
    UP=2,
    DOWN=-2,
    LEFT=-1,
    RIGHT=1
  };

  enum COLLISION_TYPE
  {
    LOOSE=0,
    TIGHT=1,
    LONG=2
  };

  vector<Vertex> vertices; // Unit polygon (It's like a miniature polygon, it's scaled and moved into right direction with offset values)
  GLfloat scale;
  GLfloat offsetX;
  GLfloat offsetY;

  Actor();
  Actor(GLfloat scale, GLfloat offsetX, GLfloat offsetY);
  vector<Vertex> getVertices();
  Vertex getVertex(GLint i);
  void draw();
};