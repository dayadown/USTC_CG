#include "ImageWidget.h"

using std::cout;
using std::endl;


ImageWidget::ImageWidget(ChildWindow* relatewindow)
{
	image_ = new QImage();
	image_backup_ = new QImage();

	draw_status_ = kNone;
	is_choosing_ = false;
	is_pasting_ = false;

	point_start_ = QPoint(0, 0);
	point_end_ = QPoint(0, 0);

	source_window_ = NULL;
}

ImageWidget::~ImageWidget(void)
{
}

int ImageWidget::ImageWidth()
{
	return image_->width();
}

int ImageWidget::ImageHeight()
{
	return image_->height();
}

void ImageWidget::set_draw_status_to_choose()
{
	draw_status_ = kChoose;	
}

void ImageWidget::set_draw_status_to_paste()
{
	draw_status_ = kPaste;
}

QImage* ImageWidget::image()
{
	return image_;
}

void ImageWidget::set_source_window(ChildWindow* childwindow)
{
	source_window_ = childwindow;
}

void ImageWidget::paintEvent(QPaintEvent* paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	QRect rect = QRect(0, 0, image_->width(), image_->height());
	painter.drawImage(rect, *image_);

	// Draw choose region
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::red);
	painter.drawRect(point_start_.x(), point_start_.y(),
		point_end_.x() - point_start_.x(), point_end_.y() - point_start_.y());

	painter.end();
}

void ImageWidget::mousePressEvent(QMouseEvent* mouseevent)
{
	if (Qt::LeftButton == mouseevent->button())
	{
		switch (draw_status_)
		{
		case kChoose:
			is_choosing_ = true;
			point_start_ = point_end_ = mouseevent->pos();
			break;

		case kPaste:
		{
			is_pasting_ = true;

			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			// Start point in source image
			int xsourcepos = source_window_->imagewidget_->point_start_.rx();
			int ysourcepos = source_window_->imagewidget_->point_start_.ry();

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->point_end_.rx()
				- source_window_->imagewidget_->point_start_.rx() + 1;
			int h = source_window_->imagewidget_->point_end_.ry()
				- source_window_->imagewidget_->point_start_.ry() + 1;

			// Paste
			if ((xpos + w < image_->width()) && (ypos + h < image_->height()))
			{
				// Restore image
			//	*(image_) = *(image_backup_);

				// Paste
				for (int i = 0; i < w; i++)
				{
					for (int j = 0; j < h; j++)
					{
						image_->setPixel(xpos + i, ypos + j, source_window_->imagewidget_->image()->pixel(xsourcepos + i, ysourcepos + j));
					}
				}
			}
		}

		update();
		break;

		default:
			break;
		}
	}
	else {
		Seamless_cloning();
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case kChoose:
		// Store point position for rectangle region
		if (is_choosing_)
		{
			point_end_ = mouseevent->pos();
		}
		break;

	case kPaste:
		// Paste rectangle region to object image
		if (is_pasting_)
		{
			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();
			//std::cout << xpos << " " << ypos << endl;

			// Start point in source image
			int xsourcepos = source_window_->imagewidget_->point_start_.rx();
			int ysourcepos = source_window_->imagewidget_->point_start_.ry();

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->point_end_.rx()
				- source_window_->imagewidget_->point_start_.rx() + 1;
			int h = source_window_->imagewidget_->point_end_.ry()
				- source_window_->imagewidget_->point_start_.ry() + 1;

			// Paste
			if ((xpos > 0) && (ypos > 0) && (xpos + w < image_->width()) && (ypos + h < image_->height()))
			{
				// Restore image 
				*(image_) = *(image_backup_);

				// Paste
				for (int i = 0; i < w; i++)
				{
					for (int j = 0; j < h; j++)
					{
						image_->setPixel(xpos + i, ypos + j, source_window_->imagewidget_->image()->pixel(xsourcepos + i, ysourcepos + j));
					}
				}
			}
		}

	default:
		break;
	}

	update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case kChoose:
		if (is_choosing_)
		{
			point_end_ = mouseevent->pos();
			is_choosing_ = false;
			draw_status_ = kNone;
		}

	case kPaste:
		if (is_pasting_)
		{
			is_pasting_ = false;
			draw_status_ = kNone;
			int w = source_window_->imagewidget_->point_end_.rx()
				- source_window_->imagewidget_->point_start_.rx() + 1;
			int h = source_window_->imagewidget_->point_end_.ry()
				- source_window_->imagewidget_->point_start_.ry() + 1;
			struct in_image new_in_image(mouseevent->pos(), w, h);
			int left_x = source_window_->imagewidget_->point_start_.rx() - 1;
			int right_x = source_window_->imagewidget_->point_end_.rx() + 1;
			for (int i = 0; i < h; i++) {
				int y = source_window_->imagewidget_->point_start_.y();
				y = y + i;
				new_in_image.left_bound.push_back(source_window_->imagewidget_->image_->pixel(left_x, y));
				new_in_image.right_bound.push_back(source_window_->imagewidget_->image_->pixel(right_x, y));
			}
			int up_y = source_window_->imagewidget_->point_start_.ry() - 1;
			int down_y = source_window_->imagewidget_->point_end_.ry() + 1;
			for (int i = 0; i < w; i++) {
				int x = source_window_->imagewidget_->point_start_.x();
				x = x + i;
				new_in_image.up_bound.push_back(source_window_->imagewidget_->image_->pixel(x, up_y));
				new_in_image.down_bound.push_back(source_window_->imagewidget_->image_->pixel(x, down_y));
			}
			in_images.push_back(new_in_image);
		}

	default:
		break;
	}

	update();
}

