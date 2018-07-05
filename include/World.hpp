#pragma once
#include <time.h>
#include <gl/freeglut.h>
#include "Agent.hpp"
#include "WorldObject.hpp"
#include "Coin.hpp"
#include "Pawn.hpp"
#include "SoundManager.hpp"

// Main world object
class World {
public:
  enum STATE
  {
    RUN=1,
    GAME_OVER=3,
    PAUSE=0,
    ONEFRAME=2
  };

  GLfloat ww;
  GLfloat wh;

  SoundManager* soundManager;
  Agent * agent;
  vector<WorldObject*> worldObjects_roadsPavs;
  vector<WorldObject*> worldObjects_trees;
  vector<Coin*> coins;
  vector<Pawn*> pawns;
  GLint* worldLayout; // Stores each row's type (Pavement or road?)
  GLint wSquareCount;
  GLint hSquareCount;

  GLint* roadStatus; // Is the road busy or free to spawn new car?
  GLint* roadDirection; // What is the traffic flow direction of road?
  GLint* roadSpawnTimeStamps; // When the last pawn is spawned on the road?

  GLint coinTextTimeout;
  GLfloat coinTextX;
  GLfloat coinTextY;
  GLint score; // Total score of player

  GLint squareSize;
  GLint leftSpawnPoint;
  GLint rightSpawnPoint;
  GLint frameTimer; // Global timer based on frames
  STATE state; // Game state

  GLint deathAnimDone; // Is agent's death animation completed?
  GLfloat FOVX; // Field of view on X (used when agent is dead)
  GLfloat FOVY; // Field of view on Y (used when agent is dead)
  GLfloat LOOKX; // Look at X (used when agent is dead)
  GLfloat LOOKY; // Look at Y (used when agent is dead)
  GLfloat difficulty; // Difficulty of game, decides initial velocity of spawned pawns

  World(GLint squareSize, GLfloat ww, GLfloat wh, GLint startDifficulty);

  // Initialize the game world
  void initWorld();

  // Draw road lines
  void drawRoadLines();

  // First(lower) layer of world objects
  void drawWorldObjects_Layer1();

  // Second(upper) layer of world objects, it includes trees
  void drawWorldObjects_Layer2();

  // Create a random pawn and push it into data structure
  void spawnPawn();

  // Create a coin and push it into data structure
  void spawnCoin();

  // Called on each frame, updates the world object
  void updateWorld();

  void checkCollisions();

  // Show score on screen
  void showScore();

  // Draw each object in game world into screen
  void drawWorld();
};