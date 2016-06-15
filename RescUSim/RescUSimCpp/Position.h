#pragma once
#include <vector>


struct Position {
	Position(double x_, double y_) {
		x = x_; y = y_;
	}
	double x;
	double y;
};

typedef std::vector<Position> PositionList;