void ImageWidget::Open(QString filename)
{
	// Load file
	if (!filename.isEmpty())
	{
		image_->load(filename);
		*(image_backup_) = *(image_);
	}

	//	setFixedSize(image_->width(), image_->height());
	//	relate_window_->setWindowFlags(Qt::Dialog);
	//	relate_window_->setFixedSize(QSize(image_->width(), image_->height()));
	//	relate_window_->setWindowFlags(Qt::SubWindow);

		//image_->invertPixels(QImage::InvertRgb);
		//*(image_) = image_->mirrored(true, true);
		//*(image_) = image_->rgbSwapped();
	cout << "image size: " << image_->width() << ' ' << image_->height() << endl;
	update();
}

void ImageWidget::Save()
{
	SaveAs();
}

void ImageWidget::SaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}

	image_->save(filename);
}

void ImageWidget::Invert()
{
	for (int i = 0; i < image_->width(); i++)
	{
		for (int j = 0; j < image_->height(); j++)
		{
			QRgb color = image_->pixel(i, j);
			image_->setPixel(i, j, qRgb(255 - qRed(color), 255 - qGreen(color), 255 - qBlue(color)));
		}
	}

	// equivalent member function of class QImage
	// image_->invertPixels(QImage::InvertRgb);
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	QImage image_tmp(*(image_));
	int width = image_->width();
	int height = image_->height();

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, height - 1 - j));
				}
			}
		}
		else
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_->setPixel(i, j, image_tmp.pixel(i, height - 1 - j));
				}
			}
		}

	}
	else
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, j));
				}
			}
		}
	}

	// equivalent member function of class QImage
	//*(image_) = image_->mirrored(true, true);
	update();
}

void ImageWidget::TurnGray()
{
	for (int i = 0; i < image_->width(); i++)
	{
		for (int j = 0; j < image_->height(); j++)
		{
			QRgb color = image_->pixel(i, j);
			int gray_value = (qRed(color) + qGreen(color) + qBlue(color)) / 3;
			image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value));
		}
	}

	update();
}

void ImageWidget::Restore()
{
	*(image_) = *(image_backup_);
	point_start_ = point_end_ = QPoint(0, 0);
	update();
}


