#include "Entity.h"
#include <vector>
#include "Level.h"
#include <iostream>

/* NOTE ALL THE BOOLS ARE STRICTLY FOR VISUALS */
Entity::Entity()
: m_speed{200}, m_allowDiagonals{false}
,showStartColor{true},showGoalColor{true},showCurrentPathColor{true}
,showPathColor{true},showFCostColor{true},showOpenListColor{true},showTilesNotCheckedColor{false}{
	m_sprite.setSize({32, 32});
	m_sprite.setFillColor(sf::Color::Green);
	m_startNode = nullptr;
	m_goalNode = nullptr;
}

sf::Vector2f Entity::getPosition() const {
	return m_sprite.getPosition();
}

void Entity::setPosition(sf::Vector2f pos) {
	m_sprite.setPosition(pos);
}

void Entity::handleInput() {
	// allows 8 directions of pathfinding, instead of 4
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
		m_allowDiagonals = !m_allowDiagonals;

		if (m_allowDiagonals)
			std::cout << "allowDiagonals Enabled\n";
		else
			std::cout << "allowDiagonals Disabled\n";
	}
}

void Entity::update(float timeDelta) {
	// no basic logic, could put a switch here for what type of pathfinding to run.
}

void Entity::draw(sf::RenderWindow* window) {
	window->draw(m_sprite);
}

/* recalculates the enemies path finding. Both styles of pathfinding use this function */
void Entity::updatePathfinding(Level& level, sf::Vector2f targetPosition)
{
	// this is not needed but nice for visuals
	level.resetTileColors();

	// create all variables.
	std::vector<Tile*> openList;
	std::vector<Tile*> closedList;
	std::vector<Tile*> pathList;
	std::vector<Tile*>::iterator position;
	Tile* currentNode;

	// reset all nodes. 
	level.resetNodes();

	// store the start and goal nodes.
	Tile* startNode = level.getTile(getPosition());
	Tile* goalNode = level.getTile(targetPosition);

	/* show color for the start and goal nodes (not needed) */
	if (showStartColor)
		startNode->sprite.setColor(sf::Color::Green);
	if (showGoalColor)
		goalNode->sprite.setColor(sf::Color::Red);

	// check we have a valid path to find. If not we can just end the function as there's no path to find.
	if (startNode == goalNode) {
		// Clear the vector of target positions.
		m_targetPositionsRealTime.clear();
		m_targetPositionsStepTime.clear();
		// Exit the function.
		return;
	}

	// pre-compute our H cost (estimated cost to goal) for each node.
	for (int i = 0; i < level.getTotalWidth(); i++) {
		for (int j = 0; j < level.getTotalHeight(); j++) {
			int rowOffset, heightOffset;
			Tile* node = level.getTile(i, j);
			if (node == nullptr)
				continue;
			heightOffset = abs(node->rowIndex - goalNode->rowIndex);
			rowOffset = abs(node->columnIndex - goalNode->columnIndex);

			node->H = heightOffset + rowOffset;

			/* not needed and color for visuals */
			if(showTilesNotCheckedColor)
				node->sprite.setColor(sf::Color::Blue);
		}
	}

	// add the start node to the open list.
	openList.push_back(startNode);

	// while we have values to check in the open list.
	while (!openList.empty()) {
		// find the node in the open list with the lowest F value and mark it as current.
		int lowestF = INT_MAX;

		for (Tile* tile : openList) {
			if (tile->F < lowestF) {
				lowestF = tile->F;
				currentNode = tile;

				if(showFCostColor)
					tile->sprite.setColor(sf::Color{128,128,128});
			}
		}

		// remove the current node from the open list and add it to the closed list.
		position = std::find(openList.begin(), openList.end(), currentNode);
		if (position != openList.end())
			openList.erase(position);

		closedList.push_back(currentNode);

		// find all valid adjacent nodes.
		std::vector<Tile*> adjacentTiles;
		Tile* node;

		// top.
		node = level.getTile(currentNode->columnIndex, currentNode->rowIndex - 1);
		if ((node != nullptr) && (level.isFloor(*node))) {
			adjacentTiles.push_back(level.getTile(currentNode->columnIndex, currentNode->rowIndex - 1));
		}

		// right.
		node = level.getTile(currentNode->columnIndex + 1, currentNode->rowIndex);
		if ((node != nullptr) && (level.isFloor(*node))) {
			adjacentTiles.push_back(level.getTile(currentNode->columnIndex + 1, currentNode->rowIndex));
		}

		// bottom.
		node = level.getTile(currentNode->columnIndex, currentNode->rowIndex + 1);
		if ((node != nullptr) && (level.isFloor(*node))) {
			adjacentTiles.push_back(level.getTile(currentNode->columnIndex, currentNode->rowIndex + 1));
		}

		// left.
		node = level.getTile(currentNode->columnIndex - 1, currentNode->rowIndex);
		if ((node != nullptr) && (level.isFloor(*node))) {
			adjacentTiles.push_back(level.getTile(currentNode->columnIndex - 1, currentNode->rowIndex));
		}

		if(m_allowDiagonals)
		{
			// north-west.
			node = level.getTile(currentNode->columnIndex-1, currentNode->rowIndex - 1);
			if ((node != nullptr) && (level.isFloor(*node))) {
				adjacentTiles.push_back(level.getTile(currentNode->columnIndex-1, currentNode->rowIndex - 1));
			}

			// north-east.
			node = level.getTile(currentNode->columnIndex + 1, currentNode->rowIndex-1);
			if ((node != nullptr) && (level.isFloor(*node))) {
				adjacentTiles.push_back(level.getTile(currentNode->columnIndex + 1, currentNode->rowIndex-1));
			}

			// south-west
			node = level.getTile(currentNode->columnIndex-1, currentNode->rowIndex + 1);
			if ((node != nullptr) && (level.isFloor(*node))) {
				adjacentTiles.push_back(level.getTile(currentNode->columnIndex-1, currentNode->rowIndex + 1));
			}

			// south-east
			node = level.getTile(currentNode->columnIndex + 1, currentNode->rowIndex + 1);
			if ((node != nullptr) && (level.isFloor(*node))) {
				adjacentTiles.push_back(level.getTile(currentNode->columnIndex + 1, currentNode->rowIndex + 1));
			}
		}

		// for all adjacent nodes.
		for (Tile* node : adjacentTiles) {
			// if the node is our goal node.
			if (node == goalNode) {
				// parent the goal node to current.
				node->parentNode = currentNode;
				// store the current path.
				while (node->parentNode != nullptr) {
					pathList.push_back(node);
					node = node->parentNode;
				}

				// empty the open list and break out of our for loop.
				openList.clear();
				break;
			}
			else {
				// if the node is not in the closed list.
				position = std::find(closedList.begin(), closedList.end(), node);
				if (position == closedList.end()) {
					// if the node is not in the open list.
					position = std::find(openList.begin(), openList.end(), node);
					if (position == openList.end()) {
						// add the node to the open list.
						openList.push_back(node);

						// set the parent of the node to the current node.
						node->parentNode = currentNode;

						// calculate G (total movement cost so far) cost.
						node->G = currentNode->G + 10;

						// calculate the F (total movement cost + heuristic) cost.
						node->F = node->G + node->H;


						/* open list color*/
						if(showOpenListColor)
							node->sprite.setColor(sf::Color::Cyan);
					}
					else {
						// check if this path is quicker that the other.
						int tempG = currentNode->G + 10;

						// check if tempG is faster than the other. I.e, whether it's faster to go A->C->B that A->C.
						if (tempG < node->G) {
							// re-parent node to this one.
							node->parentNode = currentNode;

							if(showPathColor)
								node->sprite.setColor(sf::Color{90,170,30});

						}
					}
				}
			}
		}
	}

	// clear the vector of target positions.
	m_targetPositionsRealTime.clear();
	m_targetPositionsStepTime.clear();

	// store the node locations as the enemies target locations.
	for (Tile* tile : pathList) {
		m_targetPositionsRealTime.push_back(level.getActualTileLocation(tile->columnIndex, tile->rowIndex));
		m_targetPositionsStepTime.push_back(level.getActualTileLocation(tile->columnIndex, tile->rowIndex));

		/* only needed to show visual colors*/
		if(showPathColor)
			tile->sprite.setColor(sf::Color{200,0,200});
	}

	// reverse the target position as we read them from goal to origin and we need them the other way around.
	std::reverse(m_targetPositionsRealTime.begin(), m_targetPositionsRealTime.end());
}

