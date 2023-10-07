#pragma once
#include"ImageWarping.h"
class IDW_Warping :public my_Warping {

public:
	IDW_Warping();
	IDW_Warping(QImage*);
	void startwarp() override;
};