#pragma once
#include <SFML/Graphics.hpp>

enum TILE{NONE,FLOOR,WALL,COUNT};

struct Tile
{
	TILE type;
	int columnIndex;					// The column index of the tile.
	int rowIndex;						// The row index of the tile.
	sf::Sprite sprite;					// The tile sprite.
	int H;								// Heuristic / movement cost to goal.
	int G;								// Movement cost. (Total of entire path)
	int F;								// Estimated cost for full path. (G + H)
	Tile* parentNode;					// Node to reach this node.
};


const int GRID_WIDTH = 30;
const int GRID_HEIGHT = 30;
const int TILE_SIZE = 32;

class Level {
private:
	sf::Texture texture;
	sf::Sprite tiles;
	int tileWidth;
	int tileHeight;
	int width;
	int height;
	int* layer1, * layer2, * layer3, * solidLayer;
	std::vector<int> solidTiles;
	int w, h;

	void loadDimensions(std::ifstream* in);
	void loadSolidTiles(std::ifstream* in);
	void loadLayer(int* layer, std::ifstream* in);
	void generateSolidLayer();

	const int frameWidth = 32;
	const int frameHeight = 32;

	/**
	 * A 2D array that describes the level data.
	 * The type is Tile, which holds a sprite and an index.
	 */
	Tile m_grid[GRID_WIDTH][GRID_HEIGHT];

public:
	Level();
	~Level();

	bool load(const char* mapName, const char* imageName);
	void draw(int layer, int xOffset, int yOffset, sf::RenderWindow* window);

	bool isTileSolid(int tile);
	bool checkSolid(int x, int y);

	int getWidth() { return width; };
	int getHeight() { return height; };
	int getTileWidth() { return tileWidth; };
	int getTileHeight() { return tileHeight; };
	int getTotalWidth() { return width * tileWidth; };
	int getTotalHeight() { return height * tileHeight; };

	void sheetTextureRect(int x, int y, int frame, sf::Sprite& sprite);

	void resetTileColors();

	// gets the tile that the position lies on.
	Tile* getTile(sf::Vector2f position);

	// returns a pointer to the tile at the given index.
	Tile* getTile(int columnIndex, int rowIndex);

	// checks if a given tile is valid.
	bool tileIsValid(int column, int row);

	// gets the actual location of a tile in the level.
	sf::Vector2f getActualTileLocation(int columnIndex, int rowIndex);

	// resets the A* data of all tiles.
	void resetNodes();

	// return true if the given tile is a floor tile.
	bool isFloor(int columnIndex, int rowIndex);

	// return true if the given tile is a floor tile.
	bool isFloor(const Tile& tile);
};

