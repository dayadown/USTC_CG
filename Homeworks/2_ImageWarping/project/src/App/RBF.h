#pragma once
#include"ImageWarping.h"
class RBF_Warping :public my_Warping {

public:
	RBF_Warping();
	RBF_Warping(QImage*);
	void startwarp() override;
};