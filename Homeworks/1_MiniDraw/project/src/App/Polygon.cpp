#include "Polygon.h"
void mPolygon::Draw(QPainter& painter) {
	painter.drawLine(start,end);
}