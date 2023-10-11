#pragma once
#include"ImageWarping.h"
using namespace Eigen;
class IDW_Warping :public my_Warping {

public:
	IDW_Warping();
	IDW_Warping(QImage*);
	void startwarp() override;
	void Get_Ts();

	std::vector<QPoint> not_holes;
	std::vector<QPoint> holes;
	MatrixXd is_hole;
	void fillhole();

	double Get_weight(QPoint,QPoint);
	QPoint point_transform(QPoint);
	std::vector<Matrix2d> Ts;
};