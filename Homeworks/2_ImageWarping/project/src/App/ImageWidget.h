#pragma once
#include <QWidget>
#include <qevent.h>
#include <qpainter.h>


#include "ImageWarping.h"
#include "IDW.h"
#include "RBF.h"

QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget(void);
	~ImageWidget(void);

	enum warping_type {
		IDW = 0,
		RBF = 1,
	};
	void settoIDW();
	void settoRBF();
	void warping_flag();
public:
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

protected:
	void paintEvent(QPaintEvent *paintevent);

public slots:
	// File IO
	void Open();												// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();												// Save image to another file

	// Image processing
	void Invert();												// Invert pixel value in image
	void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
	void TurnGray();											// Turn image to gray-scale map
	void Restore();												// Restore image to origin
	void Warping();

private:
	QImage		*ptr_image_;				// image 
	QImage		*ptr_image_backup_;
	my_Warping  *warp;
	warping_type     _wtype;
	bool draw;
	struct Line line;
	std::vector<struct Line> Lines;
};