void Entity::updateByStepTime(Level& level) {
	
	// if the step-time positions vector isn't empty
	if(!m_targetPositionsStepTime.empty()) {
		// move towards current target location.
		sf::Vector2f pos = m_targetPositionsStepTime.back();
		// getting the tile so we can grab it's position to move by tiles
		Tile* tile = level.getTile(pos);
		auto tilePosition = tile->sprite.getPosition();
		setPosition(tilePosition);
		// remove the position, we don't need it
		m_targetPositionsStepTime.pop_back();

		if(showCurrentPathColor)
			tile->sprite.setColor(sf::Color::Yellow);
	}
}

void Entity::updateByRealTime(Level& level, float dt)
{
	// if the step-time positions vector isn't empty
	if (!m_targetPositionsRealTime.empty()) {
		sf::Vector2f targetLocation = m_targetPositionsRealTime.front();
		sf::Vector2f currentPosition = getPosition();

		/* this is not needed, but I like to see the colors */
		if(showCurrentPathColor)
		{
			auto tile = level.getTile(targetLocation);
			tile->sprite.setColor(sf::Color::Yellow);
		}
		/****************************************************/

		// move towards current target location.
		m_velocity = sf::Vector2f(targetLocation.x - currentPosition.x, targetLocation.y - currentPosition.y);

		if (abs(m_velocity.x) < 10.f && abs(m_velocity.y) < 10.f) {
			m_targetPositionsRealTime.erase(m_targetPositionsRealTime.begin());
		}
		else {
			float length = sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);
			m_velocity.x /= length;
			m_velocity.y /= length;

			m_newPosition.x += m_velocity.x * (m_speed * dt);
			m_newPosition.y += m_velocity.y * (m_speed * dt);

			m_sprite.setPosition(m_newPosition);
		}
	}
}

/* strictly for visuals */
void Entity::turnOnAllColors() {
	/* variables to show colors */
	showStartColor = true;
	showGoalColor = true;
	showOpenListColor = true;
	showCurrentPathColor = true;
	showPathColor = true;
	showFCostColor = true;
}

/* strictly for visuals */
void Entity::turnOffAllColors(){
/* variables to show colors */
 showStartColor = false;
 showGoalColor = false;
 showOpenListColor = false;
 showCurrentPathColor = false;
 showPathColor = false;
 showFCostColor = false;
 showTilesNotCheckedColor = false;
}

void Entity::toggleTilesNotCheckedColors() {
	showTilesNotCheckedColor = !showTilesNotCheckedColor;
}
