#pragma once
#include "shape.h"
class Freehand :public Shape {
public:
	Freehand() {};
	Freehand(QPoint start_, QPoint end_);
	~Freehand() {};

	void Draw(QPainter& painter);
};