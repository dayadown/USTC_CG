#pragma once

#include "shape.h"

class mEllipse :public Shape {
public:
	mEllipse();
	~mEllipse();
	
	void Draw(QPainter& painter);
};