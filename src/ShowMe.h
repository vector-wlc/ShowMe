/*
 * @Coding: gbk
 * @Author: vector-wlc
 * @Date: 2021-08-15 10:38:36
 * @Description: 
 */
#pragma once
#pragma execution_character_set("utf-8")
#include "CheckBoxWithIndex.h"
#include "HlayoutPair.h"
#include "InfoWidget.h"
#include "Memory.h"
#include <QtWidgets/QMainWindow>
#include <list>
#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qcombobox.h>
#include <qfontdialog.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtimer.h>

class ShowMe : public QMainWindow {
    Q_OBJECT

public:
    ShowMe(QWidget* parent = Q_NULLPTR);
    ~ShowMe();

private:
    QTimer* timer;
    QTabWidget* tab_widget;
    std::vector<InfoWidget*> info_widget_vec;
    std::vector<CheckBoxWithIndex*> plant_box_vec;
    std::vector<CheckBoxWithIndex*> zombie_box_vec;
    std::list<int> plant_dict_index;
    std::list<int> zombie_dict_index;

    std::vector<HLayoutPair<QLabel, QLabel>*> label_vec; // 其他页面中的显示label

    // settings
    // 全局
    HLayoutPair<QLabel, QSpinBox>* update_interval_box;
    HLayoutPair<QLabel, QDoubleSpinBox>* opacity_box;
    std::vector<QFont> info_font_vec;
    std::vector<QColor> info_color_vec;
    std::vector<HLayoutPair<QLabel, QPushButton>*> info_font_btn_vec;
    std::vector<HLayoutPair<QLabel, QPushButton>*> info_color_btn_vec;

    // 植物
    HLayoutPair<QLabel, QDoubleSpinBox>* plant_find_threshold_box;
    HLayoutPair<QLabel, QComboBox>* plant_type_box;
    HLayoutPair<QLabel, QSpinBox>* plant_state_box;
    HLayoutPair<QLabel, QDoubleSpinBox>* plant_offset_x_box;
    HLayoutPair<QLabel, QDoubleSpinBox>* plant_offset_y_box;
    HLayoutPair<QLabel, QPushButton>* plant_save_btn;

    // 僵尸
    HLayoutPair<QLabel, QDoubleSpinBox>* zombie_find_threshold_box;
    HLayoutPair<QLabel, QComboBox>* zombie_type_box;
    HLayoutPair<QLabel, QSpinBox>* zombie_state_box;
    HLayoutPair<QLabel, QDoubleSpinBox>* zombie_offset_x_box;
    HLayoutPair<QLabel, QDoubleSpinBox>* zombie_offset_y_box;
    HLayoutPair<QLabel, QPushButton>* zombie_save_btn;

    void run();
    void creatUi();
    QWidget* creatPlantPage();
    QWidget* creatZombiePage();
    QWidget* creatOtherPage();
    QWidget* creatSettingsPage();
    void connectSettingsPage();
    void saveSettings();
    void loadSettings();
};
