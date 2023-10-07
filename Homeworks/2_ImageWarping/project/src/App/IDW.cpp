#include"IDW.h"
using namespace Eigen;
IDW_Warping::IDW_Warping() {

}

IDW_Warping::IDW_Warping(QImage* image) {
	_image = image;
}

void IDW_Warping::startwarp() { 
	std::vector<Matrix2f> Ts;
	for (auto i = Lines.begin(); i != Lines.end(); i++) {
		Matrix2f T;

	}
}