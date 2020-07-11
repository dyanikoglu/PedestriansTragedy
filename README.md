# A Pedestrian's Tragedy
![gif](https://github.com/dyanikoglu/PedestriansTragedy/raw/master/Readme_GIF.gif)

A road crossing game developed with OpenGL & FMOD on C++.

## Features
- Trees will appear randomly on pavements, they will block agent's movement.
- Agent will move with a smooth animation instead of teleporting to target location.
- Picked coins will fly onto score text at the corner of screen with increasing scale. Idle coins on game world will animate from left to right slowly. Also, a feedback text will appear at the top of picked coin.
- If a car/truck hits to agent, agent will be driven away to hit direction with a sound effect. After this, view will zoom at agent, and a paraphrase will apper at the top of agent.
- Spawned cars/trucks will sense each other. They will slow down after getting too close to other cars/trucks.
- Cars/Trucks will sense agent if it's too close to front side of car/truck. They will honk their horn to agent with %50 chance.
- Initial velocity of spawned cars/trucks will increase as the time goes by.

## Environment Setup
- Solution is created with Visual Studio 2017.

- It's assumed that OpenGL, FreeGLUT and Windows SDK 8.1 is already installed correctly on the system. This guide can be followed to get your dev. environment ready: http://www.cs.uky.edu/~cheng/cs633/OpenGLInstallGuideWindows.pdf

- Project must be compiled on x86 mode.

- If a standalone exe is needed, media folder and fmod.dll should be on same directory with exe file.

## Game Rules
- Player can't go back from it's current direction. If so, game is over immediately.
- If agent collides with any of car/truck in game world, game is over immediately.
- If agent collides with any coin in game world, score will increse by 5.
- Each move on direction of agent will increase score by 1.
- On each 2000th frame of game, pawns will spawn with higher speeds. In other words, difficulty will increase as time goes by.
- Game will continue until agent dies, or tries to go wrong direction.
- Agent cannot pass through center of the trees on pavements.

## Controls
- Arrow Keys -> Movement
- q -> Quit from game
- Middle Mouse Button -> Pause Game
- Left Mouse Button -> (If paused) Continue game
- Right Mouse Button -> (If paused) Advance game by 1 frame

## Licence
Copyright (C) 2017-2018 Doğa Can YANIKOĞLU

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/

## Contributing
Contributors are encouraged to fork this repository and issue pull requests.
