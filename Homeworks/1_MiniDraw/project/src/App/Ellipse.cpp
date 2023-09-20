#include "Ellipse.h"

mEllipse::mEllipse() {

}

mEllipse::~mEllipse() {

}

void mEllipse::Draw(QPainter& painter) {
	painter.drawEllipse(start.x(),start.y(),end.x()-start.x(), end.y() - start.y());
}