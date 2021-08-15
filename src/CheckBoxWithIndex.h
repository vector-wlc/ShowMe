/*
 * @Coding: gbk
 * @Author: vector-wlc
 * @Date: 2021-08-15 10:38:36
 * @Description: 
 */
#pragma once
#include <qcheckbox.h>
class CheckBoxWithIndex : public QCheckBox {
public:
    CheckBoxWithIndex(const QString& text, int _index)
        : QCheckBox(text)
        , index(_index)
    {
    }

    int getIndex()
    {
        return index;
    }

private:
    int index;
};
