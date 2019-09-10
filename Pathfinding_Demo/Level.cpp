#define _CRT_SECURE_NO_WARNINGS

#include "Level.h"
#include <fstream>
#include <string>
#include <iostream>

void Level::resetNodes()
{
	for (int i = 0; i < GRID_WIDTH; i++) {
		for (int j = 0; j < GRID_HEIGHT; j++) {
			auto cell = &m_grid[i][j];
			cell->parentNode = nullptr;
			cell->H = 0;
			cell->G = 0;
			cell->F = 0;
		}
	}
}

bool Level::isFloor(int columnIndex, int rowIndex)
{
	Tile* tile = &m_grid[columnIndex][rowIndex];

	return ((tile->type == TILE::FLOOR));
}

bool Level::isFloor(const Tile& tile)
{
	return ((tile.type == TILE::FLOOR));
}

Level::Level()
{
	tileWidth = -1;
	tileHeight = -1;
	width = -1;
	height = -1;
	layer1 = layer2 = layer3 = solidLayer = NULL;

	// Store the column and row information for each node.
	for (int i = 0; i < GRID_WIDTH; i++) {
		for (int j = 0; j < GRID_HEIGHT; j++) {
			auto cell = &m_grid[i][j];
			cell->columnIndex = i;
			cell->rowIndex = j;
			cell->type = TILE::FLOOR;
		}
	}

}

Level::~Level()
{}


void Level::loadDimensions(std::ifstream* in) {
	std::string buffer;

	//Get the line that says 'width'
	getline(*in, buffer, ' ');
	//Get the width value
	getline(*in, buffer, '\n');
	width = atoi(buffer.c_str());

	//Get the line that says 'height'
	getline(*in, buffer, ' ');
	//Get the width value
	getline(*in, buffer, '\n');
	height = atoi(buffer.c_str());

	//Get the line that says 'tile_width'
	getline(*in, buffer, ' ');
	//Get the width value
	getline(*in, buffer, '\n');
	tileWidth = atoi(buffer.c_str());

	//Get the line that says 'tile_height'
	getline(*in, buffer, ' ');
	//Get the width value
	getline(*in, buffer, '\n');
	tileHeight = atoi(buffer.c_str());
}

void Level::loadSolidTiles(std::ifstream* in) {
	std::string buffer;

	//Get the line that says 'solid_tiles' and ignore it
	getline(*in, buffer, '\n');
	//Get the row containing solid tiles and ignore it
	getline(*in, buffer, '\n');

	char* string = new char[buffer.size() + 1];
	strcpy(string, buffer.c_str());

	char* token = strtok(string, " ,");

	while (token != NULL) {
		solidTiles.push_back(atoi(token));
		token = strtok(NULL, " ,");
	}

	delete[] string;

}

void Level::loadLayer(int* layer, std::ifstream* in) {
	std::string buffer;

	//Get the row containing 'layerX' and ignore it
	getline(*in, buffer, '\n');

	int i = 0;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			char delim = ',';

			if (x == width - 1)
				delim = '\n';

			getline(*in, buffer, delim);
			layer[i] = atoi(buffer.c_str());
			i++;
		}
	}

}

void Level::generateSolidLayer() {
	for (int i = 0; i < width * height; i++) {
		if (isTileSolid(layer1[i]) || isTileSolid(layer2[i]) || isTileSolid(layer3[i]))
			solidLayer[i] = 1;
		else
			solidLayer[i] = 0;
	}

}

