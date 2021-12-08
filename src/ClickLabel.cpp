/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-12-05 16:45:13
 * @Description: 
 */

#include "ClickLabel.h"

void ClickLabel::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}

void ClickLabel::enterEvent(QEvent* event)
{
    this->setCursor(QCursor(Qt::PointingHandCursor));
}

void ClickLabel::leaveEvent(QEvent* event)
{
    this->setCursor(QCursor(Qt::ArrowCursor));
}