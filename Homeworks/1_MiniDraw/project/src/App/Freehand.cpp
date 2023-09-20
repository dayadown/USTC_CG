#include "Freehand.h"

Freehand::Freehand(QPoint start_, QPoint end_) {
	start = start_;
	end = end_;
}


void Freehand::Draw(QPainter &painter) {
	painter.drawLine(start, end);
}