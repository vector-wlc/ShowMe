/*
 * @Coding: gbk
 * @Author: vector-wlc
 * @Date: 2021-08-15 10:38:36
 * @Description: 
 */
#pragma once
#pragma execution_character_set("utf-8")
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