void Level::draw(int layer, int xOffset, int yOffset, sf::RenderWindow* window) {
	int* drawLayer = NULL;

	if (layer == 0)
		drawLayer = solidLayer;
	else if (layer == 1)
		drawLayer = layer1;
	else if (layer == 2)
		drawLayer = layer2;
	else if (layer == 3)
		drawLayer = layer3;
	else
		return;

	int startX = xOffset / tileWidth;
	int startY = yOffset / tileHeight;

	int columns = (window->getSize().x / tileWidth) + 2;
	int rows = (window->getSize().y / tileHeight) + 2;

	for (int y = startY; y < startY + rows; y++)
		for (int x = startX; x < startX + columns; x++) {
			int frame = -1;

			if (x >= 0 && y >= 0 && x < width && y < height)
				frame = drawLayer[y * width + x] - 1;

			if (frame >= 0) {
				if (layer == 0) {
				
					//tiles.setPosition(x * (float)tileWidth - xOffset, y * (float)tileHeight - yOffset);
					//window->draw(tiles);
					//window->setView(view);
				}
				else {

					sheetTextureRect(x * tileWidth - xOffset, y * tileHeight - yOffset, frame, m_grid[x][y].sprite);
					m_grid[x][y].sprite.setPosition(x * frameWidth, y * frameHeight);
					window->draw(m_grid[x][y].sprite);
				}
			}
		}
}


void Level::sheetTextureRect(int x, int y, int frame,sf::Sprite& sprite)
{
	int columns = w / frameWidth;

	sf::IntRect rect{(frame % columns) * frameWidth,(frame / columns) * frameHeight,frameWidth,frameHeight};

	sprite.setTextureRect(rect);
}

void Level::resetTileColors() {
	for(int x = 0; x < GRID_WIDTH; x++) {
		for(int y =0; y< GRID_HEIGHT; y++) {
			auto tile = getTile(x, y);
			tile->sprite.setColor(sf::Color{255,255,255});
		}
	}
}

Tile* Level::getTile(sf::Vector2f position)
{
	// convert to a tile position.
	int tileColumn, tileRow;

	tileColumn = static_cast<int>(position.x) / TILE_SIZE;
	tileRow = static_cast<int>(position.y) / TILE_SIZE;

	return &m_grid[tileColumn][tileRow];
}

Tile* Level::getTile(int columnIndex, int rowIndex)
{
	if (tileIsValid(columnIndex, rowIndex)) {
		return &m_grid[columnIndex][rowIndex];
	}
	else {
		return nullptr;
	}
}

bool Level::tileIsValid(int column, int row)
{
	bool validColumn, validRow;

	validColumn = ((column >= 0) && (column < GRID_WIDTH));
	validRow = ((row >= 0) && (row < GRID_HEIGHT));

	return (validColumn && validRow);
}

sf::Vector2f Level::getActualTileLocation(int columnIndex, int rowIndex)
{
	sf::Vector2f location;

	location.x = static_cast<float>((columnIndex * TILE_SIZE) + (TILE_SIZE / 2));
	location.y = static_cast<float>((rowIndex * TILE_SIZE) + (TILE_SIZE / 2));

	return location;
}


bool Level::isTileSolid(int tile) {
	for (unsigned int i = 0; i < solidTiles.size(); i++) {
		if (solidTiles[i] == tile)
			return true;
	}

	return false;

}

bool Level::checkSolid(int x, int y) {
	if (x < 0 || x >= width || y < 0 || y >= height)
		return false;

	return solidLayer[y * width + x];

}


bool Level::load(const char* mapName, const char* imageName) {
	layer1 = layer2 = layer3 = solidLayer = NULL;

	std::ifstream in(mapName);

	if (!in.good())
		return false;

	std::string buffer;

	loadDimensions(&in);

	loadSolidTiles(&in);

	//Get the tile data
	layer1 = new int[width * height];
	layer2 = new int[width * height];
	layer3 = new int[width * height];
	solidLayer = new int[width * height];

	loadLayer(layer1, &in);
	loadLayer(layer2, &in);
	loadLayer(layer3, &in);
	generateSolidLayer();

	in.close();

	//Load Image
	if (!texture.loadFromFile(imageName)) {
		std::cout << "tiles.bmp failed to load\n";
	}


	// Draw the level tiles.
	for (int i = 0; i < GRID_WIDTH; i++) {
		for (int j = 0; j < GRID_HEIGHT; j++) {
			auto& cell = m_grid[i][j];

			cell.F = 0;
			cell.G = 0;
			cell.H = 0;

			cell.sprite.setTexture(texture);
		}
	}

	w = texture.getSize().x;
	h = texture.getSize().y;

	return true;

}
