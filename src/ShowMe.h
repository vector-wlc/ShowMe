/*
 * @Coding: gbk
 * @Author: vector-wlc
 * @Date: 2021-08-15 10:38:36
 * @Description: 
 */
#pragma once

#include "CheckBoxWithIndex.h"
#include "HlayoutPair.h"
#include "InfoWidget.h"
#include "Memory.h"
#include "MemoryString.h"
#include "MutexWidget.h"

#include <QtWidgets/QMainWindow>
#include <list>
#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qcombobox.h>
#include <qfontdialog.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qsettings.h>
#include <qspinbox.h>
#include <qtimer.h>
#include <set>

class ShowMe : public QMainWindow {
    Q_OBJECT

public:
    ShowMe(QWidget* parent = Q_NULLPTR);
    ~ShowMe();

private:
    QTimer* timer;
    QTabWidget* tab_widget;
    std::vector<InfoWidget*> info_widget_vec;
    std::vector<CheckBoxWithIndex*> plant_state_box_vec;
    std::vector<CheckBoxWithIndex*> zombie_state_box_vec;
    std::vector<CheckBoxWithIndex*> plant_type_box_vec;
    std::vector<CheckBoxWithIndex*> zombie_type_box_vec;
    MutexWidget* plant_mutex_table;
    MutexWidget* zombie_mutex_table;
    std::vector<InfoWidget*> mutex_plant_info_widget_vec;
    std::vector<InfoWidget*> mutex_zombie_info_widget_vec;

    std::list<int> plant_state_list;
    std::list<int> zombie_state_list;
    std::set<int> plant_type_set;
    std::set<int> zombie_type_set;

    std::vector<HLayoutPair<QLabel, QLabel>*> label_vec; // 其他页面中的显示label

    // settings
    // 全局
    HLayoutPair<QLabel, QSpinBox>* update_interval_box;
    HLayoutPair<QLabel, QDoubleSpinBox>* opacity_box;
    std::vector<QFont> info_font_vec;
    std::vector<QColor> info_color_vec;
    std::vector<HLayoutPair<QLabel, QPushButton>*> info_font_btn_vec;
    std::vector<HLayoutPair<QLabel, QPushButton>*> info_color_btn_vec;

    // 对象
    const std::vector<QString>* object_name_ptr;
    std::vector<std::map<int, Offset>>* object_offset_dict_ptr;
    HLayoutPair<QLabel, QComboBox>* object_box;
    HLayoutPair<QLabel, QDoubleSpinBox>* find_threshold_box;
    HLayoutPair<QLabel, QComboBox>* type_box;
    HLayoutPair<QLabel, QSpinBox>* state_box;
    HLayoutPair<QLabel, QDoubleSpinBox>* offset_x_box;
    HLayoutPair<QLabel, QDoubleSpinBox>* offset_y_box;
    HLayoutPair<QLabel, QPushButton>* save_btn;
    double plant_find_threshold;
    double zombie_find_threshold;
    double* object_find_threshold_ptr;

    void showOnMouse();
    void showOnMutex();
    void run();
    void creatUi();

    // Ui
    QWidget* createStatePage(int object_type);
    QWidget* createTypePage(int object_type);
    QWidget* createObjectPage(int object_type);

    QWidget* createMutexPage();

    QWidget* createOtherPage();

    QWidget* createObjectSettingsPage();
    QWidget* createGlobalSettingsPage();
    QWidget* createSettingsPage();

    void createStatusBar();

    // connect
    void connectGlobalSettings();
    void setObjectSettingsPage(int object_type);
    void connectObjectSettings();
    void connectSettings();

    // settings
    void saveGlobalSettings(QSettings& settings);
    void saveObjectPageSettings(QSettings& settings, int object_type);
    void saveObjectSettings(QSettings& settings, int object_type);
    void saveSettings();

    void loadGlobalSettings(QSettings& settings);
    void loadObjectPageSettings(QSettings& settings, int object_type);
    void loadObjectSettings(QSettings& settings, int object_type);
    void loadSettings();
};
