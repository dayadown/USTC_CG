#include"RBF.h"
#include"ANN/ANN.h"

RBF_Warping::RBF_Warping() {

}

RBF_Warping::RBF_Warping(QImage* image) {
	_image = image;
}

void RBF_Warping::get_r() {
	for (int i = 0; i < Lines.size(); i++) {
		double min_ = INT_MAX;
		for (int j = 0; j < Lines.size(); j++) {
			if (i != j) {
				min_ = dist(Lines[i].start, Lines[j].start) < min_ ? dist(Lines[i].start, Lines[j].start) : min_;
			}
		}
		r.push_back(min_);
	}
}

void RBF_Warping::get_a() {
	///*
	int n = Lines.size();
	MatrixXd A(2, n), bj(2, n);
	MatrixXd C;
	C.resize(n, n);
	A.setZero(), C.setZero(), bj.setZero();
	for (int j = 0; j < n; j++) {
		for (int i = 0; i < n; i++) {
			C(i, j) = pow(pow(dist(Lines[i].start, Lines[j].start), 2) + r[i] * r[i], 0.5);
		}
		bj(0, j) = (double)Lines[j].end.x() - (double)Lines[j].start.x();
		bj(1, j) = (double)Lines[j].end.y() - (double)Lines[j].start.y();
	}
	A = bj * C.inverse();
	//std::cout << A << std::endl;
	for (int i = 0; i < n ; i++) {
		a.push_back(Vector2d(A(0,i), A(1,i)));
	}
}

QPoint RBF_Warping::point_trans(QPoint p) {
	Vector2d temp;
	//temp.setZero();
	temp << p.x(), p.y();
	for (int i = 0; i < Lines.size(); i++) {
		double R = pow(pow(dist(p, Lines[i].start), 2), 0.5);
		temp += R * a[i];
	}
	return QPoint(temp.x(), temp.y());
	
}

void RBF_Warping::startwarp() {
	get_r();
	get_a();
	is_hole.resize(_image->width(), _image->height());
	is_hole.setZero();
	QImage copy_image(*_image);
	for (int i = 0; i < _image->width(); i++) {
		for (int j = 0; j < _image->height(); j++) {
			_image->setPixel(i, j, qRgb(255, 255, 255));
		}
	}
	for (int i = 0; i < copy_image.width(); i++) {
		for (int j = 0; j < copy_image.height(); j++) {
			QPoint a = point_trans(QPoint(i, j));
			if (a.x() >= 0 && a.x() < copy_image.width() && a.y() >= 0 && a.y() < copy_image.height()) {
				_image->setPixel(a, copy_image.pixel(QPoint(i, j)));
				not_holes.push_back(a);
				is_hole(a.x(), a.y()) = 1;
			}
		}
	}
	for (int i = 0; i < _image->width(); i++) {
		for (int j = 0; j < _image->height(); j++) {
			if (is_hole(i, j) == 0) holes.push_back(QPoint(i, j));
		}
	}
	fillhole();
}

void RBF_Warping::fillhole() {
	ANNpointArray search_points = annAllocPts(not_holes.size(), 2);
	for (int i = 0; i < not_holes.size(); i++) {
		search_points[i][0] = not_holes[i].x();
		search_points[i][1] = not_holes[i].y();
	}
	ANNkd_tree* kdtree = new ANNkd_tree(search_points, not_holes.size(), 2);
	for (int i = 0; i < holes.size(); i++) {
		ANNpoint p = annAllocPt(2);
		p[0] = holes[i].x();
		p[1] = holes[i].y();
		ANNidxArray idx = new ANNidx[6];
		ANNdistArray dists = new ANNdist[6];
		kdtree->annkSearch(p, 6, idx, dists, 0.);
		double sum = 0;
		for (int j = 0; j < 6; j++)
			sum += dists[j];
		int hole_rgb[3] = { 0 };
		for (int k = 0; k < 6; k++) {
			int x = search_points[idx[k]][0];
			int y = search_points[idx[k]][1];
			QRgb rgb = _image->pixel(x, y);
			double w = dists[k] / sum;
			hole_rgb[0] += w * qRed(rgb);
			hole_rgb[1] += w * qGreen(rgb);
			hole_rgb[2] += w * qBlue(rgb);
		}
		_image->setPixel(holes[i], qRgb(hole_rgb[0], hole_rgb[1], hole_rgb[2]));
		delete[] idx;
		delete[] dists;
		for (int i = 0; i < 3; i++) {
			hole_rgb[i] = 0;
		}
	}
}