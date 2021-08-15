/*
 * @Coding: gbk
 * @Author: vector-wlc
 * @Date: 2021-08-15 10:38:36
 * @Description: 
 */
#ifndef __LABEL_TYPE_H__
#define __LABEL_TYPE_H__

#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>

// TEMPLATE CLASS LabelType
// ʹ�ô��ཫ������ QClass �����һ��ˮƽ���ֵ�����
template <class First, class Second>
class HLayoutPair : public QWidget {
public:
    HLayoutPair();
    ~HLayoutPair();
    First first;
    Second second;

private:
    QHBoxLayout h_layout;
};

template <class First, class Second>
HLayoutPair<First, Second>::HLayoutPair()
{
    h_layout.addWidget(&first, 2);
    h_layout.addWidget(&second, 3);
    h_layout.setContentsMargins(0, 0, 0, 0);
    setLayout(&h_layout);
}

template <class First, class Second>
HLayoutPair<First, Second>::~HLayoutPair()
{
}

#endif // __LABEL_TYPE_H__
