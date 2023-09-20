#include "viewwidget.h"
#include<iostream>
ViewWidget::ViewWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	draw_status_ = false;
	shape_ = NULL;
	type_ = Shape::kDefault;
}

ViewWidget::~ViewWidget()
{
}

void ViewWidget::setLine()
{
	type_ = Shape::kLine;
}

void ViewWidget::setRect()
{
	type_ = Shape::kRect;
}

void ViewWidget::setPolygon()
{
	type_ = Shape::kPolygon;
}

void ViewWidget::setEllipse()
{
	type_ = Shape::kEllipse;
}
void ViewWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		setMouseTracking(false);
		if (Qt::LeftButton == event->button())
		{
			switch (type_)
			{
			case Shape::kLine:
				shape_ = new Line();
				break;
			case Shape::kDefault:
				break;

			case Shape::kRect:
				shape_ = new Rect();
				break;

			case Shape::kEllipse:
				shape_ = new mEllipse();
				break;

			case Shape::kPolygon:
				shape_ = new mPolygon();
				setMouseTracking(true);
				break;
			}
			if (shape_ != NULL)
			{
				draw_status_ = true;
				if (!get_start_point) {
					start_point = event->pos();
					get_start_point = true;
				}
				start_point_ = end_point_ = event->pos();
				shape_->set_start(start_point_);
				shape_->set_end(end_point_);
			}
		}
		update();
	}
	else {
		if (type_ == Shape::kPolygon) {
			shape_->set_end(start_point);
			draw_status_ = false;
			shape_list_.push_back(shape_);
			get_start_point = false;
			shape_ = NULL;
		}
	}
}

void ViewWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (draw_status_ && shape_ != NULL)
	{
		end_point_ = event->pos();
		shape_->set_end(end_point_);
	}
}

void ViewWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (shape_ != NULL)
	{
		if (type_ != Shape::kPolygon) {
			draw_status_ = false;
			shape_list_.push_back(shape_);
			shape_ = NULL;
		}
		else {
			shape_list_.push_back(shape_);
		}
	}
}

void ViewWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	for (int i = 0; i < shape_list_.size(); i++)
	{
		shape_list_[i]->Draw(painter);
	}

	if (shape_ != NULL) {
		shape_->Draw(painter);
	}

	update();
}