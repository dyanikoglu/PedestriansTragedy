#pragma once
#include "FMOD\fmod.hpp"

class SoundManager {
public:

  // Sound effect pointers
  FMOD::System *FMODSystem;
  FMOD::Channel *ambienceChannel = 0;
  FMOD::Channel *effectChannel = 0;
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

  // Initialize sound effects with FMOD
  void initSounds();
};

