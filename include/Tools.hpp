#pragma once
#include <gl/freeglut.h>
#include <vector>
#include "Vertex.hpp"

using namespace std;

static class Tools {
public:
  // Main method for smooth animations seen in the game, interpolates v0 to v1 on delta value
  static float lerp(GLfloat v0, GLfloat v1, GLfloat delta) {
    return (1 - delta) * v0 + delta * v1;
  }

  // Classic orientation function
  static GLint getOrientation(Vertex p1, Vertex p2, Vertex p3) {
    GLint result = (p2.y - p1.y) * (p3.x - p2.x) - (p2.x - p1.x) * (p3.y - p2.y);
    if (result == 0) {
      return 0; // Colinear
    }
    return (result > 0) ? 1 : 2;
  }

  // Checks if point is in polygon
  static GLint contains(Vertex point, vector<Vertex> poly) {
    Vertex startpoint = poly.at(0);
    Vertex endpoint = poly.at((1) % poly.size());
    GLint currOri = getOrientation(startpoint, endpoint, point);
    for (int i = 1; i < poly.size(); i++) {
      Vertex startpoint = poly.at(i);
      Vertex endpoint = poly.at((i + 1) % poly.size());
      GLint tempOri = getOrientation(startpoint, endpoint, point);
      if (tempOri != currOri) {
        // Point is outside of polygon
        return 0;
      }
      currOri = tempOri;
    }
    // Point is inside polygon
    return 1;
  }

  // Check each line segment of different polygons with each other, if specified orientations are different or zero, this means they're colliding.
  static GLint checkIntersection(vector<Vertex> p1, vector<Vertex> p2) {
    for (GLint i = 0; i < p1.size(); i++) {
      for (GLint j = 0; j < p2.size(); j++) {
        // From first polygon
        GLint ori1 = getOrientation(p1[i], p1[(i + 1) % p1.size()], p2[j]);
        GLint ori2 = getOrientation(p1[i], p1[(i + 1) % p1.size()], p2[(j + 1) % p2.size()]);

        // No need to check "0 orientation"

        if (ori1 != ori2) {
          // From second polygon
          GLint ori3 = getOrientation(p2[j], p2[(j + 1) % p2.size()], p1[i]);
          GLint ori4 = getOrientation(p2[j], p2[(j + 1) % p2.size()], p1[(i + 1) % p1.size()]);

          if (ori3 != ori4) {
            // Intersection detected
            return 1;
          }
        }
      }
    }
    // No intersection
    return 0;
  }

  // Draws a circle with r radius
  static void drawCircle(GLfloat x, GLfloat y, GLfloat r) {
    GLint i;
    GLint triangleAmount = 1000;
    GLfloat twicePi = 2.0f * 3.14;

    glEnable(GL_LINE_SMOOTH);

    glBegin(GL_LINES);
    for (i = 0; i <= triangleAmount; i++)
    {
      glVertex2f(x, y);
      glVertex2f(x + (r * cos(i * twicePi / triangleAmount)), y + (r * sin(i * twicePi / triangleAmount)));
    }
    glEnd();
  }

  // Draws a bitmap text on given coordinates of screen with given font as parameters
  static void drawText(char *txt, GLfloat x, GLfloat y, void* font) {
    char text[32];
    sprintf(text, "%s", txt);
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++)
      glutBitmapCharacter(font, text[i]);
  }
};