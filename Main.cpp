#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <GL/glut.h>
#include <vector>
#include <functional>
#include <memory>
#include <mmsystem.h>
#include <string>

#include <fmod.hpp>
#include <fmod_errors.h>

// Sound effect pointers
FMOD::System *FMODSystem;
FMOD::Channel *ambienceChannel = 0;
FMOD::Channel *footStepChannel = 0;
FMOD::Channel *gameOverChannel = 0;
FMOD::Channel *effectChannel = 0;
FMOD_RESULT result;
FMOD::Sound *ambience;
FMOD::Sound *footstepGrass;
FMOD::Sound *footstepRoad;
FMOD::Sound *coinPick;
FMOD::Sound *horn1;
FMOD::Sound *horn2;
FMOD::Sound *horn3;
FMOD::Sound *horn4;
FMOD::Sound *gameOver;
FMOD::Sound *carHit;

// Actor directions
GLint const DIRECTION_LEFT = -1;
GLint const DIRECTION_RIGHT = 1;
GLint const DIRECTION_UP = 2;
GLint const DIRECTION_DOWN = -2;

// World object types
GLint const WO_PAVEMENT = 1; // World Object - Pavement type
GLint const WO_ROAD = 2; // World Object - Road type
GLint const WO_FLORA_TREE_1 = 3; // World Object - Flora - Tree type 1
GLint const WO_FLORA_TREE_2 = 4; // World Object - Flora - Tree type 2

// World states
GLint const WORLD_STATE_RUN = 1; // Game is in running state
GLint const WORLD_STATE_PAUSE = 0; // Game is paused
GLint const WORLD_STATE_OVER = 3; // Game is over
GLint const WORLD_STATE_ONEFRAME = 2; // Progress game just 1 frame

// Pawn types
GLint const PAWN_CAR = 0; // Pawn type is car
GLint const PAWN_TRUCK = 1; // Pawn type is truck

//Agent states
GLint const AGENT_MOVE = 1; // Agent movement animation is in progress
GLint const AGENT_IDLE = 0; // Agent is in idle state

// Collision types
GLint const COL_LOOSE = 0; // Loose collision area(bigger than initial polygon)
GLint const COL_STRICT = 1; // Strict collision area(same with initial polygon)
GLint const COL_LONG = 2; // Loose collision area(longer than COL_LOOSþ

using namespace std;

GLsizei wh = 600, ww = 525; // initial window size, resizing is disabled

// Main class for vertex structure used in game world
class Vertex {
	public:
		GLfloat x;
		GLfloat y;
		Vertex(GLfloat x, GLfloat y) {
			this->x = x;
			this->y = y;
		};
};

// Draws a circle with r radius
void drawCircle(GLfloat x, GLfloat y, GLfloat r) {
	GLint i;
	GLint triangleAmount = 1000;
	GLfloat twicePi = 2.0f * 3.14;

	glEnable(GL_LINE_SMOOTH);
	glLineWidth(5.0);

	glBegin(GL_LINES);
	for (i = 0; i <= triangleAmount; i++)
	{
		glVertex2f(x, y);
		glVertex2f(x + (r * cos(i * twicePi / triangleAmount)), y + (r * sin(i * twicePi / triangleAmount)));
	}
	glEnd();
}

