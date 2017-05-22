#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <GL/glut.h>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>

GLint const DIRECTION_LEFT = -1;
GLint const DIRECTION_RIGHT = 1;
GLint const DIRECTION_UP = 2;
GLint const DIRECTION_DOWN = -2;

GLint const TYPE_PAVEMENT = 1;
GLint const TYPE_ROAD = 2;

GLint const COLLISION_PAWN_PAWN = 0;
GLint const COLLISION_PAWN_AGENT = 1;
GLint const COLLISION_PAWN_WORLDOBJECT = 2;

using namespace std;

GLsizei wh = 600, ww = 500; /* initial window size */

class Vertex {
	public:
		GLfloat x;
		GLfloat y;
		Vertex(GLfloat x, GLfloat y) {
			this->x = x;
			this->y = y;
		};
};

GLint getOrientation(Vertex p1, Vertex p2, Vertex p3) {
	GLint result = (p2.y - p1.y) * (p3.x - p2.x) - (p2.x - p1.x) * (p3.y - p2.y);
	if (result == 0) {
		return 0; // Colinear
	}
	return (result > 0) ? 1 : 2;
}

// Check each line segment of different polygons with each other, if specified orientations are different or zero, this means they're colliding.
GLint checkIntersection(vector<Vertex> p1, vector<Vertex> p2) {
	for (int i = 0; i < p1.size(); i++) {
		for (int j = 0; j < p2.size(); j++) {
			// From first polygon
			GLint ori1 = getOrientation(p1[i], p1[(i + 1) % p1.size()], p2[j]);
			GLint ori2 = getOrientation(p1[i], p1[(i + 1) % p1.size()], p2[(j+1) % p2.size()]);

			// TODO Add check for 0 orientation, removed for pawns because they're parallel
			if (ori1 != ori2) {
				// From second polygon
				GLint ori3 = getOrientation(p2[j], p2[(j + 1) % p2.size()], p1[i]);
				GLint ori4 = getOrientation(p2[j], p2[(j + 1) % p2.size()], p1[(i + 1) % p1.size()]);

				if (ori3 != ori4) {
					return 1;
				}
			}
		}
	}
	return 0;
}


class Actor {
	public:
		vector<Vertex> vertices; // Unit polygon
		GLfloat scale;
		GLfloat offsetX;
		GLfloat offsetY;
		Actor() {};
		Actor(vector<Vertex> vertices, GLfloat scale, GLfloat offsetX, GLfloat offsetY) {
			this->scale = scale;
			this->vertices = vertices;
			this->offsetX = offsetX;
			this->offsetY = offsetY;
		};
		vector<Vertex> getVertices() {
			return this->vertices;
		};
		Vertex getVertex(int i) {
			return (this->vertices)[i];
		};

		void draw() {
			// Collision Bounds
			/*glBegin(GL_POLYGON);
			for (vector<Vertex>::iterator vertex = this->vertices.begin(); vertex != this->vertices.end(); ++vertex) {
				glColor3f(1, 0, 0);
				glVertex2f((vertex->x * (this->scale + 25)) + offsetX - 12.5, (vertex->y *  (this->scale)) + offsetY );
			}
			glEnd();*/

			// Actual object
			glBegin(GL_POLYGON);
			for (vector<Vertex>::iterator vertex = this->vertices.begin(); vertex != this->vertices.end(); ++vertex) {
				//glColor3f(0, 0, 1);		
				glVertex2f((vertex->x * this->scale) + offsetX, (vertex->y * this->scale) + offsetY);
			}
			glEnd();
		}
};

