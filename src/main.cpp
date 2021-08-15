/*
 * @Coding: gbk
 * @Author: vector-wlc
 * @Date: 2021-08-15 10:37:59
 * @Description: 
 */
#include "ShowMe.h"

#include <QApplication>
#pragma comment(lib, "user32.lib")

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    ShowMe w;
    w.show();
    return a.exec();
}
