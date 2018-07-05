#pragma once
#include <GL/glut.h>

// Main class for vertex structure used in game world
class Vertex {
public:
  GLfloat x;
  GLfloat y;
  Vertex(GLfloat x, GLfloat y);
};