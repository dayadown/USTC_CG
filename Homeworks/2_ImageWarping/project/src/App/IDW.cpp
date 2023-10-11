#include"IDW.h"
#include<iostream>
#include"ANN/ANN.h"
IDW_Warping::IDW_Warping() {

}

IDW_Warping::IDW_Warping(QImage* image) {
	_image = image;
}


void IDW_Warping::Get_Ts() {
	for (auto i = Lines.begin(); i != Lines.end(); i++) {
		Matrix2d A, B;
		A.setZero();
		B.setZero();
		Vector2d pi, qi, pj, qj, a, b;
		pi << (double)i->start.x(), (double)i->start.y();
		qi << (double)i->end.x(), (double)i->end.y();
		for (auto j = Lines.begin(); j != Lines.end(); j++) {
			if (j != i) {
				double wc = pow(dist(i->start, j->start), -2);
				pj << (double)j->start.x(), (double)j->start.y();
				qj << (double)j->end.x(), (double)j->end.y();
				a = qi - qj;
				b = pj - pi;
				A += wc * b * b.transpose();
				B += -wc * a * b.transpose();
			}
		}
		Ts.push_back(A.inverse() * B);
	}
}

QPoint IDW_Warping::point_transform(QPoint p) {
	Vector2d transed;
	transed.setZero();
	Vector2d pi, qi, trans_p;
	trans_p << (double)p.x(), (double)p.y();
	for (int i = 0; i < Lines.size(); i++) {
		pi << (double)Lines[i].start.x() , (double)Lines[i].start.y();
		qi << (double)Lines[i].end.x(), (double)Lines[i].end.y();
		double weight = Get_weight(p, Lines[i].start);
		transed += weight * (qi + Ts[i] * (trans_p - pi));
	}
	return QPoint(transed.x(),transed.y());
}

double IDW_Warping::Get_weight(QPoint p,QPoint i) {
	double sum = 0;
	for (auto i = Lines.begin(); i != Lines.end(); i++) {
		sum += pow(dist(i->start, p), -2);
	}
	return pow(dist(p, i), -2) / sum;
}

void IDW_Warping::startwarp() { 
	Get_Ts();
	is_hole.resize(_image->width(), _image->height());
	is_hole.setZero();
	QImage copy_image(*_image);
	for (int i = 0; i < _image->width(); i++){
		for (int j = 0; j < _image->height(); j++){
			_image->setPixel(i, j, qRgb(255, 255, 255));
		}
	}
	for (int i = 0; i < copy_image.width(); i++){
		for (int j = 0; j < copy_image.height(); j++){
			QPoint a = point_transform(QPoint(i, j));
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

void IDW_Warping::fillhole() {
	ANNpointArray search_points = annAllocPts(not_holes.size(), 2);
	for (int i = 0; i < not_holes.size(); i++) {
		search_points[i][0] = not_holes[i].x();
		search_points[i][1] = not_holes[i].y();
	}
	ANNkd_tree* kdtree = new ANNkd_tree(search_points, not_holes.size(), 2);
	for (int i = 0; i < holes.size(); i++) {
		ANNpoint p= annAllocPt(2);
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