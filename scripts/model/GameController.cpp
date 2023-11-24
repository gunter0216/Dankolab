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
		_colors = std::vector<cocos2d::Color3B>(countColors);
		for (int i = 0; i < countColors; ++i) {
			const int r = std::rand() % 256;
			const int g = std::rand() % 256;
			const int b = std::rand() % 256;
			_colors[i] = cocos2d::Color3B(r, g, b);
		}
	}

	_map = std::vector<std::vector<Cell>>(_rows);
	for (int i = 0; i < _rows; ++i) {
		_map[i] = std::vector<Cell>(_cols);
		for (int j = 0; j < _cols; ++j) {
			const int colorId = std::rand() % countColors;
			_map[i][j] = Cell{ colorId };
		}
	}
}

void GameController::getCellsWithSameColors(std::unordered_set<Vec2i>& cells, const Vec2i &cellPos) {
	const std::array<Vec2i, 4> sides = {
		Vec2i{ -1,  0 },
		Vec2i{  1,  0 },
		Vec2i{  0,  1 },
		Vec2i{  0, -1 }
	};
	for (auto side : sides) {
		Vec2i pos{ side.x + cellPos.x, side.y + cellPos.y };
		if (pos.x < 0 || pos.x >= _cols ||
				pos.y < 0 || pos.y >= _rows ||
				_map[pos.y][pos.x].node == nullptr) {
			continue;
		}
		if (_map[pos.y][pos.x].colorId == _map[cellPos.y][cellPos.x].colorId &&
				(cells.find(pos) == cells.end())) {
			cells.insert(pos);
			getCellsWithSameColors(cells, pos);
		}
	}
}

void GameController::deleteCell(int x, int y, float cellSize) {
	if (_map[y][x].node == nullptr) {
		return;
	}
	std::unordered_set<Vec2i> cellsPos;
	cellsPos.insert(Vec2i{ x, y });
	getCellsWithSameColors(cellsPos, Vec2i{ x, y });
	if (cellsPos.size() < 3) {
		return;
	}
	// delete cells with animation
	_playerScore += cellsPos.size();
	for (auto cellPos : cellsPos) {
		Cell &cell = _map[cellPos.y][cellPos.x];
		auto scale = ScaleBy::create(TIME_SCALE_CELL, SCALE_CELL);
		auto remove = RemoveSelf::create(true);
		auto seq = Sequence::create(scale, remove, nullptr);
		cell.node->runAction(seq);
		cell.node->setZOrder(2);
		cell.node = nullptr;
	}
	// fall cells with animation
	for (int i = 0; i < _cols; ++i) {
		int countEmptyCells = 0;
		for (int j = 0; j < _rows; ++j) {
			if (_map[j][i].node == nullptr) {
				countEmptyCells += 1;
			} else if (countEmptyCells > 0) {
				MoveBy* action = MoveBy::create(TIME_FALL_CELL, Vec2(0, -cellSize * countEmptyCells));
				_map[j][i].node->runAction(action);
				_map[j - countEmptyCells][i] = _map[j][i];
				_map[j][i].node = nullptr;
			}
		}
	}
	SimpleAudioEngine::getInstance()->playEffect("audio/plamb.mp3");
	checkGameFinish();
}

void GameController::checkGameFinish() {
	const std::array<Vec2i, 4> sides = {
		Vec2i{ -1,  0 },
		Vec2i{  1,  0 },
		Vec2i{  0,  1 },
		Vec2i{  0, -1 }
	};
	for (int y = 0; y < _rows; ++y) {
		for (int x = 0; x < _cols; ++x) {
			if (_map[y][x].node == nullptr) {
				continue;
			}
			int count = 0;
			for (auto side : sides) {
				Vec2i pos{ side.x + x, side.y + y };
				if (pos.x < 0 || pos.x >= _cols ||
						pos.y < 0 || pos.y >= _rows ||
						_map[pos.y][pos.x].node == nullptr) {
					continue;
				}
				if (_map[pos.y][pos.x].colorId == _map[y][x].colorId) {
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
	_map[y][x] = cell;
}

Cell GameController::getCell(int x, int y) {
	return _map[y][x];
}

Color3B GameController::colorAt(int index) {
	if (_randomGenerateColors) {
		return _colors[index];
	} else {
		return _defaultColors[index];
	}
}