void ImageWidget::Seamless_cloning() {
	for (auto it = in_images.begin(); it != in_images.end(); it++) {
		int w = it->with;
		int h = it->height;
		Eigen::VectorXd sourse_san_r = get_san(it, 0);
		Eigen::VectorXd sourse_san_g = get_san(it, 1);
		Eigen::VectorXd sourse_san_b = get_san(it, 2);
		int n = w * h;
		std::vector<T> tripletlist_r;
		std::vector<T> tripletlist_g;
		std::vector<T> tripletlist_b;
		SparseMatrixType sparse_r(n, n);
		SparseMatrixType sparse_b(n, n);
		SparseMatrixType sparse_g(n, n);
		int xx = it->start.x();
		int yy = it->start.y();
		int x, y;
		int up_boundary = yy;
		int down_boundary = yy + h-1;
		int left_boundary = xx;
		int right_boundary = xx + w - 1; 
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				int index = get_index(i, j, w, h);
				tripletlist_r.push_back(T(index, index, -4));
				tripletlist_g.push_back(T(index, index, -4));
				tripletlist_b.push_back(T(index, index, -4));
				x = xx + j;
				y = yy + i;
				QPoint points[4] = { QPoint(x ,y - 1),QPoint(x ,y + 1) ,QPoint(x - 1,y) ,QPoint(x + 1,y) };
				for (int k = 0; k < 4; k++) {
					if (points[k].x() < left_boundary || points[k].x() > right_boundary || points[k].y() < up_boundary || points[k].y() > down_boundary) {
						sourse_san_r(index) -= qRed(image_->pixel(points[k]));
						sourse_san_g(index) -= qGreen(image_->pixel(points[k]));
						sourse_san_b(index) -= qBlue(image_->pixel(points[k]));
					}
					else {
						int index_i = get_index(points[k].y() - yy, points[k].x()-xx, w, h);
						tripletlist_r.push_back(T(index, index_i, 1));
						tripletlist_g.push_back(T(index, index_i, 1));
						tripletlist_b.push_back(T(index, index_i, 1));
					}
				}
			}
		}

		Eigen::VectorXd result_r;
		Eigen::VectorXd result_g;
		Eigen::VectorXd result_b;
		sparse_r.setFromTriplets(tripletlist_r.begin(), tripletlist_r.end());
		sparse_r.makeCompressed();
		sparse_g.setFromTriplets(tripletlist_g.begin(), tripletlist_g.end());
		sparse_g.makeCompressed();
		sparse_b.setFromTriplets(tripletlist_b.begin(), tripletlist_b.end());
		sparse_b.makeCompressed();
		Solve* p_r = new Solve(sparse_r);
		Solve* p_g = new Solve(sparse_g);
		Solve* p_b = new Solve(sparse_b);
		result_r = p_r->solve(sourse_san_r);
		result_g = p_g->solve(sourse_san_g);
		result_b = p_b->solve(sourse_san_b);
		for (int i = 0; i < n; i++) {
			int now_y = i / w;
			int now_x = i % w;
			now_x += xx;
			now_y += yy;
			image_->setPixel(now_x, now_y, qRgb(result_r(i) > 255 ? 255 : (result_r(i) < 0 ? 0 : result_r(i)),
												result_g(i) > 255 ? 255 : (result_g(i) < 0 ? 0 : result_g(i)),
												result_b(i) > 255 ? 255 : (result_b(i) < 0 ? 0 : result_b(i))));
		}
	}
}

Eigen::MatrixXd ImageWidget::get_san(std::vector<in_image>::iterator it, int k) {
	int w = it->with;
	int h = it->height;
	Eigen::VectorXd san(h*w);
	int xx = it->start.x();
	int yy = it->start.y();
	int x, y;
	int up_boundary = yy;
	int down_boundary = yy + h - 1;
	int left_boundary = xx;
	int right_boundary = xx + w - 1;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			x = xx + j;
			y = yy + i;
			QPoint points[4] = { QPoint(x, y - 1),QPoint(x, y + 1) ,QPoint(x - 1, y) ,QPoint(x + 1, y) };
			int rgb[4][3];
			for (int z = 0; z < 4; z++) {
				if (points[z].x() < left_boundary || points[z].x() > right_boundary || points[z].y() < up_boundary || points[z].y() > down_boundary) {
					switch (z)
					{
					case 0:
						rgb[z][0] = qRed(it->up_bound[j]);
						rgb[z][1] = qGreen(it->up_bound[j]);
						rgb[z][2] = qBlue(it->up_bound[j]);
						break;
					case 1:
						rgb[z][0] = qRed(it->down_bound[j]);
						rgb[z][1] = qGreen(it->down_bound[j]);
						rgb[z][2] = qBlue(it->down_bound[j]);
						break;
					case 2:
						rgb[z][0] = qRed(it->left_bound[i]);
						rgb[z][1] = qGreen(it->left_bound[i]);
						rgb[z][2] = qBlue(it->left_bound[i]);
						break;
					case 3:
						rgb[z][0] = qRed(it->right_bound[i]);
						rgb[z][1] = qGreen(it->right_bound[i]);
						rgb[z][2] = qBlue(it->right_bound[i]);
						break;
					default:
						break;
					}
				}
				else {
					rgb[z][0] = qRed(image_->pixel(points[z]));
					rgb[z][1] = qGreen(image_->pixel(points[z]));
					rgb[z][2] = qBlue(image_->pixel(points[z]));
				}
			}
			int now_rgb[3] = { qRed(image_->pixel(QPoint(x,y))),qGreen(image_->pixel(QPoint(x,y))),qBlue(image_->pixel(QPoint(x,y))) };
			san(get_index(i,j, w, h)) = double(rgb[0][k])+ double(rgb[1][k]) + double(rgb[2][k]) + double(rgb[3][k])  - double(4 * now_rgb[k]);
		}
	}
	return san;
}

int ImageWidget::get_index(int x, int y,int w,int h) {
	return x * w + y;
}