// Objects controlled by player
class Agent: public Actor {
	public:
		GLfloat stepSize = 25;
		Agent(vector<Vertex> vertices, GLfloat scale, GLfloat offsetX, GLfloat offsetY) : Actor(vertices, scale, offsetX, offsetY) {};
		void step(int direction) {
			if (direction == DIRECTION_LEFT) {
				this->offsetX -= stepSize;
			}
			else if (direction == DIRECTION_RIGHT) {
				this->offsetX += stepSize;
			}
			else if (direction == DIRECTION_UP) {
				this->offsetY += stepSize;
			}
			else if (direction == DIRECTION_DOWN) {
				this->offsetY -= stepSize;
			}
		}

		void draw() {
			glColor3f(0.7, 0.0, 0);
			Actor::draw();
		}
};

// Objects non-controlled by player
class Pawn :public Actor {
	public:
		GLint assignedTo;
		GLfloat velocity;
		GLint direction;
		GLint color;
		Pawn(vector<Vertex> vertices, GLfloat scale, GLfloat offsetX, GLfloat offsetY, GLint assignedTo, GLint direction) : Actor(vertices, scale, offsetX, offsetY) {
			this->assignedTo = assignedTo;
			this->direction = direction;
			this->velocity = ((rand() % 6) + 3) * direction;
			this->color = rand() % 3;
		};
		void move() {
			this->offsetX += velocity;
		}

		void draw() {
			switch (this->color) {
				case 0:
					glColor3f(0.0, 0.0, 1.0);
					break;
				case 1:
					glColor3f(0.0, 0.0, 0.0);
					break;
				case 2:
					glColor3f(1.0, 1.0, 1.0);
					break;
			}
			Actor::draw();
		}

		// Pull the pawn a little back in the reverse direction of it's velocity
		void pullBack() {
			this->offsetX -= this->velocity;
		}

		vector<Vertex> getCollisionBound() {
			vector<Vertex> tempVertices;
			for (vector<Vertex>::iterator vertex = this->vertices.begin(); vertex != this->vertices.end(); ++vertex) {
				tempVertices.push_back(Vertex((vertex->x * (this->scale + 25)) + this->offsetX - 12.5, (vertex->y * this->scale) + this->offsetY));
			}
			return tempVertices;
		}
};

// Static objects in game world
class WorldObject {
	GLint objectType;
	vector<Vertex> vertices;
	public:
		WorldObject(vector<Vertex> vertices, GLint objectType) {
			this->objectType = objectType;
			this->vertices = vertices;
			if (objectType == TYPE_PAVEMENT) {
			}
			else if (objectType == TYPE_ROAD) {
			}
		}

		void draw(GLint startY, GLint squareSize) {
			if (objectType == TYPE_PAVEMENT) {
				glColor3f(0, 1, 0);
			}
			else {
				glColor3f(0.5, 0.5, 0.5);
			}

			glBegin(GL_POLYGON);
			for (vector<Vertex>::iterator vertex = this->vertices.begin(); vertex != this->vertices.end(); ++vertex) {
				glVertex2f(vertex->x, vertex->y);
			}
			glEnd();
		}
};


// Main world object
class World {
	public:
		Agent *agent;
		vector<WorldObject*> worldObjects;
		GLint* worldLayout;
		GLint* roadStatus;
		GLint* roadDirection;
		GLint* spawnTimeStamps;
		vector<Pawn*> pawns;
		GLint squareSize = 25;
		GLint leftSpawnPoint = -2 * squareSize;
		GLint rightSpawnPoint = ww + 2 * squareSize;
		GLint frameTimer = 0;

