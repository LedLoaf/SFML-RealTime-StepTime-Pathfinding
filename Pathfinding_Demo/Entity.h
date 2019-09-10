#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

struct Tile;
class Level;


class Entity
{
public:
	Entity();

	/* handle events */
	void handleInput();
	/* basic update logic */
	void update(float timeDelta = 0.333f);
	/* drawing the entity */
	void draw(sf::RenderWindow* window);

	/* get current position */
	sf::Vector2f getPosition() const;
	/* set current position */
	void setPosition(sf::Vector2f pos);

	/* start the pathfinding search */
	void updatePathfinding(Level& level, sf::Vector2f targetPosition);
	/* show the pathfinding in real time positions */
	void updateByRealTime(Level& level, float dt);		// level is just used for colors
	/* show the pathfinding in step time positions */
	void updateByStepTime(Level& level);

	void turnOnAllColors();
	void turnOffAllColors();
	void toggleTilesNotCheckedColors();
private:
	std::vector<sf::Vector2f> m_targetPositionsRealTime;			// for storing the path in real-time positions
	std::vector<sf::Vector2f> m_targetPositionsStepTime;			// for storing the path in step-time positions (one tile at a time)

	float m_speed;						// basic movement speed for real-time
	sf::RectangleShape m_sprite;		// obviously can be a sprite
	bool m_allowDiagonals;				// allows for 8 directions during pathfinding
	Tile* m_startNode;					// the node to start the pathfinding
	Tile* m_goalNode;						// the node to end the pathfinding

	sf::Vector2f m_velocity;			// velocity for real-time pathfinding
	sf::Vector2f m_newPosition;			// continue from last position (for real-time pathfinding)

	/* too lazy for getters/setters */
public:
	/* variables to show colors */
	bool showStartColor;
	bool showGoalColor;
	bool showOpenListColor;
	bool showCurrentPathColor;
	bool showTilesNotCheckedColor;
	bool showPathColor;
	bool showFCostColor;
};

