/*
 * @Coding: gbk
 * @Author: vector-wlc
 * @Date: 2021-08-15 10:38:36
 * @Description: 
 */
#pragma once

#include <qlabel.h>
#include <qlayout.h>
#include <qwidget.h>
class InfoWidget : public QWidget {
private:
    QLabel* label;

public:
    InfoWidget();

    void setText(const QStringList& text);
    void setFont(const QFont& font);
    void setColor(const QColor& color);
};