		void initWorld() {
			srand(time(NULL));
			worldLayout = new GLint[wh / this->squareSize];
			roadStatus = new GLint[wh / this->squareSize];
			roadDirection = new GLint[wh / this->squareSize];
			spawnTimeStamps = new GLint[wh / this->squareSize];

			// Create player agent
			vector<Vertex> initials;
			initials = { Vertex(0.2,0.1), Vertex(0.5,0.9), Vertex(0.8,0.1) };
			agent = new Agent(initials, squareSize, 0, 0);

			// Decide world layout
			// TODO Check if lane and pavement count is enough, if not loop again. Store road and pavement counts in world.
			for (int i = 0; i < (wh / this->squareSize);i++) {
				int randNum = rand() % 10;
				initials = { Vertex(0,i*squareSize), Vertex(0,(i+1)*squareSize), Vertex(wh,(i + 1)*squareSize), Vertex(wh,i*squareSize) };
				// First & Last row is always pavement, pavement cannot repeat in two consecutive rows
				if (i == 0 || i == (wh / this->squareSize) - 1 || (this->worldLayout[i-1] != TYPE_PAVEMENT && randNum < 4)) {
					worldLayout[i] = TYPE_PAVEMENT;
					roadStatus[i] = -1;
					roadDirection[i] = 0;
					this->worldObjects.push_back(new WorldObject(initials, TYPE_PAVEMENT));
				}
				else {
					worldLayout[i] = TYPE_ROAD;
					roadStatus[i] = 1;
					this->worldObjects.push_back(new WorldObject(initials, TYPE_ROAD));
					roadDirection[i] = 1 - 2 * (rand() % 2);
				}
			}
		};

		void drawGrids() {
			for (int curr = 0; curr <= wh; curr += squareSize) {
				glColor3f(0.2, 0.0, 0.0);
				glBegin(GL_LINES);
				glVertex2f(0, curr);
				glVertex2f(ww, curr);
				glEnd();
			}

			for (int curr = 0; curr <= ww; curr += squareSize) {
				glColor3f(0.2, 0.0, 0.0);
				glBegin(GL_LINES);
				glVertex2f(curr, 0);
				glVertex2f(curr, wh);
				glEnd();
			}
		}

		void drawWorldObjects() {
			GLint y = 0;
			for (vector<WorldObject*>::iterator obj = this->worldObjects.begin(); obj != this->worldObjects.end(); ++obj) {
				(*obj)->draw(y, squareSize);
				y += this->squareSize;
			}
		}

		void spawnTestPawn() {
			vector<Vertex>initials = { Vertex(0, 0.25), Vertex(0, 0.75), Vertex(1,0.75), Vertex(1,0.25) };
			pawns.push_back(new Pawn(initials, squareSize, 250, 3 * squareSize, 3, 0));
		}

		void spawnPawn() {
			int randomRow = rand() % (wh / this->squareSize);
			// Selected row is a road
			if (worldLayout[randomRow] == TYPE_ROAD) {
				// Road is free to spawn a new car
				if (roadStatus[randomRow]) {
					vector<Vertex> initials;
					if (roadDirection[randomRow] == DIRECTION_RIGHT) {
						initials = { Vertex(0, 0.25), Vertex(0, 0.75), Vertex(0.05, 0.80), Vertex(0.6, 0.80), Vertex(1, 0.70), Vertex(1, 0.30), Vertex(0.6, 0.20), Vertex(0.05, 0.20) };
					} 
					else if (roadDirection[randomRow] == DIRECTION_LEFT) {
						initials = { Vertex(1, 0.25), Vertex(1, 0.75), Vertex(0.95, 0.80), Vertex(0.4, 0.80), Vertex(0, 0.70), Vertex(0, 0.30), Vertex(0.4, 0.20), Vertex(0.95, 0.20) };
					}
					roadStatus[randomRow] = 0; // Road is busy now, set road as busy
					spawnTimeStamps[randomRow] = this->frameTimer;
					// Traffic flow is to right
					if (roadDirection[randomRow] == DIRECTION_RIGHT) {
						pawns.push_back(new Pawn(initials, squareSize, -squareSize, randomRow * squareSize, randomRow, roadDirection[randomRow]));
					}
					// Traffic flow is to left
					else if (roadDirection[randomRow] == DIRECTION_LEFT) {
						pawns.push_back(new Pawn(initials, squareSize, ww + squareSize, randomRow * squareSize, randomRow, roadDirection[randomRow]));
					}
				}
			}
		}

