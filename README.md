# SFML-RealTime-StepTime-Pathfinding
Simple example of pathfinding in SFML with the ability to toggle between Real-Time and Step-Time. You can learn more information from a great
book I read called Procedural Generation Content using C++. Which goes through making a small Rogue game in SFML. I don't know of many books
that go through pathfinding and explain it so simple.

Please link up SFML 2.4+ or add them to a folder named External (the solution links SFML from there) in the solution directory.

Use WASD to move the camera around
Use numbers 0-9 to toggle various colors on and off to change the visual indicators.
1 to allow diagnals
2 to toggle OpenList colors
3 to toggle FCost colors (todo add numbers to tiles for F G H)
4 to toggle current path colors
5 to toggle path colors
6 to toggle starting tile colors
7 to togggle goal tile colors
8 to toggle tiles not checked colors
9 reset all colors OFF
0 reset all colors ON
R to toggle to Real-Time or Step-Time pathfinding

The most important things to look at for the pathfinding algorithm itself are:
UpdatePathfinding() in Entity.cpp 
UpdateByRealTime() in Entity.cpp
UpdateByStepTime() in Entity.cpp
ResetNodes() in Level.cpp
And lastly, the Tile struct in Level.cpp.

Everything else is just for color visuals, and the ability to see tiles on a map. 
If I took out the visuals the program would be extremely small, with just the above functions needed.

One thing to point out is the rowsIndex and colsIndex in the Tile struct are just easier access to what is basically this->sprite.getPosition / TILE_SIZE. You can simply do that as well for checking the nodes.

Thanks, hope this helps someone. I'm not a great teacher, but I did learn how to do A* like this. 
