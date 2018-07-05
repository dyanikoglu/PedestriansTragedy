#include "Tools.hpp"
#include "World.hpp"

World::World(GLint squareSize, GLfloat ww, GLfloat wh, GLint startDifficulty)
{
  // Init sounds
  soundManager = new SoundManager();
  soundManager->initSounds();

  // Init variables:
  this->squareSize = squareSize;
  this->leftSpawnPoint = -2 * squareSize;
  this->rightSpawnPoint = ww;
  this->difficulty = startDifficulty;
  this->frameTimer = 0;
  this->score = 0;
  this->coinTextTimeout = 0;
  this->coinTextX = 0;
  this->coinTextY = 0;
  this->deathAnimDone = 0;
  this->FOVX = ww;
  this->FOVY = wh;
  this->ww = ww;
  this->wh = wh;
  this->LOOKX = 0;
  this->LOOKY = 0;


  initWorld(); // Initialize the world
}

void World::initWorld() 
{
  srand(time(NULL));

  soundManager->FMODSystem->playSound(soundManager->ambience, 0, false, &(soundManager->ambienceChannel));
  soundManager->ambienceChannel->setVolume(0.5f);

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
  while (sidewalks < 6)
  {
    this->worldObjects_roadsPavs.clear();
    sidewalks = 0;
    GLint consecuviteRoad = 0;
    GLint randNum = 0;
    for (GLint i = 0; i < (hSquareCount); i++) {
      initials = { Vertex(0,i*squareSize), Vertex(0,(i + 1)*squareSize), Vertex(wh,(i + 1)*squareSize), Vertex(wh,i*squareSize) };
      // First & Last row is always pavement, continue building roads until random number is equal to consecutive road count
      if (i == 0 || i == (hSquareCount)-1 || (consecuviteRoad == randNum)) {
        worldLayout[i] = WorldObject::TYPE::PAVEMENT;
        roadStatus[i] = -1;
        roadDirection[i] = 0;
        this->worldObjects_roadsPavs.push_back(new WorldObject(initials, WorldObject::TYPE::PAVEMENT));

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
              this->worldObjects_trees.push_back(new WorldObject(initials, WorldObject::TYPE::FLORA_TREE_1));
            }
            else {
              this->worldObjects_trees.push_back(new WorldObject(initials, WorldObject::TYPE::FLORA_TREE_2));
            }
          }
        }

        consecuviteRoad = 0;
        sidewalks++;
        randNum = (rand() % 2) + 3;
      }
      else {
        worldLayout[i] = WorldObject::TYPE::ROAD;
        roadStatus[i] = 1;
        this->worldObjects_roadsPavs.push_back(new WorldObject(initials, WorldObject::TYPE::ROAD));
        roadDirection[i] = 1 - 2 * (rand() % 2);
        consecuviteRoad++;
      }
    }
  }

  // Create player agent
  agent = new Agent(squareSize, squareSize, ((wSquareCount) / 2 * squareSize), 0, blockedSquares, ww, wh);
  this->state = STATE::RUN;
};

