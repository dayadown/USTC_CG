#pragma once
#include <QWidget>
#include <vector>
#include <Eigen/Core>
#include<Eigen/Sparse>
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#include "ChildWindow.h"
class ChildWindow;
QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

typedef Eigen::SparseMatrix<double> SparseMatrixType;
typedef Eigen::Triplet<double> T;
typedef Eigen::SimplicialCholesky<SparseMatrixType> Solve;

enum DrawStatus
{
	kChoose, 
	kPaste, 
	kNone
};

struct in_image {
	QPoint start;
	int with;
	int height;
	//std::vector<QPoint> B_points;
	in_image(QPoint a,int b,int c):
		start(a),with(b),height(c){}
	std::vector<QRgb> up_bound;
	std::vector<QRgb> down_bound;
	std::vector<QRgb> left_bound;
	std::vector<QRgb> right_bound;
};

class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget(ChildWindow *relatewindow);
	~ImageWidget(void);

	int ImageWidth();											// Width of image
	int ImageHeight();											// Height of image
	void set_draw_status_to_choose();
	void set_draw_status_to_paste();
	QImage* image();
	void set_source_window(ChildWindow* childwindow);

protected:
	void paintEvent(QPaintEvent *paintevent);
	void mousePressEvent(QMouseEvent *mouseevent);
	void mouseMoveEvent(QMouseEvent *mouseevent);
	void mouseReleaseEvent(QMouseEvent *mouseevent);

public slots:
	// File IO
	void Open(QString filename);								// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();												// Save image to another file

	// Image processing
	void Invert();												// Invert pixel value in image
	void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
	void TurnGray();											// Turn image to gray-scale map
	void Restore();												// Restore image to origin

public:
	QPoint						point_start_;					// Left top point of rectangle region
	QPoint						point_end_;						// Right bottom point of rectangle region

private:
	QImage						*image_;						// image 
	QImage						*image_backup_;

	// Pointer of child window
	ChildWindow					*source_window_;				// Source child window

	// Signs
	DrawStatus					draw_status_;					// Enum type of draw status
	bool						is_choosing_;
	bool						is_pasting_;


	//poisson
private:
	std::vector<in_image> in_images;
public:
	void Seamless_cloning();
	Eigen::MatrixXd get_san(std::vector<in_image>::iterator, int);
	int get_index(int, int, int, int);
};

