# A Pedestrian's Tragedy

![Game Screenshot](https://i.hizliresim.com/AL9zjX.png)

A road crossing game developed with OpenGL & FMOD on C++.

## Contents

I. Basic Implementation Details

II. Features

III. Game Rules

IV. Controls



I. Implementation Details
-------------------------
Implementation is made on C++, using object oriented approach. 

Coordinate system is implemented with scale, offset and unit polygon of object. Unit polygon is 1x1 presentation of desired object shape. This polygon vertices are multiplied by scale value, and then summed with offset values. Game world is splitted into squares(25x25 in my implementation). Each object's place is initialized with respect to this squared layout. This approach makes my implementation more organized and easy to manipulate.

"Actor" is main class for dynamic objects in game world. It stores informations like scale, unit polygon, and offset of the object. "Coin" is class for coins, "pawn" is a class for cars/trucks, and "agent" is the class for player agent. Each of them implements their own functions and stores their own variables. Each of them inherits from "Actor" class.

"WorldObject" is main class for static objects in game world. It's used for roads, pavements, and trees in the game world.

"World" is main class of the game world. It stores each created object in game world, updates them, and draws required ones to screen. It also handles collisions, animations, game states, scoring etc.

Smooth animations seen in the game are made with some linear interpolation manipulation. With a constant delta value, current point can be interpolated towards target point, and this generated value can be written into current point back. If we repeat this process on each frame until the object reaches it's target position, we will get very smooth transition effect between these two points.

Collisions are handled with orientation tests. For detecting collision between 2 polygons, line segments of these polygons orientation tested with each other. This approach is used for collision of pawn-agent, and pawn-pawn. On the other hand, for detecting coin-agent collision, coin's center point is checked for if it's inside agent's polygon or not. Using a bounding box for circle is not a nice solution, and checking each vertex of circle is not a cheap solution. So, this approach seems like best one to me.

FMOD API is used for sound system implementation.


II.Features
------------------
- Cars/Trucks are drawn like a top-down car/truck instead of a rectangle.
- Trees will appear randomly on pavements, they will block agent's movement.
- Agent will move with a smooth animation instead of teleporting to target location.
- Picked coins will fly onto score text at the corner of screen with increasing scale. Idle coins on game world will animate from left to right slowly. Also, a feedback text will appear at the top of picked coin.
- If a car/truck hits to agent, agent will be driven away to hit direction with a sound effect. After this, view will zoom at agent, and a paraphrase will apper at the top of agent.
- Spawned cars/trucks will sense each other. They will slow down after getting too close to other cars/trucks.
- Cars/Trucks will sense agent if it's too close to front side of car/truck. They will honk their horn to agent with %50 chance.
- Initial velocity of spawned cars/trucks will increase as the time goes by.
- Agent can rotate to each direction. (instead of just up or down)
- Various sound effects

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
