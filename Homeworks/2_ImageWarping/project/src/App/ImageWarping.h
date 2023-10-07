#pragma once
#include <Eigen/Dense>
#include<QWidget>
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


	QImage* _image;
	std::vector<Line> Lines;

};