		void updateWorld() {
			this->frameTimer++;
			if (frameTimer % 5 == 0) {
				this->spawnPawn();
			}
			for (int i = 0; i < wh / this->squareSize; i++) {
				if (this->frameTimer - this->spawnTimeStamps[i] > 80) {
					this->roadStatus[i] = 1;
				}
			}

			

			for (vector<Pawn*>::iterator pawn = this->pawns.begin(); pawn != this->pawns.end(); ++pawn) {
				(*pawn)->move();
			}
			this->checkCollisions();
		}

		void checkCollisions() {
			// Collision of Spawn Point - Pawn
			for (int i = 0; i < this->pawns.size(); i++) {
				if (pawns.at(i)->direction == DIRECTION_LEFT) {
					if ((pawns.at(i)->getVertex(0).x * pawns.at(i)->scale) + pawns.at(i)->offsetX <= leftSpawnPoint) {
						Pawn* ref = pawns.at(i); // TODO Check if this really works
						roadStatus[ref->assignedTo] = 1;
						pawns.erase(pawns.begin() + i);
						delete ref;
					}
				}
				else {
					if ((pawns.at(i)->getVertex(0).x * pawns.at(i)->scale) + pawns.at(i)->offsetX >= rightSpawnPoint) {
						Pawn* ref = pawns.at(i);
						roadStatus[ref->assignedTo] = 1;
						pawns.erase(pawns.begin() + i);
						delete ref;
					}
				}
			}

			// Collision of Pawn - Pawn
			for (int i = 0; i < this->pawns.size();i++) {
				for (int j = i+1; j < this->pawns.size(); j++) {
					// If pawns are on same road-line
					if (this->pawns.at(i)->assignedTo == this->pawns.at(j)->assignedTo) {
						GLint result = checkIntersection(this->pawns.at(i)->getCollisionBound(), this->pawns.at(j)->getCollisionBound());
						if (result) {
							// Set higher velocity to lower one, so pawns won't crash
							// [j] hits [i]
							if (abs(this->pawns.at(i)->velocity) < abs(this->pawns.at(j)->velocity)) {
								this->pawns.at(j)->pullBack();
								this->pawns.at(j)->velocity = this->pawns.at(i)->velocity;
							}
							// [i] hits [j]
							else {
								this->pawns.at(i)->pullBack();
								this->pawns.at(i)->velocity = this->pawns.at(j)->velocity;
							}
						}
					}
				}
			}
		}

		void drawWorld() {
			this->drawWorldObjects();
			//this->drawGrids();
			this->agent->draw();
			for (vector<Pawn*>::iterator pawn = this->pawns.begin(); pawn != this->pawns.end(); ++pawn) {
				(*pawn)->draw();
			}
		}
};

World *world;

void myReshape(GLsizei w, GLsizei h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	ww = w;
	wh = h;
}

void init(void)
{
	glViewport(0, 0, ww, wh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)ww, 0.0, (GLdouble)wh);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	world = new World();
	world->initWorld();
	//world->spawnTestPawn();
	glFlush();
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 0.0, 0.0);
	world->drawWorld();
	glutSwapBuffers();
}

void processSpecialKeys(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		world->agent->step(DIRECTION_UP);
		break;
	case GLUT_KEY_DOWN:
		world->agent->step(DIRECTION_DOWN);
		break;
	case GLUT_KEY_RIGHT:
		world->agent->step(DIRECTION_RIGHT);
		break;
	case GLUT_KEY_LEFT:
		world->agent->step(DIRECTION_LEFT);
		break;
	}
}

void timer(int)
{
	world->updateWorld();
	glutPostRedisplay();
	glutTimerFunc(1000.0 / 60.0, timer, 0);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(ww, wh);
	glutCreateWindow("Frogger");
	init();
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutSpecialFunc(processSpecialKeys);
	glutTimerFunc(1000.0 / 60.0, timer, 0);
	glutMainLoop();
}