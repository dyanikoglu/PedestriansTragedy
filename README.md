# A Pedestrian's Tragedy

![Game Screenshot](https://i.hizliresim.com/AL9zjX.png)

A road crossing game developed with OpenGL & FMOD on C++.

## Contents

I. Environment Setup

II. Features

III. Game Rules

IV. Controls


I. Environment Setup
---------------------
- It's assumed that OpenGL and FreeGLUT is already installed correctly on the system. If not, they should be included in project properties. Code should compile on Visual Studio 2017 / Windows 10 without problem.

- Just launch project by opening "PedestriansTragedy.vcxproj". Environment is ready to compile Main.cpp for this project file.

- Project must be compiled on x86 mode.

- If it's needed to create a project from scratch, fmod_vc.lib library should be included in linker, and FMOD, media folders shold be at same folder with main.cpp. 

- If a standalone exe is needed, media folder and fmod.dll should be on same directory with exe file.


II. Features
------------------
- Trees will appear randomly on pavements, they will block agent's movement.
- Agent will move with a smooth animation instead of teleporting to target location.
- Picked coins will fly onto score text at the corner of screen with increasing scale. Idle coins on game world will animate from left to right slowly. Also, a feedback text will appear at the top of picked coin.
- If a car/truck hits to agent, agent will be driven away to hit direction with a sound effect. After this, view will zoom at agent, and a paraphrase will apper at the top of agent.
- Spawned cars/trucks will sense each other. They will slow down after getting too close to other cars/trucks.
- Cars/Trucks will sense agent if it's too close to front side of car/truck. They will honk their horn to agent with %50 chance.
- Initial velocity of spawned cars/trucks will increase as the time goes by.

III. Game Rules
-------------------
- Player can't go back from it's current direction. If so, game is over immediately.
- If agent collides with any of car/truck in game world, game is over immediately.
- If agent collides with any coin in game world, score will increse by 5.
- Each move on direction of agent will increase score by 1.
- On each 2000th frame of game, pawns will spawn with higher speeds. In other words, difficulty will increase as time goes by.
- Game will continue until agent dies, or tries to go wrong direction.
- Agent cannot pass through center of the trees on pavements.


IV. Controls
------------
- Arrow Keys -> Movement
- q -> Quit from game
- Middle Mouse Button -> Pause Game
- Left Mouse Button -> (If paused) Continue game
- Right Mouse Button -> (If paused) Advance game by 1 frame


## Licence

Copyright (C) 2017 Doğa Can YANIKOĞLU

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/

## Contributing

Contributors are encouraged to fork this repository and issue pull requests.
