#pragma once
#include "shape.h"
class mPolygon :public Shape {
public:
	mPolygon() {};
	~mPolygon() {};

	void Draw(QPainter& painter);
};