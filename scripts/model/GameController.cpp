#include "GameController.h"

GameController::GameController() {
}

void GameController::generateGrid(int cols, int rows, int countColors, bool randomGenerateColors) {
	_playerScore = 0;
	_gameIsFinish = false;
	this->_cols = cols;
	this->_rows = rows;
	this->_randomGenerateColors = randomGenerateColors;

	if (randomGenerateColors) {
		colors = std::vector<cocos2d::Color3B>(countColors);
		for (int i = 0; i < countColors; ++i) {
			const int r = std::rand() % 256;
			const int g = std::rand() % 256;
			const int b = std::rand() % 256;
			colors[i] = cocos2d::Color3B(r, g, b);
		}
	}

	map = std::vector<std::vector<Cell>>(_rows);
	for (int i = 0; i < _rows; ++i) {
		map[i] = std::vector<Cell>(_cols);
		for (int j = 0; j < _cols; ++j) {
			const int colorId = std::rand() % countColors;
			map[i][j] = Cell{ colorId };
		}
	}
}

void GameController::deleteCell(int x, int y, float cellSize) {
	if (map[y][x].node == nullptr) {
		return;
	}
	const std::array<std::pair<int, int>, 5> sides = {
		std::pair<int, int>(-1, 0),
		std::pair<int, int>(1, 0),
		std::pair<int, int>(0, 1),
		std::pair<int, int>(0, -1),
		std::pair<int, int>(0, 0)
	};
	// get cells with same colors
	std::vector<std::pair<std::pair<int, int>, Cell*>> cells;
	for (auto side : sides) {
		std::pair<int, int> pos { side.first + x, side.second + y };
		if (pos.first < 0 || pos.first >= _cols ||
				pos.second < 0 || pos.second >= _rows ||
				map[pos.second][pos.first].node == nullptr) {
			continue;
		}
		if (map[pos.second][pos.first].colorId == map[y][x].colorId) {
			cells.push_back(
				{ std::pair<int, int>{ pos.first, pos.second }, &map[pos.second][pos.first] });
		}
	}
	if (cells.size() < 3) {
		return;
	}
	// delete cells with animation
	_playerScore += cells.size();
	bool hasBottomCell = false;
	bool hasTopCell = false;
	for (auto cell : cells) {
		auto scale = ScaleBy::create(TIME_SCALE_CELL, SCALE_CELL);
		auto remove = RemoveSelf::create(true);
		auto seq = Sequence::create(scale, remove, nullptr);
		cell.second->node->runAction(seq);
		cell.second->node->setZOrder(2);
		cell.second->node = nullptr;
		if (cell.first == std::pair<int, int>(x, y - 1)) {
			hasBottomCell = true;
		}
		if (cell.first == std::pair<int, int>(x, y + 1)) {
			hasTopCell = true;
		}
	}
	// fall top cells with animation
	for (auto cell : cells) {
		if ((hasBottomCell && cell.first.first == x && cell.first.second != y - 1) ||
				(!hasBottomCell && cell.first.first == x && cell.first.second != y)) {
			continue;
		}
		int col = cell.first.first;
		for (int row = cell.first.second + 1; row < _rows; ++row) {
			if (map[row][col].node != nullptr) {
				int lengthFall = 1;
				if (col == x) {
					lengthFall += (int)hasBottomCell + (int)hasTopCell;
				}
				MoveBy* action = MoveBy::create(TIME_FALL_CELL, Vec2(0, -cellSize * lengthFall));
				map[row][col].node->runAction(action);
				map[row - lengthFall][col] = map[row][col];
				map[row][col].node = nullptr;
			}
		}
	}
	SimpleAudioEngine::getInstance()->playEffect("audio/plamb.mp3");
	checkGameFinish();
}

void GameController::checkGameFinish() {
	const std::array<std::pair<int, int>, 4> sides = {
		std::pair<int, int>(-1, 0),
		std::pair<int, int>(1, 0),
		std::pair<int, int>(0, 1),
		std::pair<int, int>(0, -1),
	};
	for (int y = 0; y < _rows; ++y) {
		for (int x = 0; x < _cols; ++x) {
			if (map[y][x].node == nullptr) {
				continue;
			}
			int count = 0;
			for (auto side : sides) {
				std::pair<int, int> pos{ side.first + x, side.second + y };
				if (pos.first < 0 || pos.first >= _cols ||
						pos.second < 0 || pos.second >= _rows ||
						map[pos.second][pos.first].node == nullptr) {
					continue;
				}
				if (map[pos.second][pos.first].colorId == map[y][x].colorId) {
					++count;
				}
			}
			if (count > 1) {
				return;
			}
		}
	}
	_gameIsFinish = true;
}

void GameController::setCell(int x, int y, const Cell& cell) {
	map[y][x] = cell;
}

Cell GameController::getCell(int x, int y) {
	return map[y][x];
}

Color3B GameController::colorAt(int index) {
	if (_randomGenerateColors) {
		return colors[index];
	} else {
		return defaultColors[index];
	}
}