void World::drawRoadLines() 
{
  GLint row = 1;
  glColor3f(1.0f, 1.0f, 1.0f);
  glLineWidth(2);
  for (GLint curr = squareSize; curr <= wh; curr += squareSize)
  {
    if (worldLayout[row] == WorldObject::TYPE::ROAD && worldLayout[row - 1] == WorldObject::TYPE::ROAD) {
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


void World::drawWorldObjects_Layer1()
{
  for (vector<WorldObject*>::iterator obj = this->worldObjects_roadsPavs.begin(); obj != this->worldObjects_roadsPavs.end(); ++obj) {
    (*obj)->draw();
  }
}


void World::drawWorldObjects_Layer2() 
{
  for (vector<WorldObject*>::iterator obj = this->worldObjects_trees.begin(); obj != this->worldObjects_trees.end(); ++obj) {
    (*obj)->draw();
  }
}


void World::spawnPawn() 
{
  GLint randomRow = rand() % (hSquareCount);
  // Selected row is a road
  if (worldLayout[randomRow] == WorldObject::TYPE::ROAD) {
    // Road is free to spawn a new car
    if (roadStatus[randomRow]) {
      // Traffic flow is to right
      if (roadDirection[randomRow] == Actor::DIRECTION::RIGHT) {
        pawns.push_back(new Pawn(difficulty, squareSize, leftSpawnPoint, randomRow * squareSize, randomRow, (Actor::DIRECTION)roadDirection[randomRow]));
      }
      // Traffic flow is to left
      else if (roadDirection[randomRow] == Actor::DIRECTION::LEFT) {
        pawns.push_back(new Pawn(difficulty, squareSize, rightSpawnPoint, randomRow * squareSize, randomRow, (Actor::DIRECTION)roadDirection[randomRow]));
      }
      roadStatus[randomRow] = 0; // Road is busy now, set road as busy
      roadSpawnTimeStamps[randomRow] = this->frameTimer;
    }
  }
}


void World::spawnCoin() 
{
  this->coins.push_back(new Coin(5, this->squareSize * (rand() % wSquareCount) + (squareSize / 3), this->squareSize * (rand() % (hSquareCount)) + (squareSize / 2), this->frameTimer + (rand() % 100) + 600, 5));
}


void World::updateWorld()
{
  this->frameTimer++;

  // Try spawning a pawn on each 5th frame.
  if (frameTimer % 5 == 0)
  {
    this->spawnPawn();
  }

  // Try spawning a coin on each 150th frame, remove timed out coins, max spawned coin amount is 6
  for (int i = 0; i < coins.size(); i++)
  {
    // Timeout coin
    if (coins.at(i)->timeout <= this->frameTimer) 
    {
      coins.erase(coins.begin() + i);
    }
  } if (coins.size() < 6 && frameTimer % 150 == 0) {
    spawnCoin();
  }

  // Increase speed of pawns on each 2000th frame
  if (frameTimer % 2000 == 0 && this->difficulty < 4)
  {
    this->difficulty++;
  }

  // Set roads as available if some amount of time passed since a pawn is spawned on this road(80 frames).
  for (GLint i = 0; i < hSquareCount; i++)
  {
    if (this->frameTimer - this->roadSpawnTimeStamps[i] > 80) {
      this->roadStatus[i] = 1;
    }
  }

  // Move pawns
  for (vector<Pawn*>::iterator pawn = this->pawns.begin(); pawn != this->pawns.end(); ++pawn)
  {
    (*pawn)->move();
  }

  // Move agent with a smooth animation
  this->agent->move();

  // Animate coins
  vector<GLint> toRemove;
  for (int i = 0; i < coins.size(); i++)
  {
    if (coins.at(i)->isTaken) {
      coins.at(i)->taken();
      if (abs(coins.at(i)->offsetX - coins.at(i)->targetX) < 5)
      {
        toRemove.push_back(i);
      }
    }
    else 
    {
      coins.at(i)->idle();
    }
  }

  // Delete picked coins from data structure
  for (vector<GLint>::iterator ind = toRemove.begin(); ind != toRemove.end(); ++ind)
  {
    coins.erase(coins.begin() + (*ind));
  }
  toRemove.clear();

  // Check if there exists a collision
  this->checkCollisions();

  // Agent is dead, do some animation stuff:
  if (this->state == STATE::GAME_OVER) 
  {
    if (!this->agent->triedToGoBack) 
    {
      if (abs(this->agent->offsetX - this->agent->targetX) > 2) 
      {
        this->agent->offsetX = Tools::lerp(this->agent->offsetX, this->agent->targetX, 0.025);
      }
      if (this->agent->deathAnimFlag == 0)
      {
        if (abs(this->agent->scale - this->agent->targetScale) > 1) {
          this->agent->scale = Tools::lerp(this->agent->scale, this->agent->targetScale, 0.08);
        }
        else 
        {
          this->agent->deathAnimFlag = 1;
        }
      }
      else {
        this->agent->scale = Tools::lerp(this->agent->scale, this->agent->initialScale, 0.08);
      }
    }

    if (this->FOVX < 1000 && this->FOVY < 1000) 
    {
      this->FOVX = Tools::lerp(this->FOVX, 1000, 0.02);
      this->FOVY = Tools::lerp(this->FOVY, 1000, 0.02);
    }
    if (this->LOOKX < this->agent->offsetX && this->LOOKY < this->agent->offsetY) {
      this->LOOKX = Tools::lerp(this->LOOKX, -this->agent->offsetX, 0.02);
      this->LOOKY = Tools::lerp(this->LOOKY, -this->agent->offsetY, 0.02);
    }

    if (this->FOVX > 970 && this->FOVY > 970) 
    {
      this->deathAnimDone = 1;
    }
    glViewport(this->LOOKX, this->LOOKY, this->FOVX, this->FOVY);
    glutPostRedisplay();
  }
}

void World::checkCollisions()
{
  // Collision of Spawn Point - Pawn, delete pawn pointer from the vector
  for (GLint i = 0; i < this->pawns.size(); i++)
  {
    if (pawns.at(i)->direction == Actor::DIRECTION::LEFT) 
    {
      if ((pawns.at(i)->getVertex(0).x * pawns.at(i)->scale) + pawns.at(i)->offsetX <= leftSpawnPoint) {
        roadStatus[pawns.at(i)->assignedTo] = 1;
        pawns.erase(pawns.begin() + i);
      }
    }
    else if (pawns.at(i)->direction == Actor::DIRECTION::RIGHT)
    {
      if ((pawns.at(i)->getVertex(0).x * pawns.at(i)->scale) + pawns.at(i)->offsetX >= rightSpawnPoint) {
        roadStatus[pawns.at(i)->assignedTo] = 1;
        pawns.erase(pawns.begin() + i);
      }
    }
  }

  // Collision of Pawn - Pawn, equalize these pawns' velocities
  for (GLint i = 0; i < this->pawns.size(); i++)
  {
    for (GLint j = i + 1; j < this->pawns.size(); j++) 
    {
      // If pawns are on same road-line
      if (this->pawns.at(i)->assignedTo == this->pawns.at(j)->assignedTo) 
      {
        GLint result = Tools::checkIntersection(this->pawns.at(i)->getCollisionBound(Actor::COLLISION_TYPE::LOOSE), this->pawns.at(j)->getCollisionBound(Actor::COLLISION_TYPE::LOOSE));
        if (result) 
        {
          // Set higher velocity to lower one, so pawns won't crash
          // [j] hits [i]
          if (abs(this->pawns.at(i)->velocity) < abs(this->pawns.at(j)->velocity))
          {
            this->pawns.at(j)->pullBack();
            this->pawns.at(j)->velocity = this->pawns.at(i)->velocity;
          }
          // [i] hits [j]
          else 
          {
            this->pawns.at(i)->pullBack();
            this->pawns.at(i)->velocity = this->pawns.at(j)->velocity;
          }
        }
      }
    }
  }

  // Collision of Pawn - Agent, honk horn or kill agent
  if (this->state != STATE::GAME_OVER)
  {
    for (GLint i = 0; i < this->pawns.size(); i++) 
    {
      if (this->pawns.at(i)->assignedTo == this->agent->currentRow) 
      {
        GLint intersectionResult = Tools::checkIntersection(this->pawns.at(i)->getCollisionBound(Actor::COLLISION_TYPE::TIGHT), this->agent->getCollisionBound(Actor::COLLISION_TYPE::TIGHT));

        // Pawns can honk their horn if agent is close to front side of pawn, do an extra collision check for this:
        if (this->pawns.at(i)->honked == 0)
        {
          GLint hornResult = Tools::checkIntersection(this->pawns.at(i)->getCollisionBound(Actor::COLLISION_TYPE::LONG), this->agent->getCollisionBound(Actor::COLLISION_TYPE::TIGHT));
          if (hornResult) {
            this->pawns.at(i)->honked = 1; // Each pawn can try their chance for honking horn one time.
                                           // %50 chance:
            if (rand() % 2 == 0) {
              GLint hornType = rand() % 4; // Random horn type
              switch (hornType) {
              case 0:
                soundManager->FMODSystem->playSound(soundManager->horn1, 0, false, &(soundManager->effectChannel));
                break;
              case 1:
                soundManager->FMODSystem->playSound(soundManager->horn2, 0, false, &(soundManager->effectChannel));
                break;
              case 2:
                soundManager->FMODSystem->playSound(soundManager->horn3, 0, false, &(soundManager->effectChannel));
                break;
              case 3:
                soundManager->FMODSystem->playSound(soundManager->horn4, 0, false, &(soundManager->effectChannel));
                break;
              default:
                break;
              }
            }

          }
        }
        // Pawn hits to agent:
        if (intersectionResult) 
        {
          soundManager->ambienceChannel->setVolume(0.15f);
          soundManager->FMODSystem->playSound(soundManager->gameOver, 0, false, &(soundManager->effectChannel));
          soundManager->FMODSystem->playSound(soundManager->carHit, 0, false, &(soundManager->effectChannel));
          this->agent->targetX = this->agent->offsetX + 20 * this->pawns.at(i)->velocity;
          this->pawns.at(i)->velocity = 0;
          // Rotate agent to right direction:
          if (this->pawns.at(i)->direction == Actor::DIRECTION::LEFT)
          {
            this->agent->vertices = this->agent->rotate_left;
          }
          else
          {
            this->agent->vertices = this->agent->rotate_right;
          }

          this->state = STATE::GAME_OVER;
        }
      }
    }
  }


  // Collision of Agent - Coin, pick up the coin
  for (GLint i = 0; i < this->coins.size(); i++)
  {
    GLint result = Tools::contains(Vertex(this->coins.at(i)->offsetX, this->coins.at(i)->offsetY), this->agent->getCollisionBound(Actor::COLLISION_TYPE::LOOSE));
    if (result)
    {
      soundManager->FMODSystem->playSound(soundManager->coinPick, 0, false, &(soundManager->effectChannel));
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


void World::showScore() {
  char scoreText[21];
  sprintf(scoreText, "Score: %d", this->score);
  glColor3f(1.0f, 1.0f, 0.0f);
  Tools::drawText(scoreText, 0, wh - 0.75*this->squareSize, GLUT_BITMAP_HELVETICA_18);
}


void World::drawWorld()
{
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
  if (coinTextTimeout > frameTimer) 
  {
    glColor3f(1.0f, 1.0f, 0.0f);
    Tools::drawText("+5", coinTextX, coinTextY, GLUT_BITMAP_HELVETICA_18);
  }

  this->showScore();

  // If death animation of agent is completed, show paraphrase and score on screen:
  if (this->deathAnimDone)
  {
    glColor3f(1.0f, 0.0f, 0.0f);
    if (this->agent->triedToGoBack)
    {
      Tools::drawText("Don't be a coward", this->agent->offsetX - this->squareSize * 1.25, this->agent->offsetY + 2 * this->squareSize, GLUT_BITMAP_TIMES_ROMAN_24);
    }
    else
    {
      Tools::drawText("WASTED", this->agent->offsetX - this->squareSize / 2, this->agent->offsetY + 2 * this->squareSize, GLUT_BITMAP_TIMES_ROMAN_24);
    }
    char scoreText[21];
    sprintf(scoreText, "Your score: %d", this->score);
    glColor3f(1.0f, 0.0f, 0.0f);
    Tools::drawText(scoreText, this->agent->offsetX - this->squareSize / 1.70, this->agent->offsetY + 1.5 * this->squareSize, GLUT_BITMAP_HELVETICA_18);
  }
}
