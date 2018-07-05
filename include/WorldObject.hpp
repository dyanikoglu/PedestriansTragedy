#pragma once
#include <vector>
#include <gl/freeglut.h>
#include "Vertex.hpp"

// Static objects in game world
class WorldObject {
public:
  enum TYPE
  {
    PAVEMENT=1,
    FLORA_TREE_1=3,
    FLORA_TREE_2=4,
    ROAD=2,
  };

  TYPE objectType;
  vector<Vertex> vertices;
  GLint offsetX;
  GLint offsetY;

  WorldObject(vector<Vertex> vertices, TYPE objectType);
  void draw();
};