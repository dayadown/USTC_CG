#pragma once
#include"ImageWarping.h"
#include<iostream>
using namespace Eigen;
class RBF_Warping :public my_Warping {

public:
	RBF_Warping();
	RBF_Warping(QImage*);
	void startwarp() override;
	QPoint point_trans(QPoint p);
	std::vector<Vector2d> a;
	std::vector<double> r;
	void get_r();
	void get_a();

	std::vector<QPoint> not_holes;
	std::vector<QPoint> holes;
	MatrixXd is_hole;
	void fillhole();
};