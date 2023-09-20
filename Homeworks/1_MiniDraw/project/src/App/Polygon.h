#pragma once
#include "shape.h"
class Polygon :public Shape {
public:
	Polygon() {};
	~Polygon() {};

	void Draw(QPainter& painter);
};