#pragma once
#include <vector>


struct Position {
	Position(float x_, float y_) {
		x = x_; y = y_;
	}
	float x;
	float y;
};

typedef std::vector<Position> PositionList;