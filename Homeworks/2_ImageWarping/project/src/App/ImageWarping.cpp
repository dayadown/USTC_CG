#include"ImageWarping.h"
my_Warping::my_Warping(QImage* image) :
	_image(image) {
}

my_Warping::my_Warping() :
	_image(NULL) {
}

double my_Warping::dist(QPoint A, QPoint B) {
	double a = pow(A.x() - B.x(), 2) + pow(A.y() - B.y(), 2);
	return sqrt(a);
}

