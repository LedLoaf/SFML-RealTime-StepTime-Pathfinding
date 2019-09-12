#include "Level.h"
#include "Entity.h"
#include <iostream>
#include <SFML/Graphics.hpp>

// Global Functions
/* can use to allow the enemies to only attack within a specific distance*/
float distanceBetweenPoints(sf::Vector2f position1, sf::Vector2f position2);
// for visual information 
std::string displayInfo(bool status);
// for visual information 
std::string displayMsg(bool status, const std::string& isTrue, const std::string& isFalse);


/* ABOUT THIS 
 * The main functions are located in Entity.h . The function is called UpdatePathfinding.
 * I've added 2 additional functions to show Real-Time and Step-Time styles of pathfinding, which is simply just retrieving the tile position rather than by adding velocity
 * Level is pretty much irrelevant in terms of pathfinding, except for one important function restNodes() and of course the Tile struct. Everything else is just for visuals
 * There are a basic bugs with switching between Real-Time and Step-Time pathfinding. This is mostly just educational for the A* algorithm.
 */

int main() {
	sf::RenderWindow window{{1280,768},"Pathfinding Example: Real-Time and Step-Time"};
	window.setFramerateLimit(60);

	sf::View view{{-300,0,1280,768}};

	Level level;
	level.load("map.map", "tiles.png");

	Entity player;
	/* previous tile the player is on */
	Tile* m_playerPreviousTile = nullptr;
	/* the tile the player is on */
	Tile* currentTile = level.getTile(player.getPosition());		// getTile retrieves the Tile based off the position

	sf::Font font;
	if (!font.loadFromFile("Dosis-Light.ttf"))
		std::cout << "Failed to load Dosis-Light.ttf\n";

	sf::Text directions;
	directions.setFillColor(sf::Color{240,128,128});		// light coral
	directions.setFont(font);
	directions.setCharacterSize(18);
	directions.setStyle(sf::Text::Bold);

	// basic clock stuff
	sf::Clock clock;
	sf::Time elapsedTime = sf::Time::Zero;
	sf::Time FrameTime = sf::seconds(1.f / 15.f);		// 15 fps

	/* toggling between real-time and step-time pathfinding */
	bool isPathfindingRealTime = true;

	while(window.isOpen()) {
		sf::Time dt = clock.restart();
		elapsedTime += dt;

		sf::Event event;
		while(window.pollEvent(event)) {
			if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
				window.close();
			if(event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Num2)
					player.showOpenListColor = !player.showOpenListColor;
				if (event.key.code == sf::Keyboard::Num3)
					player.showFCostColor = !player.showFCostColor;
				if (event.key.code == sf::Keyboard::Num4)
					player.showCurrentPathColor = !player.showCurrentPathColor;
				if (event.key.code == sf::Keyboard::Num5)
					player.showPathColor = !player.showPathColor;
				if (event.key.code == sf::Keyboard::Num6)
					player.showStartColor = !player.showStartColor;
				if (event.key.code == sf::Keyboard::Num7)
					player.showGoalColor = !player.showGoalColor;
				/* NOTE* this can't be turned on by setAllColors(true) and must be manual*/
				if (event.key.code == sf::Keyboard::Num8)
					player.showTilesNotCheckedColor = !player.showTilesNotCheckedColor;
				/* turn off all color visuals */
				if (event.key.code == sf::Keyboard::Num9)
					player.setAllColorsStatus(false);
				/* turn on all color visuals (NOTE* EXCEPT showTilesNotCheckedColor)*/
				if (event.key.code == sf::Keyboard::Num0)
					player.setAllColorsStatus(true);
				/* toggle real-time and step-time pathfinding */
				if (event.key.code == sf::Keyboard::R)
					player.isRealTimeMode = !player.isRealTimeMode;

			}
		}

		/* move the view with WASD keys */
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			view.move(0, -10);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			view.move(0, 10);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			view.move(-10, 0);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			view.move(10, 0);
		}


		// execute pathfinding manually, instead of every click using space
		/*	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			if (playerCurrentTile != nullptr) {
				if (m_playerPreviousTile != playerCurrentTile) {
					m_playerPreviousTile = playerCurrentTile;

					player.updatePathfinding(level, playerCurrentTile->sprite.getPosition());

				}
			}
		}*/


		/* getting the mouse positions to find out what tile we clicked*/
		auto mpos = sf::Mouse::getPosition(window);
		auto pos = window.mapPixelToCoords(mpos,view);
		/* divide by tile-size to find the correct tile */
		mpos.x /= 32;
		mpos.y /= 32;

		if (elapsedTime > FrameTime) {
			elapsedTime -= FrameTime;

			/* enemy key input */
			player.handleInput();

			/* find out where we clicked */
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
				/* quick checking to make sure we are in-bounds */
				if (pos.x >= 0 && pos.y >= 0 && pos.x < level.getTotalWidth() && pos.y < level.getTotalHeight()) {
					
					currentTile = level.getTile(pos);
					currentTile->sprite.setColor(sf::Color::Red);
					std::cout << "(" << currentTile->columnIndex << ", " << currentTile->rowIndex << ")\n";
				}

				/* if the tile the player is on isn't nullptr */
				if (currentTile != nullptr) {
					/* if our previous tile doesn't equal the tile under the player*/
					if (m_playerPreviousTile != currentTile) {
						/* assign the current tile as the previous */
						m_playerPreviousTile = currentTile;
						/* start the pathfinding algorithm with the level and entity's current location*/
						player.updatePathfinding(level, currentTile->sprite.getPosition());

					}
				}
			}		

			player.update(level, FrameTime.asSeconds());
		}


		window.clear();

		window.setView(view);
		/* basic drawing of the map layers */
		level.draw(1, view.getCenter().x-400, view.getCenter().y- 400, &window);
		level.draw(2, view.getCenter().x-400, view.getCenter().y- 400, &window);
		level.draw(0, view.getCenter().x-400, view.getCenter().y- 400, &window);
		/* standard draw method to reveal entity sprite (rectangle)*/
		player.draw(&window);
		window.setView(window.getDefaultView());


		/* some easy visuals to relay information */
		if (currentTile)
		{
			std::string str =
				"===\t\tDirections\t\t==="
				"\nWASD Move View: position(" + std::to_string((int)player.getPosition().x) + ", " + std::to_string((int)player.getPosition().y) + ")" +
				"\nLeft Click: Moving to Tile[" + std::to_string(currentTile->columnIndex) + "][" + std::to_string(currentTile->rowIndex) + "]" +
				"\n(Numbers 0-10 for toggling)" +
				"\n1: Diagonal pathfinding - " + displayInfo(player.allowDiagonals).c_str() +
				"\n2: OpenList Colors - " + displayInfo(player.showOpenListColor).c_str() +
				"\n3: FCost Color - " + displayInfo(player.showFCostColor).c_str() +
				"\n4: Current Path Colors - " + displayInfo(player.showCurrentPathColor).c_str() +
				"\n5: Path Colors - " + displayInfo(player.showPathColor).c_str() +
				"\n6: Start Colors - " + displayInfo(player.showStartColor).c_str() +
				"\n7: Goal Colors - " + displayInfo(player.showGoalColor).c_str() +
				"\n8: Toggle Tiles Not Checked Colors - " + displayInfo(player.showTilesNotCheckedColor).c_str() +
				"\n9: Reset all colors off - " + displayMsg(player.isAllOff(), "True", "False") +
				"\n0: Reset all colors on - " + displayMsg(!player.isAllOff(), "True", "False") +
				"\nR key: Toggle Mode: " + displayMsg(player.isRealTimeMode,"Real-Time","Step-Time") + " pathfinding";
			directions.setPosition(4, 0);
			directions.setString(str);
		}


		/* draw text for some direction */
		window.draw(directions);

		window.display();
	}
}

/* can use to allow the enemies to only attack within a specific distance*/
float distanceBetweenPoints(sf::Vector2f position1, sf::Vector2f position2)
{
	return (abs(sqrt(((position1.x - position2.x) * (position1.x - position2.x)) + ((position1.y - position2.y) * (position1.y - position2.y)))));

}

// for visual information 
std::string displayInfo(bool status)
{
	return status ? std::string("On") : std::string("Off");
}
// for visual information 
std::string displayMsg(bool status, const std::string& isTrue, const std::string& isFalse)
{
	return status ? isTrue : isFalse;
}
