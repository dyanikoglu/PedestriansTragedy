#include "SoundManager.hpp"


void SoundManager::initSounds() {
  FMOD::System_Create(&FMODSystem);
  FMODSystem->init(32, FMOD_INIT_NORMAL, 0);
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

