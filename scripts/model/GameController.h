#pragma once

#include "cocos2d.h"
#include <iostream>
#include <array>
#include <vector>
#include <unordered_set>

#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;

USING_NS_CC;

struct Vec2i {
	int x;
	int y;

	bool operator==(const Vec2i& other) const {
		return x == other.x && y == other.y;
	}
};

template<>
struct std::hash<Vec2i>
{
	std::size_t operator()(const Vec2i& v) const noexcept
	{
		std::size_t h1 = std::hash<int>{}(v.x);
		std::size_t h2 = std::hash<int>{}(v.y);
		return h1 ^ (h2 << 1);
	}
};

struct Cell {
	int colorId;
	Vec2i pos;
	MenuItemSprite* node;
};

class GameController
{
public:
	GameController();

	void generateGrid(int cols, int rows, int countColors, bool randomGenerateColors);
	void setCell(int x, int y, const Cell& cell);
	Cell getCell(int x, int y);
	Color3B colorAt(int index);
	void deleteCell(int x, int y, float cellSize);
	int getPlayerScore() {
		return _playerScore;
	};
	bool gameIsFinish() {
		return _gameIsFinish;
	}
	void checkGameFinish();

private:
	std::vector<std::vector<Cell>> _map;
	std::vector<Color3B> _colors;
	std::vector<Color3B> _defaultColors {
		Color3B(255, 0, 0),
		Color3B(0, 255, 0),
		Color3B(0, 0, 255),
		Color3B(255, 255, 0),
		Color3B(0, 255, 255),
		Color3B(255, 0, 255),
		Color3B(128, 255, 255),
		Color3B(255, 128, 255),
		Color3B(255, 255, 128)
	};

	int _cols;
	int _rows;
	bool _randomGenerateColors;

	const float TIME_FALL_CELL = 0.5f;
	const float TIME_SCALE_CELL = 0.25f;
	const float SCALE_CELL = 1.5f;

	int _playerScore = 0;
	// true if game is finish
	bool _gameIsFinish = false;

	void getCellsWithSameColors(std::unordered_set<Vec2i>& cells, const Vec2i& cellPos);
};