// Draws a bitmap text on given coordinates of screen with given font as parameters
void drawText(char *txt, GLfloat x, GLfloat y, void* font) {
	char text[32];
	sprintf(text, "%s", txt);
	glRasterPos2f(x, y);
	for (int i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(font, text[i]);
}

// Main method for smooth animations seen in the game, interpolates v0 to v1 on delta value
float lerp(GLfloat v0, GLfloat v1, GLfloat delta) {
	return (1 - delta) * v0 + delta * v1;
}

// Classic orientation function
GLint getOrientation(Vertex p1, Vertex p2, Vertex p3) {
	GLint result = (p2.y - p1.y) * (p3.x - p2.x) - (p2.x - p1.x) * (p3.y - p2.y);
	if (result == 0) {
		return 0; // Colinear
	}
	return (result > 0) ? 1 : 2;
}

// Checks if point is in polygon
GLint contains(Vertex point, vector<Vertex> poly) {
	Vertex startpoint = poly.at(0);
	Vertex endpoint = poly.at((1) % poly.size());
	GLint currOri = getOrientation(startpoint, endpoint, point);
	for (int i = 1; i < poly.size(); i++) {
		Vertex startpoint = poly.at(i);
		Vertex endpoint = poly.at((i+1) % poly.size());
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
GLint checkIntersection(vector<Vertex> p1, vector<Vertex> p2) {
	for (GLint i = 0; i < p1.size(); i++) {
		for (GLint j = 0; j < p2.size(); j++) {
			// From first polygon
			GLint ori1 = getOrientation(p1[i], p1[(i + 1) % p1.size()], p2[j]);
			GLint ori2 = getOrientation(p1[i], p1[(i + 1) % p1.size()], p2[(j+1) % p2.size()]);

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

// Parent class for dynamic objects in game world
class Actor {
	public:
		vector<Vertex> vertices; // Unit polygon (It's like a miniature polygon, it's scaled and moved into right direction with offset values)
		GLfloat scale;
		GLfloat offsetX;
		GLfloat offsetY;
		Actor() {};
		Actor(GLfloat scale, GLfloat offsetX, GLfloat offsetY) {
			this->scale = scale;
			this->offsetX = offsetX;
			this->offsetY = offsetY;
		};
		vector<Vertex> getVertices() {
			return this->vertices;
		};
		Vertex getVertex(GLint i) {
			return (this->vertices)[i];
		};

		void draw() {
			glBegin(GL_POLYGON);
			for (vector<Vertex>::iterator vertex = this->vertices.begin(); vertex != this->vertices.end(); ++vertex) {
				glVertex2f((vertex->x * this->scale) + offsetX, (vertex->y * this->scale) + offsetY);
			}
			glEnd();
		}
};

// Objects controlled by player
class Agent: public Actor {
	public:
		GLfloat stepSize; // Should be equal to square size of world layout
		GLint direction;
		GLfloat targetX;
		GLfloat targetY;
		GLfloat initialScale;
		GLfloat targetScale;
		GLint currentRow = 0;
		GLint stateX = AGENT_IDLE;
		GLint stateY = AGENT_IDLE;
		vector<Vertex> blockedSquares;
		GLint triedToGoBack = 0;
		GLint deathAnimFlag;

		vector<Vertex> const rotate_up = { Vertex(0.2,0.1), Vertex(0.5,0.9), Vertex(0.8,0.1) };
		vector<Vertex> const rotate_down = { Vertex(0.2,0.9), Vertex(0.5,0.1), Vertex(0.8,0.9) };
		vector<Vertex> const rotate_left = { Vertex(0.9,0.2), Vertex(0.1,0.5), Vertex(0.9,0.8) };
		vector<Vertex> const rotate_right = { Vertex(0.1,0.2), Vertex(0.9,0.5), Vertex(0.1,0.8) };

		Agent(GLfloat stepSize, GLfloat scale, GLfloat offsetX, GLfloat offsetY, vector<Vertex> blockedSquares) : Actor(scale, offsetX, offsetY) {
			this->targetX = offsetX;
			this->targetY = offsetY;
			this->initialScale = scale;
			this->stepSize = stepSize;
			this->direction = DIRECTION_UP;
			this->vertices = rotate_up;
			this->blockedSquares = blockedSquares;
			this->targetScale = scale * 1.75;
			this->deathAnimFlag = 0;
		};

		GLint step(GLint direction) {
			if (direction == DIRECTION_LEFT) {
				// Decline input if if target location is blocked
				for (vector<Vertex>::iterator move = this->blockedSquares.begin(); move != this->blockedSquares.end(); ++move) {
					if (move->x == offsetX - stepSize && move->y == this->offsetY) {
						return 0;
					}
				}

				if (this->stateX != AGENT_MOVE && this->offsetX > 0) {
					this->vertices = rotate_left;
					this->targetX = this->offsetX - stepSize;
					this->stateX = AGENT_MOVE;
					return 1;
				}
			}
			else if (direction == DIRECTION_RIGHT) {
				// Decline input if if target location is blocked
				for (vector<Vertex>::iterator move = this->blockedSquares.begin(); move != this->blockedSquares.end(); ++move) {
					if (move->x == offsetX + stepSize && move->y == this->offsetY) {
						return 0;
					}
				}

				if (this->stateX != AGENT_MOVE && this->offsetX < ww - stepSize) {
					this->vertices = rotate_right;
					this->targetX = this->offsetX + stepSize;
					this->stateX = AGENT_MOVE;
					return 1;
				}
			}
			else if (direction == DIRECTION_UP) {
				// Decline input if if target location is blocked
				for (vector<Vertex>::iterator move = this->blockedSquares.begin(); move != this->blockedSquares.end(); ++move) {
					if (move->x == offsetX && move->y == this->offsetY + stepSize) {
						return 0;
					}
				}

				if (this->stateY != AGENT_MOVE  && this->offsetY < wh - stepSize) {
					this->vertices = rotate_up;
					this->targetY = this->offsetY + stepSize;
					// Time to change direction of agent
					if (this->targetY == wh - stepSize) {
						this->vertices = rotate_down;
						this->direction = DIRECTION_DOWN;
					}
					this->stateY = AGENT_MOVE;
					this->currentRow++;
					return 1;
				}
			}
			else if (direction == DIRECTION_DOWN) {
				// Decline input if if target location is blocked
				for (vector<Vertex>::iterator move = this->blockedSquares.begin(); move != this->blockedSquares.end(); ++move) {
					if (move->x == offsetX && move->y == this->offsetY - stepSize) {
						return 0;
					}
				}

				if (this->stateY != AGENT_MOVE && this->direction == DIRECTION_DOWN && this->offsetY > 0) {
					this->vertices = rotate_down;
					this->targetY = this->offsetY - stepSize;
					// Time to change direction of agent
					if (this->targetY == 0) {
						this->vertices = rotate_up;
						this->direction = DIRECTION_UP;
					}
					this->stateY = AGENT_MOVE;
					this->currentRow--;
					return 1;
				}
			}
		}

		vector<Vertex> getCollisionBound(GLint collisionType) {
			vector<Vertex> tempVertices;
			for (vector<Vertex>::iterator vertex = this->vertices.begin(); vertex != this->vertices.end(); ++vertex) {
				if (collisionType == COL_LOOSE) {
					tempVertices.push_back(Vertex((vertex->x * (this->scale + 12.5)) + this->offsetX - 6.25, (vertex->y * this->scale) + this->offsetY));
				}
				else if (collisionType == COL_STRICT) {
					tempVertices.push_back(Vertex((vertex->x * this->scale) + this->offsetX, (vertex->y * this->scale) + this->offsetY));
				}
			}
			return tempVertices;
		}

		void draw() {
			glColor3f(0.8, 0.0, 0);
			Actor::draw();
		}
};

class Coin :public Actor {
	public:
		GLint timeout;
		GLfloat initialX;
		GLfloat targetX;
		GLfloat targetY;
		GLfloat targetRadius;
		GLint points;
		GLint isTaken;
		Coin(GLfloat radius, GLfloat offsetX, GLfloat offsetY, GLint timeout, GLint points) : Actor(radius, offsetX, offsetY) {
			this->initialX = offsetX;
			this->targetX = offsetX + 10;
			this->timeout = timeout;
			this->points = points;
			this->isTaken = 0;
			this->targetRadius = radius + 10;
		}

		// Idle animation of coin
		void idle() {
			if (abs(this->offsetX - this->targetX) > 1) {
				this->offsetX = lerp(this->offsetX, this->targetX, 0.01);
			}
			else if(this->targetX == this->initialX + 10){
				this->targetX = this->initialX;
			}
			else {
				this->targetX = this->initialX + 10;
			}
		}

		// Pick up animation of coin
		void taken() {
			this->offsetX = lerp(this->offsetX, this->targetX, 0.05);
			this->offsetY = lerp(this->offsetY, this->targetY, 0.05);
			this->scale = lerp(this->scale, this->targetRadius, 0.05);
		}

		void draw() {
			glColor3f(1.0, 1.0, 0.0);
			drawCircle(this->offsetX, this->offsetY, this->scale);
		}
};

// Objects non-controlled by player
class Pawn :public Actor {
	public:
		GLint assignedTo; // Which row is assigned to this pawn
		GLfloat velocity;
		GLint direction;
		GLint color;
		GLint type;
		GLint honked; // Is this pawn tried honking it's horn?

		Pawn(GLint difficulty, GLfloat scale, GLfloat offsetX, GLfloat offsetY, GLint assignedTo, GLint direction) : Actor(scale, offsetX, offsetY) {
			this->assignedTo = assignedTo;
			this->direction = direction;
			this->velocity = ((rand() % 6) + difficulty) * direction;
			this->color = rand() % 6;
			this->type = rand() % 2;

			this->honked = 0;

			if (type == PAWN_CAR) {
				if (direction == DIRECTION_RIGHT) {
					this->vertices = { Vertex(0, 0.25), Vertex(0, 0.75), Vertex(0.05, 0.80), Vertex(0.6, 0.80), Vertex(1, 0.70), Vertex(1, 0.30), Vertex(0.6, 0.20), Vertex(0.05, 0.20) };
				}
				else if (direction == DIRECTION_LEFT) {
					this->vertices = { Vertex(1, 0.25), Vertex(1, 0.75), Vertex(0.95, 0.80), Vertex(0.4, 0.80), Vertex(0, 0.70), Vertex(0, 0.30), Vertex(0.4, 0.20), Vertex(0.95, 0.20) };
				}
			}
			else if (type == PAWN_TRUCK) {
				if (direction == DIRECTION_RIGHT) {
					this->vertices = { Vertex(0, 0.25), Vertex(0, 0.75), Vertex(0.05, 0.80), Vertex(1.6, 0.80), Vertex(2, 0.70), Vertex(2, 0.30), Vertex(1.6, 0.20), Vertex(0.05, 0.20) };
				}
				else if (direction == DIRECTION_LEFT) {
					this->vertices = { Vertex(2, 0.25), Vertex(2, 0.75), Vertex(1.95, 0.80), Vertex(0.4, 0.80), Vertex(0, 0.70), Vertex(0, 0.30), Vertex(0.4, 0.20), Vertex(1.95, 0.20) };
				}
			}	
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

		// Pull the pawn a little back in the reverse direction of it's velocity, required for pawn-pawn collision handling
		void pullBack() {
			this->offsetX -= this->velocity;
		}

		vector<Vertex> getCollisionBound(GLint collisionType) {
			vector<Vertex> tempVertices;
			for (vector<Vertex>::iterator vertex = this->vertices.begin(); vertex != this->vertices.end(); ++vertex) {
				if (collisionType == COL_LOOSE) {
					if (this->type == PAWN_CAR)
						tempVertices.push_back(Vertex((vertex->x * (this->scale + 25)) + this->offsetX - 12.5, (vertex->y * this->scale) + this->offsetY));
					else if (this->type == PAWN_TRUCK)
						tempVertices.push_back(Vertex((vertex->x * (this->scale + 12.5)) + this->offsetX - 6.25, (vertex->y * this->scale) + this->offsetY));
				}
				else if (collisionType == COL_STRICT) {
					tempVertices.push_back(Vertex((vertex->x * this->scale) + this->offsetX, (vertex->y * this->scale) + this->offsetY));
				}
				else if (collisionType == COL_LONG) {
					if (this->type == PAWN_CAR) {
						if (this->direction == DIRECTION_LEFT) {
							tempVertices.push_back(Vertex((vertex->x * (this->scale + 50)) + this->offsetX - 100, (vertex->y * this->scale) + this->offsetY));
						}
						else {
							tempVertices.push_back(Vertex((vertex->x * (this->scale + 50)) + this->offsetX + 100, (vertex->y * this->scale) + this->offsetY));
						}
					}
					else if (this->type == PAWN_TRUCK) {
						if (this->direction == DIRECTION_LEFT) {
							tempVertices.push_back(Vertex((vertex->x * (this->scale + 25)) + this->offsetX - 50, (vertex->y * this->scale) + this->offsetY));
						}
						else {
							tempVertices.push_back(Vertex((vertex->x * (this->scale + 25)) + this->offsetX + 50, (vertex->y * this->scale) + this->offsetY));
						}
					}
				}
			}
			return tempVertices;
		}
};

// Static objects in game world
class WorldObject {
	public:
		GLint objectType;
		vector<Vertex> vertices;
		GLint offsetX;
		GLint offsetY;
	
		WorldObject(vector<Vertex> vertices, GLint objectType) {
			this->objectType = objectType;
			if (objectType == WO_PAVEMENT) {
				this->vertices = vertices;
			}
			else if (objectType == WO_ROAD) {
				this->vertices = vertices;
			}
			else if (objectType == WO_FLORA_TREE_1 || objectType == WO_FLORA_TREE_2) {
				this->offsetX = vertices.at(0).x;
				this->offsetY = vertices.at(0).y;
			}
		}

		void draw() {
			if (objectType == WO_PAVEMENT) {
				glColor3f(0.0f, 1.0f, 0.0f);
			}
			else if(objectType == WO_ROAD) {
				glColor3f(0.5f, 0.5f, 0.5f);
			}
			else if (objectType == WO_FLORA_TREE_1) {
				GLint r = 20;
				glColor3f(0, 0.5f, 0);
				drawCircle(this->offsetX, this->offsetY, r);

				glBegin(GL_TRIANGLES);
				glVertex2f(this->offsetX + r / 2, this->offsetY + 0.8*r);
				glVertex2f(this->offsetX + r / 2, this->offsetY - 0.8*r);
				glVertex2f(this->offsetX + 2*r, this->offsetY);

				glVertex2f(this->offsetX - r / 2, this->offsetY - 0.8*r);
				glVertex2f(this->offsetX - r / 2, this->offsetY + 0.8*r);
				glVertex2f(this->offsetX - 2 * r, this->offsetY);

				glVertex2f(this->offsetX + 0.8*r, this->offsetY + r / 2);
				glVertex2f(this->offsetX - 0.8*r, this->offsetY + r / 2 );
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
			else if (objectType == WO_FLORA_TREE_2) {
				GLint r = 20;
				glColor3f(0, 0.5f, 0);
				drawCircle(this->offsetX, this->offsetY, r);

				drawCircle(this->offsetX + r, this->offsetY, r / 2);
				drawCircle(this->offsetX + r, this->offsetY + r / 1.2, r / 2);
				drawCircle(this->offsetX - r, this->offsetY, r / 2);
				drawCircle(this->offsetX - r, this->offsetY + r / 1.2, r / 2);
				drawCircle(this->offsetX, this->offsetY + r, r / 2);
				drawCircle(this->offsetX + r, this->offsetY - r / 1.2, r / 2);
				drawCircle(this->offsetX - r, this->offsetY - r / 1.2, r / 2);
				drawCircle(this->offsetX, this->offsetY - r, r / 2);

				glColor3f(0.2f, 0.0f, 0);
				glBegin(GL_POINTS);
				glVertex2f(this->offsetX, this->offsetY);
				glEnd();
				return;
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
		vector<WorldObject*> worldObjects_roadsPavs;
		vector<WorldObject*> worldObjects_trees;
		vector<Coin*> coins;
		vector<Pawn*> pawns;
		GLint* worldLayout;
		GLint wSquareCount;
		GLint hSquareCount;

		GLint* roadStatus;
		GLint* roadDirection;
		GLint* roadSpawnTimeStamps;

		GLint coinTextTimeout = 0;
		GLfloat coinTextX = 0;
		GLfloat coinTextY = 0;
		GLint score;
		
		GLint squareSize;
		GLint const leftSpawnPoint = -2 * squareSize;
		GLint const rightSpawnPoint = ww + 2 * squareSize;
		GLint frameTimer;
		GLint state;

		GLint deathAnimDone = 0;
		GLfloat FOVX = ww;
		GLfloat LOOKX = 0;
		GLfloat LOOKY = 0;
		GLfloat FOVY = wh;
		GLfloat difficulty;

		World(GLint squareSize, GLint startDifficulty) {
			this->squareSize = squareSize;
			this->difficulty = startDifficulty;
			this->frameTimer = 0;
			this->score = 0;
			initWorld(); // Initialize the world
		}

		// Initialize the game world
		void initWorld() {
			srand(time(NULL));

			FMODSystem->playSound(ambience, 0, false, &ambienceChannel);
			ambienceChannel->setVolume(0.5f);

			wSquareCount = ww / this->squareSize; // Total square count of window width
			hSquareCount = wh / this->squareSize; // Total square count of window heigth

			worldLayout = new GLint[hSquareCount];
			roadStatus = new GLint[hSquareCount];
			roadDirection = new GLint[hSquareCount];
			roadSpawnTimeStamps = new GLint[hSquareCount];

			vector<Vertex> initials; // Temp vector for defining object vertices
			vector<Vertex> blockedSquares; // User cant pass through these squares

			// Decide world layout, repeat until requirements are are met.
			GLint sidewalks = 0;
			while (sidewalks < 6) {
				this->worldObjects_roadsPavs.clear();
				sidewalks = 0;
				GLint consecuviteRoad = 0;
				GLint randNum = 0;
				for (GLint i = 0; i < (hSquareCount); i++) {
					initials = { Vertex(0,i*squareSize), Vertex(0,(i + 1)*squareSize), Vertex(wh,(i + 1)*squareSize), Vertex(wh,i*squareSize) };
					// First & Last row is always pavement, continue building roads until random number is equal to consecutive road count
					if (i == 0 || i == (hSquareCount) - 1 || (consecuviteRoad == randNum)) {
						worldLayout[i] = WO_PAVEMENT;
						roadStatus[i] = -1;
						roadDirection[i] = 0;
						this->worldObjects_roadsPavs.push_back(new WorldObject(initials, WO_PAVEMENT));

						if (i != hSquareCount - 2) {
							GLint treeCount = rand() % 3 + 1;
							for (int j = 0; j < treeCount; j++) {
								GLint treeType = rand() % 2;
								initials.clear();

								GLint place = (rand() % (wSquareCount / 3)) * 3; // A tree can't be closer than 3 squares to other trees
								if (i == 0 && place == wSquareCount / 2 + 1) {
									place++;
								}

								blockedSquares.push_back(Vertex((place - 1) * squareSize, squareSize * i)); // User can't pass through this tree

								initials.push_back(Vertex(squareSize * place - squareSize / 2, squareSize * i + squareSize / 2));
								if (treeType == 0) {
									this->worldObjects_trees.push_back(new WorldObject(initials, WO_FLORA_TREE_1));
								}
								else {
									this->worldObjects_trees.push_back(new WorldObject(initials, WO_FLORA_TREE_2));
								}
							}
						}

						consecuviteRoad = 0;
						sidewalks++;
						randNum = (rand() % 2) + 3;
					}
					else {
						worldLayout[i] = WO_ROAD;
						roadStatus[i] = 1;
						this->worldObjects_roadsPavs.push_back(new WorldObject(initials, WO_ROAD));
						roadDirection[i] = 1 - 2 * (rand() % 2);
						consecuviteRoad++;
					}
				}
			}

			// Create player agent
			agent = new Agent(squareSize, squareSize, ((wSquareCount) / 2 * squareSize), 0, blockedSquares);
			this->state = WORLD_STATE_RUN;
		};

		// Draw road lines
		void drawRoadLines() {
			GLint row = 1;
			glColor3f(1.0f, 1.0f, 1.0f);
			glLineWidth(2);
			for (GLint curr = squareSize; curr <= wh; curr += squareSize) {
				if (worldLayout[row] == WO_ROAD && worldLayout[row-1] == WO_ROAD) {
					for (GLint wid = 0; wid <= ww; wid += 2 * squareSize) {
						glBegin(GL_LINES);
						glVertex2f(wid, curr);
						glVertex2f(wid + squareSize, curr);
						glEnd();
					}
				}
				row++;
			}
			glLineWidth(1); // Roll back line width change, it includes roads and pavements
		}

		// First(lower) layer of world objects
		void drawWorldObjects_Layer1() {
			for (vector<WorldObject*>::iterator obj = this->worldObjects_roadsPavs.begin(); obj != this->worldObjects_roadsPavs.end(); ++obj) {
				(*obj)->draw();		
			}
		}

		// Second(upper) layer of world objects, it includes trees
		void drawWorldObjects_Layer2() {
			for (vector<WorldObject*>::iterator obj = this->worldObjects_trees.begin(); obj != this->worldObjects_trees.end(); ++obj) {
				(*obj)->draw();
			}
		}

		// Create a random pawn and push it into data structure
		void spawnPawn() {
			GLint randomRow = rand() % (hSquareCount);
			// Selected row is a road
			if (worldLayout[randomRow] == WO_ROAD) {
				// Road is free to spawn a new car
				if (roadStatus[randomRow]) {
					// Traffic flow is to right
					if (roadDirection[randomRow] == DIRECTION_RIGHT) {
						pawns.push_back(new Pawn(difficulty, squareSize, -2 * squareSize, randomRow * squareSize, randomRow, roadDirection[randomRow]));
					}
					// Traffic flow is to left
					else if (roadDirection[randomRow] == DIRECTION_LEFT) {
						pawns.push_back(new Pawn(difficulty, squareSize, ww + 2 * squareSize, randomRow * squareSize, randomRow, roadDirection[randomRow]));
					}
					roadStatus[randomRow] = 0; // Road is busy now, set road as busy
					roadSpawnTimeStamps[randomRow] = this->frameTimer;
				}
			}
		}

		// Create a coin and push it into data structure
		void spawnCoin() {
			this->coins.push_back(new Coin(5, this->squareSize * (rand() % wSquareCount) + (squareSize / 3), this->squareSize * (rand() % (hSquareCount)) + (squareSize / 2), this->frameTimer + (rand() % 100) + 600, 5));
		}

		// Called on each frame, updates the world object
		void updateWorld() {
			this->frameTimer++;

			// Try spawning a pawn on each 5th frame.
			if (frameTimer % 5 == 0) {
				this->spawnPawn();
			}

			// Try spawning a coin on each 150th frame, remove timed out coins, max spawned coin amount is 6
			for (int i = 0; i < coins.size(); i++) {
				// Timeout coin
				if (coins.at(i)->timeout <= this->frameTimer) {
					coins.erase(coins.begin() + i);
				}
			} if (coins.size() < 6 && frameTimer % 150 == 0) {
				spawnCoin();
			}

			// Increase speed of pawns on each 2000th frame
			if (frameTimer % 2000 == 0 && this->difficulty < 4) {
				this->difficulty++;
			}

			// Set roads as available if some amount of time passed since a pawn is spawned on this road(80 frames).
			for (GLint i = 0; i < hSquareCount; i++) {
				if (this->frameTimer - this->roadSpawnTimeStamps[i] > 80) {
					this->roadStatus[i] = 1;
				}
			}

			// Move pawns
			for (vector<Pawn*>::iterator pawn = this->pawns.begin(); pawn != this->pawns.end(); ++pawn) {
				(*pawn)->move();
			}

			// Move agent with a smooth animation
			if (this->agent->stateY == AGENT_MOVE) {
				if (abs(this->agent->offsetY - this->agent->targetY) > 1) {
					this->agent->offsetY = lerp(this->agent->offsetY, this->agent->targetY, 0.4);
				}
				else {
					this->agent->offsetY = this->agent->targetY;
					this->agent->stateY = AGENT_IDLE;
				}
			}
			if (this->agent->stateX == AGENT_MOVE) {
				if (abs(this->agent->offsetX - this->agent->targetX) > 1) {
					this->agent->offsetX = lerp(this->agent->offsetX, this->agent->targetX, 0.4);
				}
				else {
					this->agent->offsetX = this->agent->targetX;
					this->agent->stateX = AGENT_IDLE;
				}
			}

			// Animate coins
			vector<GLint> toRemove;
			for (int i = 0; i < coins.size(); i++) {
				if (coins.at(i)->isTaken) {
					coins.at(i)->taken();
					if (abs(coins.at(i)->offsetX - coins.at(i)->targetX) < 5) {
						toRemove.push_back(i);
					}
				}
				else {
					coins.at(i)->idle();
				}
			}

			// Delete picked coins from data structure
			for (vector<GLint>::iterator ind = toRemove.begin(); ind != toRemove.end(); ++ind) {
				coins.erase(coins.begin() + (*ind));
			}
			toRemove.clear();

			// Check if there exists a collision
			this->checkCollisions();
		}

		void checkCollisions() {
			// Collision of Spawn Point - Pawn, delete pawn pointer from the vector
			for (GLint i = 0; i < this->pawns.size(); i++) {
				if (pawns.at(i)->direction == DIRECTION_LEFT) {
					if ((pawns.at(i)->getVertex(0).x * pawns.at(i)->scale) + pawns.at(i)->offsetX <= leftSpawnPoint) {
						roadStatus[pawns.at(i)->assignedTo] = 1;
						pawns.erase(pawns.begin() + i);
					}
				}
				else if(pawns.at(i)->direction == DIRECTION_RIGHT){
					if ((pawns.at(i)->getVertex(0).x * pawns.at(i)->scale) + pawns.at(i)->offsetX >= rightSpawnPoint) {
						roadStatus[pawns.at(i)->assignedTo] = 1;
						pawns.erase(pawns.begin() + i);
					}
				}
			}

			// Collision of Pawn - Pawn, equalize these pawns' velocities
			for (GLint i = 0; i < this->pawns.size(); i++) {
				for (GLint j = i + 1; j < this->pawns.size(); j++) {
					// If pawns are on same road-line
					if (this->pawns.at(i)->assignedTo == this->pawns.at(j)->assignedTo) {
						GLint result = checkIntersection(this->pawns.at(i)->getCollisionBound(COL_LOOSE), this->pawns.at(j)->getCollisionBound(COL_LOOSE));
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

			// Collision of Pawn - Agent, honk horn or kill agent
			if (this->state != WORLD_STATE_OVER) {
				for (GLint i = 0; i < this->pawns.size(); i++) {
					if (this->pawns.at(i)->assignedTo == this->agent->currentRow) {
						GLint intersectionResult = checkIntersection(this->pawns.at(i)->getCollisionBound(COL_STRICT), this->agent->getCollisionBound(COL_STRICT));
						
						// Pawns can honk their horn if agent is close to front side of pawn, do an extra collision check for this:
						if (this->pawns.at(i)->honked == 0) {
							GLint hornResult = checkIntersection(this->pawns.at(i)->getCollisionBound(COL_LONG), this->agent->getCollisionBound(COL_STRICT));
							if (hornResult) {
								this->pawns.at(i)->honked = 1; // Each pawn can try their chance for honking horn one time.
								// %50 chance:
								if (rand() % 2 == 0) {
									GLint hornType = rand() % 4; // Random horn type
									switch (hornType) {
									case 0:
										FMODSystem->playSound(horn1, 0, false, &effectChannel);
										break;
									case 1:
										FMODSystem->playSound(horn2, 0, false, &effectChannel);
										break;
									case 2:
										FMODSystem->playSound(horn3, 0, false, &effectChannel);
										break;
									case 3:
										FMODSystem->playSound(horn4, 0, false, &effectChannel);
										break;
									default:
										break;
									}
								}
								
							}
						}
						// Pawn hits to agent:
						if (intersectionResult) {
							ambienceChannel->setVolume(0.15f);
							FMODSystem->playSound(gameOver, 0, false, &gameOverChannel);
							FMODSystem->playSound(carHit, 0, false, &effectChannel);
							this->agent->targetX = this->agent->offsetX + 20 * this->pawns.at(i)->velocity;
							this->pawns.at(i)->velocity = 0;
							// Rotate agent to right direction:
							if (this->pawns.at(i)->direction == DIRECTION_LEFT) {
								this->agent->vertices = this->agent->rotate_left;
							}
							else {
								this->agent->vertices = this->agent->rotate_right;
							}

							this->state = WORLD_STATE_OVER;
						}
					}
				}
			}


			// Collision of Agent - Coin, pick up the coin
			for (GLint i = 0; i < this->coins.size(); i++) {
				GLint result = contains(Vertex(this->coins.at(i)->offsetX, this->coins.at(i)->offsetY), this->agent->getCollisionBound(COL_LOOSE));
				if (result) {
					FMODSystem->playSound(coinPick, 0, false, &effectChannel);
					this->coinTextX = coins.at(i)->offsetX;
					this->coinTextY = coins.at(i)->offsetY;
					this->coinTextTimeout = frameTimer + 100;
					this->score += coins.at(i)->points;
					coins.at(i)->isTaken = 1;
					coins.at(i)->targetX = 0;
					coins.at(i)->targetY = wh;
				}
			}
		}

		// Show score on screen
		void showScore() {
			char scoreText[21];
			sprintf(scoreText, "Score: %d", this->score);
			glColor3f(1.0f, 1.0f, 0.0f);
			drawText(scoreText, 0, wh - 0.75*this->squareSize, GLUT_BITMAP_HELVETICA_18);
		}

		// Draw each object in game world into screen
		void drawWorld() {
			this->drawWorldObjects_Layer1();
			this->drawRoadLines();
			this->agent->draw();
			for (vector<Pawn*>::iterator pawn = this->pawns.begin(); pawn != this->pawns.end(); ++pawn) {
				(*pawn)->draw();
			}
			for (vector<Coin*>::iterator coin = this->coins.begin(); coin != this->coins.end(); ++coin) {
				(*coin)->draw();
			}
			this->drawWorldObjects_Layer2();

			// Draw coin earned text (+5) if it exists
			if (coinTextTimeout > frameTimer) {
				glColor3f(1.0f, 1.0f, 0.0f);
				drawText("+5", coinTextX, coinTextY, GLUT_BITMAP_HELVETICA_18);
			}

			this->showScore();
		}
};

World *world; // Main world object

void resize(GLsizei w, GLsizei h) {
	// Do not let user resize the window
	glutReshapeWindow(ww, wh);
}

void init(void) {
	glViewport(0, 0, ww, wh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)ww, 0.0, (GLdouble)wh);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	world = new World(25, 1); // Create game world
	glFlush();
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	world->drawWorld();

	// If death animation of agent is completed, show paraphrase and score on screen:
	if (world->deathAnimDone) {
		glColor3f(1.0f, 0.0f, 0.0f);
		if (world->agent->triedToGoBack) {
			drawText("Don't be a coward", world->agent->offsetX - world->squareSize * 1.25, world->agent->offsetY + 2 * world->squareSize, GLUT_BITMAP_TIMES_ROMAN_24);
		}
		else {
			drawText("WASTED", world->agent->offsetX - world->squareSize / 2, world->agent->offsetY + 2 * world->squareSize, GLUT_BITMAP_TIMES_ROMAN_24);
		}
		char scoreText[21];
		sprintf(scoreText, "Your score: %d", world->score);
		glColor3f(1.0f, 0.0f, 0.0f);
		drawText(scoreText, world->agent->offsetX - world->squareSize / 1.70, world->agent->offsetY + 1.5 * world->squareSize, GLUT_BITMAP_HELVETICA_18);
	}

	glFlush();
	glutSwapBuffers();
}

void processSpecialKeys(GLint key, GLint x, GLint y) {
	// If game is over, ignore player input
	if (world->state == WORLD_STATE_OVER) {
		return;
	}

	switch (key) {
	case GLUT_KEY_UP:
		// Agent tried to go wrong direction:
		if (world->agent->direction == DIRECTION_DOWN && world->agent->stateY != AGENT_MOVE && world->agent->offsetY != wh - world->squareSize) {
			ambienceChannel->setVolume(0.15f);
			FMODSystem->playSound(gameOver, 0, false, &gameOverChannel);
			world->agent->triedToGoBack = 1;
			world->state = WORLD_STATE_OVER;
		}
		// Move agent:
		else if (world->agent->step(DIRECTION_UP) == 1 && world->agent->offsetY != wh - world->squareSize) {
			if (world->worldLayout[world->agent->currentRow] == WO_PAVEMENT) {
				FMODSystem->playSound(footstepGrass, 0, false, &footStepChannel);
			}
			else {
				FMODSystem->playSound(footstepRoad, 0, false, &footStepChannel);
			}
			world->score++; // Increase score if direction of agent and movement is same
		}
		break;
	case GLUT_KEY_DOWN:
		// Agent tried to go wrong direction:
		if (world->agent->direction == DIRECTION_UP && world->agent->stateY != AGENT_MOVE && world->agent->offsetY != 0) {
			ambienceChannel->setVolume(0.15f);
			FMODSystem->playSound(gameOver, 0, false, &gameOverChannel);
			world->agent->triedToGoBack = 1;
			world->state = WORLD_STATE_OVER;
		}
		// Move agent:
		else if (world->agent->step(DIRECTION_DOWN) == 1 && world->agent->offsetY != 0) {
			if (world->worldLayout[world->agent->currentRow] == WO_PAVEMENT) {
				FMODSystem->playSound(footstepGrass, 0, false, &footStepChannel);
			}
			else {
				FMODSystem->playSound(footstepRoad, 0, false, &footStepChannel);
			}
			world->score++; // Increase score if direction of agent and movement is same
		}
		break;
	case GLUT_KEY_RIGHT:
		if (world->worldLayout[world->agent->currentRow] == WO_PAVEMENT) {
			FMODSystem->playSound(footstepGrass, 0, false, &footStepChannel);
		}
		else {
			FMODSystem->playSound(footstepRoad, 0, false, &footStepChannel);
		}
		world->agent->step(DIRECTION_RIGHT);
		break;
	case GLUT_KEY_LEFT:
		if (world->worldLayout[world->agent->currentRow] == WO_PAVEMENT) {
			FMODSystem->playSound(footstepGrass, 0, false, &footStepChannel);
		}
		else {
			FMODSystem->playSound(footstepRoad, 0, false, &footStepChannel);
		}
		world->agent->step(DIRECTION_LEFT);
		break;
	}
}

void processNormalKeys(unsigned char key, int x, int y) {
	if (key == 113 || key == 81) // ASCII Q = 81, ASCII q = 113
		exit(0);
}

void mouseInput(GLint button, GLint state, GLint x, GLint y) {
	switch (button) {
	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_DOWN && world->state == WORLD_STATE_RUN)
			world->state = WORLD_STATE_PAUSE;
		break;
	case GLUT_LEFT_BUTTON:
		if (world->state == WORLD_STATE_PAUSE && state == GLUT_DOWN) {
			world->state = WORLD_STATE_RUN;
		}
	case GLUT_RIGHT_BUTTON:
		if (world->state == WORLD_STATE_PAUSE && state == GLUT_DOWN) {
			world->state = WORLD_STATE_ONEFRAME;
		}
	}
}

void timer(GLint) {
	FMODSystem->update(); // Sync FMOD on each frame

	// Agent is alive:
	if (world->state != WORLD_STATE_PAUSE) {
		if (world->state == WORLD_STATE_ONEFRAME) {
			// Draw current frame and pause again
			world->state = WORLD_STATE_PAUSE;
		}
		world->updateWorld();
		glutPostRedisplay();
	}
	// Agent is dead, do some animation stuff:
	if (world->state == WORLD_STATE_OVER) {
		if (!world->agent->triedToGoBack) {
			if (abs(world->agent->offsetX - world->agent->targetX) > 2) {
				world->agent->offsetX = lerp(world->agent->offsetX, world->agent->targetX, 0.025);
			}
			if (world->agent->deathAnimFlag == 0) {
				if (abs(world->agent->scale - world->agent->targetScale) > 1) {
					world->agent->scale = lerp(world->agent->scale, world->agent->targetScale, 0.08);
				}
				else {
					world->agent->deathAnimFlag = 1;
				}
			}
			else {
				world->agent->scale = lerp(world->agent->scale, world->agent->initialScale, 0.08);
			}
		}

		if (world->FOVX < 1000 && world->FOVY < 1000) {
			world->FOVX = lerp(world->FOVX, 1000, 0.02);
			world->FOVY = lerp(world->FOVY, 1000, 0.02);
		}
		if (world->LOOKX < world->agent->offsetX && world->LOOKY < world->agent->offsetY) {
			world->LOOKX = lerp(world->LOOKX, -world->agent->offsetX, 0.02);
			world->LOOKY = lerp(world->LOOKY, -world->agent->offsetY, 0.02);
		}

		if (world->FOVX > 970 && world->FOVY > 970) {
			world->deathAnimDone = 1;
		}
		glViewport(world->LOOKX, world->LOOKY, world->FOVX, world->FOVY);
		glutPostRedisplay();
	}
	glutTimerFunc(1000.0 / 60.0, timer, 0); // 60 FPS
}

// Initialize sound effects with FMOD
void initSounds() {
	unsigned int      version;
	void             *extradriverdata = 0;
	result = FMOD::System_Create(&FMODSystem);
	result = FMODSystem->getVersion(&version);

	FMODSystem->init(32, FMOD_INIT_NORMAL, extradriverdata);
	FMODSystem->createStream("media/ambience.wav", FMOD_DEFAULT, 0, &ambience);
	ambience->setMode(FMOD_LOOP_NORMAL);                         
	FMODSystem->createSound("media/gameOver.wav", FMOD_DEFAULT, 0, &gameOver);
	FMODSystem->createSound("media/coin.wav", FMOD_DEFAULT, 0, &coinPick);
	FMODSystem->createSound("media/hit.wav", FMOD_DEFAULT, 0, &carHit);
	FMODSystem->createSound("media/footstep_grass.wav", FMOD_DEFAULT, 0, &footstepGrass);
	FMODSystem->createSound("media/footstep_road.wav", FMOD_DEFAULT, 0, &footstepRoad);
	FMODSystem->createSound("media/horn_1.wav", FMOD_DEFAULT, 0, &horn1);
	FMODSystem->createSound("media/horn_2.wav", FMOD_DEFAULT, 0, &horn2);
	FMODSystem->createSound("media/horn_3.wav", FMOD_DEFAULT, 0, &horn3);
	FMODSystem->createSound("media/horn_4.wav", FMOD_DEFAULT, 0, &horn4);
}

int main(GLint argc, char** argv) {
	initSounds();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(ww, wh);
	glutCreateWindow("Istanbul Traffic Simulator 2017");
	init();
	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutSpecialFunc(processSpecialKeys);
	glutKeyboardFunc(processNormalKeys);
	glutMouseFunc(mouseInput);
	glutTimerFunc(1000.0 / 60.0, timer, 0); // 60 FPS
	glutMainLoop();
}