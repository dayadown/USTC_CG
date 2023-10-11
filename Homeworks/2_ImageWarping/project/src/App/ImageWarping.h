#pragma once
#include <Eigen/Dense>
#include<QWidget>
#include<qimage.h>
#include<vector>
class QImage;
struct Line
{
	QPoint start;
	QPoint end;
};
class my_Warping {

public:
	virtual void startwarp() = 0;
	my_Warping();
	my_Warping(QImage*);
	double dist(QPoint, QPoint);


	QImage* _image;
	std::vector<Line> Lines;

};