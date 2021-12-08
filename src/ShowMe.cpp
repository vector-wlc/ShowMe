#include "ShowMe.h"
#include "ClickLabel.h"

#include <QDesktopServices>
#include <QMessageBox>
#include <QStatusBar>
#include <cmath>
#include <qevent.h>
#include <qheaderview.h>
#include <qlayout.h>
#include <qsettings.h>
#include <qtabwidget.h>
#include <qtextcodec.h>

ShowMe::ShowMe(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("ShowMe 1.1");
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=]() { this->run(); });

    timer->setInterval(100);
    timer->start();

    this->creatUi();
    this->loadSettings();
    this->connectSettings();
}

ShowMe::~ShowMe()
{
    saveSettings();
    for (auto& info_widget : info_widget_vec) {
        delete info_widget;
    }

    for (auto& info_widget : this->mutex_plant_info_widget_vec) {
        delete info_widget;
    }

    for (auto& info_widget : this->mutex_zombie_info_widget_vec) {
        delete info_widget;
    }
}

void ShowMe::creatUi()
{
    this->setFont(QFont("Microsoft YaHei"));
    tab_widget = new QTabWidget(this);
    this->setCentralWidget(tab_widget);
    tab_widget->addTab(createMutexPage(), "锁定");
    tab_widget->addTab(createOtherPage(), "其他");
    tab_widget->addTab(createSettingsPage(), "设置");
    tab_widget->insertTab(0, createObjectPage(ZOMBIE), "僵尸");
    tab_widget->insertTab(0, createObjectPage(PLANT), "植物");
    tab_widget->setCurrentIndex(0);

    createStatusBar();
}

void ShowMe::loadSettings()
{
    QSettings settings("./ini/settings.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));

    // 植物页面
    loadObjectPageSettings(settings, PLANT);

    // 僵尸页面
    loadObjectPageSettings(settings, ZOMBIE);

    // 全局
    loadGlobalSettings(settings);

    // plant
    loadObjectSettings(settings, PLANT);

    // zombie
    loadObjectSettings(settings, ZOMBIE);
}

void ShowMe::connectSettings()
{
    connectGlobalSettings();
    connectObjectSettings();
}

void ShowMe::saveSettings()
{
    QSettings settings("./ini/settings.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));

    // 植物页面
    saveObjectPageSettings(settings, PLANT);

    // 僵尸页面
    saveObjectPageSettings(settings, ZOMBIE);

    // 全局
    saveGlobalSettings(settings);

    // 植物
    saveObjectSettings(settings, PLANT);

    // 僵尸
    saveObjectSettings(settings, ZOMBIE);
}

void ShowMe::showOnMouse()
{
    // 在鼠标上的显示
    std::vector<QStringList> text_list_vec;
    std::pair<QStringList, double> text_list_min_distance;
    text_list_min_distance = PlantMemoryToString(
        plant_state_list, plant_find_threshold, this->plant_type_set,
        this->update_interval_box->second.value());

    text_list_vec.push_back(text_list_min_distance.first);
    if (this->object_box->second.currentIndex() == PLANT) {
        this->save_btn->first.setText(
            QString("%1").arg(text_list_min_distance.second));
    }

    text_list_min_distance = ZombieMemoryToString(
        zombie_state_list, zombie_find_threshold, this->zombie_type_set,
        this->update_interval_box->second.value());

    text_list_vec.push_back(text_list_min_distance.first);
    if (this->object_box->second.currentIndex() == ZOMBIE) {
        this->save_btn->first.setText(
            QString("%1").arg(text_list_min_distance.second));
    }

    text_list_vec.push_back(SeedMemoryToString());

    text_list_vec.push_back(PlaceMemoryToString());

    int j = 0;
    for (int i = 0; i < text_list_vec.size(); ++i) {
        if (text_list_vec[i].size() != 0) {
            info_widget_vec[i]->setText(text_list_vec[i]);
            int width_offset = info_widget_vec[i]->width() * int(j % 2);
            int height_offset = info_widget_vec[i]->height() * int(j < 2);
            info_widget_vec[i]->move(QCursor::pos().x() - width_offset,
                QCursor::pos().y() - height_offset);
            info_widget_vec[i]->show();
            ++j;
        } else {
            info_widget_vec[i]->hide();
        }
    }
}

void ShowMe::showOnMutex()
{
    // 在锁上的显示
    const int DEFAULT_STATE = 0xffff;
    RECT rect;
    GetWindowRect(g_hwnd, &rect);
    // 当页面在 "锁定" 的时候显示
    bool is_update_table = tab_widget->currentIndex() == 2;
    int row_count = plant_mutex_table->rowCount();
    PlantMemory plant;
    int plant_type;
    for (int row = 0; row < row_count; ++row) {
        int index = plant_mutex_table->item(row, 0)->text().toInt();
        plant.setIndex(index);
        plant_type = plant.type();
        if (plant.isDisappeared() || plant.isCrushed()) {
            if (is_update_table) {
                plant_mutex_table->item(row, 1)->setText("未知");
                plant_mutex_table->item(row, 2)->setText("未知");
            }
            mutex_plant_info_widget_vec[row]->hide();
        } else {
            if (is_update_table) {
                plant_mutex_table->item(row, 1)->setText(PLANT_NAME[plant_type]);
                plant_mutex_table->item(row, 2)->setText(
                    QString("(%1, %2)").arg(plant.row() + 1).arg(plant.col() + 1));
            }
            auto text_list = ObjectIndexToString(plant_state_list, index, PLANT,
                this->update_interval_box->second.value());
            mutex_plant_info_widget_vec[row]->setText(text_list);
            const auto& plant_offset_map = plant_offset_dict[plant_type];
            auto iter = plant_offset_map.find(plant.state());
            if (iter == plant_offset_map.end()) {
                iter = plant_offset_map.find(DEFAULT_STATE);
            }
            int offset_x = plant.hurtWidth() * iter->second.x;
            int x = rect.left + plant.abscissa() + offset_x;
            int y = rect.top + plant.ordinate();
            mutex_plant_info_widget_vec[row]->move(x, y);
            mutex_plant_info_widget_vec[row]->show();
        }
    }

    row_count = zombie_mutex_table->rowCount();
    ZombieMemory zombie;
    int zombie_type;
    for (int row = 0; row < row_count; ++row) {
        int index = zombie_mutex_table->item(row, 0)->text().toInt();
        zombie.setIndex(index);
        zombie_type = zombie.type();
        if (zombie.isDisappeared() || zombie.isDead()) {
            if (is_update_table) {
                zombie_mutex_table->item(row, 1)->setText("未知");
                zombie_mutex_table->item(row, 2)->setText("未知");
            }
            mutex_zombie_info_widget_vec[row]->hide();
        } else {
            if (is_update_table) {
                zombie_mutex_table->item(row, 1)->setText(ZOMBIE_NAME[zombie_type]);
                zombie_mutex_table->item(row, 2)->setText(
                    QString("(%1, %2)")
                        .arg(zombie.row() + 1)
                        .arg(zombie.abscissa() / 80 + 0.5));
            }
            auto text_list = ObjectIndexToString(zombie_state_list, index, ZOMBIE,
                this->update_interval_box->second.value());
            mutex_zombie_info_widget_vec[row]->setText(text_list);
            const auto& zombie_offset_map = zombie_offset_dict[zombie_type];
            auto iter = zombie_offset_map.find(zombie.state());
            if (iter == zombie_offset_map.end()) {
                iter = zombie_offset_map.find(DEFAULT_STATE);
            }
            int offset_x = zombie.hurtWidth() * iter->second.x;
            int offset_y = zombie.hurtHeight() * iter->second.y;
            int x = rect.left + zombie.abscissa() + offset_x;
            int y = rect.top + zombie.ordinate() + offset_y;
            mutex_zombie_info_widget_vec[row]->move(x, y);
            mutex_zombie_info_widget_vec[row]->show();
        }
    }
}

void ShowMe::run()
{
    if (!IsDisplayed()) {
        for (auto& info_widget : this->mutex_plant_info_widget_vec) {
            info_widget->hide();
        }
        for (auto& info_widget : this->mutex_zombie_info_widget_vec) {
            info_widget->hide();
        }
        return;
    }

    showOnMutex();

    if (IsMouseInPvZ()) {
        showOnMouse();
    } else {
        for (auto& info_widget : this->info_widget_vec) {
            info_widget->hide();
        }
    }

    // 当页面在 "其他" 时显示其他内容
    if (tab_widget->currentIndex() == 3) {
        auto text_list = OtherMemoryToString();
        for (int i = 0; i < text_list.size(); ++i) {
            this->label_vec[i]->second.setText(text_list[i]);
        }
    }
}

QWidget* ShowMe::createStatePage(int object_type)
{
    QString type_memory_str = "";
    std::vector<MemoryDict>* memory_dict_ptr;
    std::list<int>* dict_index_ptr;
    std::vector<CheckBoxWithIndex*>* state_box_vec_ptr;

    switch (object_type) {
    case PLANT:
        type_memory_str = "plant_memory";
        memory_dict_ptr = &plant_memory_dict;
        dict_index_ptr = &this->plant_state_list;
        state_box_vec_ptr = &this->plant_state_box_vec;
        break;

    default:
        type_memory_str = "zombie_memory";
        memory_dict_ptr = &zombie_memory_dict;
        dict_index_ptr = &this->zombie_state_list;
        state_box_vec_ptr = &this->zombie_state_box_vec;
        break;
    }
    // 读取 memory_dict.ini 文件获取读取列表
    QSettings settings("./ini/memory_dict.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    QString memory_dict_str = settings.value(type_memory_str).toString();
    auto memory_str_list = memory_dict_str.split("|");

    MemoryDict memory_dict;
    std::vector<QString> data_type_set = {"int", "float", "bool", "short",
        "byte"};

    for (const auto& memory_str : memory_str_list) {
        if (memory_str.size() == 0) {
            continue;
        }
        auto memory = memory_str.split(",");
        if (std::find(data_type_set.begin(), data_type_set.end(), memory[2]) == data_type_set.end()) {
            QMessageBox::information(this, "警告",
                "内存标签: " + memory[0] + " 中的地址类型为 : " + memory[2] + "不是 ShowMe 支持的读取类型，本软件仅支持 "
                                                                              "int float bool short byte 五种类型的读取");
            continue;
        }
        memory_dict.name = memory[0];
        memory_dict.address = memory[1].toInt();
        memory_dict.data_type = memory[2];
        memory_dict_ptr->push_back(memory_dict);
    }

    const int COL_CNT = 4;
    auto widget = new QWidget;
    auto grid_layout = new QGridLayout;
    CheckBoxWithIndex* check_box;
    for (int cnt = 0; cnt < memory_dict_ptr->size(); ++cnt) {
        check_box = new CheckBoxWithIndex((*memory_dict_ptr)[cnt].name, cnt);
        connect(check_box, &QCheckBox::stateChanged, [=]() {
            if (check_box->isChecked()) {
                dict_index_ptr->push_back(check_box->getIndex());
            } else {
                auto iter = std::find(dict_index_ptr->begin(), dict_index_ptr->end(),
                    check_box->getIndex());
                if (iter != dict_index_ptr->end()) {
                    dict_index_ptr->erase(iter);
                }
            }
        });
        state_box_vec_ptr->push_back(check_box);
        grid_layout->addWidget(check_box, cnt / COL_CNT, cnt % COL_CNT);
    }
    widget->setLayout(grid_layout);
    return widget;
}

QWidget* ShowMe::createTypePage(int object_type)
{
    std::set<int>* type_set_ptr;
    std::vector<CheckBoxWithIndex*>* type_box_vec_ptr;
    const std::vector<QString>* type_name_vec_ptr;

    switch (object_type) {
    case PLANT:
        type_set_ptr = &this->plant_type_set;
        type_name_vec_ptr = &PLANT_NAME;
        type_box_vec_ptr = &this->plant_type_box_vec;
        break;

    default:
        type_set_ptr = &this->zombie_type_set;
        type_name_vec_ptr = &ZOMBIE_NAME;
        type_box_vec_ptr = &this->zombie_type_box_vec;
        break;
    }

    const int COL_CNT = 4;
    auto widget = new QWidget;
    auto grid_layout = new QGridLayout;
    CheckBoxWithIndex* check_box;
    for (int cnt = 0; cnt < type_name_vec_ptr->size(); ++cnt) {
        check_box = new CheckBoxWithIndex((*type_name_vec_ptr)[cnt], cnt);
        connect(check_box, &QCheckBox::stateChanged, [=]() {
            if (check_box->isChecked()) {
                type_set_ptr->insert(check_box->getIndex());
            } else {
                auto iter = type_set_ptr->find(check_box->getIndex());
                if (iter != type_set_ptr->end()) {
                    type_set_ptr->erase(iter);
                }
            }
        });
        type_box_vec_ptr->push_back(check_box);
        grid_layout->addWidget(check_box, cnt / COL_CNT, cnt % COL_CNT);
    }

    widget->setLayout(grid_layout);
    return widget;
}

QWidget* ShowMe::createObjectPage(int object_type)
{
    std::vector<CheckBoxWithIndex*>* type_box_vec_ptr;
    std::vector<CheckBoxWithIndex*>* state_box_vec_ptr;

    switch (object_type) {
    case PLANT:
        state_box_vec_ptr = &this->plant_state_box_vec;
        type_box_vec_ptr = &this->plant_type_box_vec;
        break;

    default:
        state_box_vec_ptr = &this->zombie_state_box_vec;
        type_box_vec_ptr = &this->zombie_type_box_vec;
        break;
    }
    auto widget = new QWidget;
    auto tab_widget_ = new QTabWidget;

    auto state_widget = createStatePage(object_type);
    auto type_widget = createTypePage(object_type);

    tab_widget_->addTab(state_widget, "状态");
    tab_widget_->addTab(type_widget, "类型");

    state_widget->show();
    type_widget->hide();

    auto select_all_btn = new QPushButton("全部选择");
    auto cancel_all_btn = new QPushButton("全部取消");

    connect(select_all_btn, &QPushButton::released, [=]() {
        std::vector<CheckBoxWithIndex*>* box_vec_ptr;
        if (tab_widget_->currentIndex() == 0) {
            box_vec_ptr = state_box_vec_ptr;
        } else {
            box_vec_ptr = type_box_vec_ptr;
        }

        for (auto box_ptr : (*box_vec_ptr)) {
            box_ptr->setChecked(true);
        }
    });

    connect(cancel_all_btn, &QPushButton::released, [=]() {
        std::vector<CheckBoxWithIndex*>* box_vec_ptr;
        if (tab_widget_->currentIndex() == 0) {
            box_vec_ptr = state_box_vec_ptr;
        } else {
            box_vec_ptr = type_box_vec_ptr;
        }

        for (auto box_ptr : (*box_vec_ptr)) {
            box_ptr->setChecked(false);
        }
    });

    auto h_layout = new QHBoxLayout;
    h_layout->addStretch();
    h_layout->addWidget(select_all_btn);
    h_layout->addStretch();
    h_layout->addWidget(cancel_all_btn);
    h_layout->addStretch();

    auto v_layout = new QVBoxLayout;
    v_layout->addWidget(tab_widget_);
    v_layout->addLayout(h_layout);
    widget->setLayout(v_layout);
    return widget;
}

QWidget* ShowMe::createMutexPage()
{
    auto h_layout = new QHBoxLayout;
    this->plant_mutex_table = new MutexWidget;
    this->zombie_mutex_table = new MutexWidget;
    this->plant_mutex_table->setColumnCount(3);
    this->zombie_mutex_table->setColumnCount(3);
    this->plant_mutex_table->setRowCount(0);
    this->zombie_mutex_table->setRowCount(0);

    QStringList header_lists;
    header_lists << "植物编号"
                 << "类型"
                 << "位置";

    this->plant_mutex_table->setHorizontalHeaderLabels(header_lists);
    header_lists[0] = "僵尸编号";
    this->zombie_mutex_table->setHorizontalHeaderLabels(header_lists);
    this->plant_mutex_table->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    this->zombie_mutex_table->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);

    h_layout->addWidget(this->plant_mutex_table);
    h_layout->addWidget(this->zombie_mutex_table);

    auto wigdet = new QWidget;
    wigdet->setLayout(h_layout);
    connect(plant_mutex_table, &MutexWidget::rowChanged, [=]() {
        int raw_count = plant_mutex_table->rowCount();
        while (this->mutex_plant_info_widget_vec.size() > raw_count) {
            delete this->mutex_plant_info_widget_vec
                [this->mutex_plant_info_widget_vec.size() - 1];
            mutex_plant_info_widget_vec.pop_back();
        }

        while (this->mutex_plant_info_widget_vec.size() < raw_count) {
            mutex_plant_info_widget_vec.push_back(new InfoWidget);
        }
    });

    connect(zombie_mutex_table, &MutexWidget::rowChanged, [=]() {
        int raw_count = zombie_mutex_table->rowCount();
        while (this->mutex_zombie_info_widget_vec.size() > raw_count) {
            delete this->mutex_zombie_info_widget_vec
                [this->mutex_zombie_info_widget_vec.size() - 1];
            mutex_zombie_info_widget_vec.pop_back();
        }

        while (this->mutex_zombie_info_widget_vec.size() < raw_count) {
            mutex_zombie_info_widget_vec.push_back(new InfoWidget);
        }
    });

    return wigdet;
}

QWidget* ShowMe::createOtherPage()
{
    const std::vector<QString> label_text_vec = {
        "植物栈位 : ",
        "僵尸栈位 : ",
        "冰道坐标 : ",
        "刷新倒计时 : ",
        "大波刷新倒计时 : ",
        "刷新血量 : ",
        "当前波数 : ",
        "游戏时钟 : ",
    };

    // 将所有的显示内容加一个标签
    for (const auto& tip : label_text_vec) {
        auto label_type = new HLayoutPair<QLabel, QLabel>;
        label_type->first.setText(tip);
        this->label_vec.push_back(label_type);
    }

    // 进行页面布局
    auto grid_layout = new QGridLayout;
    for (int i = 3; i < this->label_vec.size(); ++i) {
        grid_layout->addWidget(this->label_vec[i], (i - 1) / 2, (i - 1) % 2);
    }

    auto refresh_box = new QGroupBox("刷新相关");
    refresh_box->setLayout(grid_layout);

    auto v_layout = new QVBoxLayout;
    for (int i = 0; i < 3; ++i) {
        v_layout->addWidget(this->label_vec[i]);
    }

    auto stack_ice_box = new QGroupBox("栈位及冰道");
    stack_ice_box->setLayout(v_layout);

    auto total_layout = new QVBoxLayout;
    total_layout->addWidget(refresh_box);
    total_layout->addWidget(stack_ice_box);

    auto widget = new QWidget(this);
    widget->setLayout(total_layout);
    return widget;
}

QWidget* ShowMe::createObjectSettingsPage()
{
    this->object_box = new HLayoutPair<QLabel, QComboBox>;
    this->object_box->first.setText("对象 : ");
    this->object_box->second.addItem("植物");
    this->object_box->second.addItem("僵尸");
    this->find_threshold_box = new HLayoutPair<QLabel, QDoubleSpinBox>;
    this->find_threshold_box->first.setText("搜索阈值 : ");
    this->find_threshold_box->second.setMinimum(0);
    this->type_box = new HLayoutPair<QLabel, QComboBox>;
    this->type_box->first.setText("类型 : ");
    this->state_box = new HLayoutPair<QLabel, QSpinBox>;
    this->state_box->first.setText("状态 : ");
    this->state_box->second.setMaximum(0xffff);
    this->state_box->second.setMinimum(0);
    this->offset_x_box = new HLayoutPair<QLabel, QDoubleSpinBox>;
    this->offset_x_box->first.setText("X 偏移 : ");
    this->offset_x_box->second.setMinimum(-100);
    this->offset_y_box = new HLayoutPair<QLabel, QDoubleSpinBox>;
    this->offset_y_box->first.setText("Y 偏移 : ");
    this->offset_y_box->second.setMinimum(-100);
    this->save_btn = new HLayoutPair<QLabel, QPushButton>;
    this->save_btn->second.setText("保存修改");

    auto grid_layout = new QGridLayout;

    grid_layout->addWidget(this->object_box, 0, 0);
    grid_layout->addWidget(this->find_threshold_box, 1, 0);
    grid_layout->addWidget(this->type_box, 1, 1);
    grid_layout->addWidget(this->state_box, 2, 0);
    grid_layout->addWidget(this->offset_x_box, 2, 1);
    grid_layout->addWidget(this->offset_y_box, 3, 0);
    grid_layout->addWidget(this->save_btn, 3, 1);

    auto group_box = new QGroupBox("对象");
    group_box->setLayout(grid_layout);
    return group_box;
}

QWidget* ShowMe::createGlobalSettingsPage()
{
    this->update_interval_box = new HLayoutPair<QLabel, QSpinBox>;
    this->update_interval_box->first.setText("刷新间隔(ms) : ");
    this->update_interval_box->second.setMinimum(0);
    this->update_interval_box->second.setMaximum(10000);
    this->opacity_box = new HLayoutPair<QLabel, QDoubleSpinBox>;
    this->opacity_box->first.setText("透明度: ");
    this->opacity_box->second.setMinimum(0);
    this->opacity_box->second.setMaximum(1);
    const std::vector<QString> INFO_FONT_TIP_VEC = {
        "植物字体 : ",
        "僵尸字体 : ",
        "卡槽字体 : ",
        "核坑字体 : ",
    };

    const std::vector<QString> INFO_COLOR_TIP_VEC = {
        "植物颜色 : ",
        "僵尸颜色 : ",
        "卡槽颜色 : ",
        "核坑颜色 : ",
    };

    auto global_layout = new QGridLayout;
    global_layout->addWidget(this->update_interval_box, 0, 0);
    global_layout->addWidget(this->opacity_box, 0, 1);
    for (int i = 0; i < INFO_FONT_TIP_VEC.size(); ++i) {
        this->info_font_btn_vec.push_back(new HLayoutPair<QLabel, QPushButton>);
        this->info_font_btn_vec[i]->first.setText(INFO_FONT_TIP_VEC[i]);
        this->info_color_btn_vec.push_back(new HLayoutPair<QLabel, QPushButton>);
        this->info_color_btn_vec[i]->first.setText(INFO_COLOR_TIP_VEC[i]);
        auto v_layout = new QVBoxLayout;
        v_layout->addWidget(this->info_font_btn_vec[i]);
        v_layout->addWidget(this->info_color_btn_vec[i]);
        global_layout->addLayout(v_layout, i / 2 + 1, i % 2);
    }

    for (int i = 0; i < this->info_font_btn_vec.size(); ++i) {
        info_widget_vec.push_back(new InfoWidget);
    }

    auto global_box = new QGroupBox("全局");
    global_box->setLayout(global_layout);

    return global_box;
}

QWidget* ShowMe::createSettingsPage()
{
    auto total_layout = new QVBoxLayout;
    total_layout->addWidget(createGlobalSettingsPage());
    total_layout->addWidget(createObjectSettingsPage());
    auto widget = new QWidget(this);
    widget->setLayout(total_layout);
    return widget;
}

void ShowMe::createStatusBar()
{
    // 状态栏
    auto author_label = new ClickLabel;
    author_label->setText("Made by vector-wlc");
    auto msvc_label = new ClickLabel;
    msvc_label->setText("   Build by MSVC2019");
    auto qt_label = new ClickLabel;
    qt_label->setText("and Qt 5.14.2");

    this->statusBar()->addWidget(author_label);
    this->statusBar()->addWidget(msvc_label);
    this->statusBar()->addWidget(qt_label);

    connect(author_label, &ClickLabel::clicked, [=]() {
        QDesktopServices::openUrl(
            QUrl(QString("https://github.com/vector-wlc/ShowMe")));
    });
    connect(msvc_label, &ClickLabel::clicked, [=]() {
        QDesktopServices::openUrl(
            QUrl(QString("https://visualstudio.microsoft.com")));
    });
    connect(qt_label, &ClickLabel::clicked, [=]() {
        QDesktopServices::openUrl(QUrl(QString("https://www.qt.io/")));
    });
}

void ShowMe::connectGlobalSettings()
{
    connect(&this->update_interval_box->second,
        (void (QSpinBox::*)(int))(&QSpinBox::valueChanged),
        [=](int value) { timer->setInterval(value); });

    connect(&this->opacity_box->second,
        (void (QDoubleSpinBox::*)(double))(&QDoubleSpinBox::valueChanged),
        [=](double value) {
            for (auto& info_widget : this->info_widget_vec)
                info_widget->setWindowOpacity(value);
        });

    for (int i = 0; i < this->info_font_btn_vec.size(); ++i) {
        connect(&this->info_font_btn_vec[i]->second, &QPushButton::released, [=]() {
            bool ok;
            this->info_font_vec[i] = QFontDialog::getFont(&ok, this->info_font_vec[i], this);
            this->info_widget_vec[i]->setFont(this->info_font_vec[i]);
            auto font_str = this->info_font_vec[i].family() + QString(", %1").arg(this->info_font_vec[i].pointSize());
            this->info_font_btn_vec[i]->second.setText(font_str);
        });
    }

    for (int i = 0; i < this->info_color_btn_vec.size(); ++i) {
        connect(&this->info_color_btn_vec[i]->second, &QPushButton::released,
            [=]() {
                this->info_color_vec[i] = QColorDialog::getColor(this->info_color_vec[i], this);
                this->info_widget_vec[i]->setColor(this->info_color_vec[i]);
                QPalette palette = this->info_color_btn_vec[i]->second.palette();
                palette.setColor(QPalette::Button, this->info_color_vec[i]);
                this->info_color_btn_vec[i]->second.setPalette(palette);
                this->info_color_btn_vec[i]->second.setAutoFillBackground(true);
                this->info_color_btn_vec[i]->second.setFlat(true);
            });
    }
}

void ShowMe::setObjectSettingsPage(int object_type)
{
    switch (object_type) {
    case PLANT:
        this->object_name_ptr = &PLANT_NAME;
        this->object_offset_dict_ptr = &plant_offset_dict;
        this->object_find_threshold_ptr = &plant_find_threshold;
        break;

    default:
        this->object_name_ptr = &ZOMBIE_NAME;
        this->object_offset_dict_ptr = &zombie_offset_dict;
        this->object_find_threshold_ptr = &zombie_find_threshold;
        break;
    }

    this->type_box->second.clear();
    for (const auto& name : *this->object_name_ptr) {
        this->type_box->second.addItem(name);
    }
    this->find_threshold_box->second.setValue(*object_find_threshold_ptr);
}

void ShowMe::connectObjectSettings()
{
    setObjectSettingsPage(PLANT);
    auto& object_offset_map = (*this->object_offset_dict_ptr)[0];
    auto iter = object_offset_map.begin();
    this->state_box->second.setValue(iter->first);
    this->offset_x_box->second.setValue(iter->second.x);
    this->offset_y_box->second.setValue(iter->second.y);

    connect(&this->object_box->second,
        (void (QComboBox::*)(int))(&QComboBox::currentIndexChanged),
        [=](int object_type) { setObjectSettingsPage(object_type); });

    connect(&this->find_threshold_box->second,
        (void (QDoubleSpinBox::*)(double))(&QDoubleSpinBox::valueChanged),
        [=](double value) {
            *this->object_find_threshold_ptr = this->find_threshold_box->second.value();
        });

    connect(&this->type_box->second,
        (void (QComboBox::*)(int))(&QComboBox::currentIndexChanged),
        [=](int index) {
            if (index < 0) {
                return;
            }
            auto& object_offset_map = (*this->object_offset_dict_ptr)[index];
            auto iter = object_offset_map.begin();
            this->state_box->second.setValue(iter->first);
            this->offset_x_box->second.setValue(iter->second.x);
            this->offset_y_box->second.setValue(iter->second.y);
        });

    connect(&this->state_box->second,
        (void (QSpinBox::*)(int))(&QSpinBox::valueChanged), [=](int state) {
            auto& object_offset_map = (*this->object_offset_dict_ptr)[this->type_box->second
                                                                          .currentIndex()];
            auto iter = object_offset_map.find(state);
            if (iter != object_offset_map.end()) {
                this->offset_x_box->second.setValue(iter->second.x);
                this->offset_y_box->second.setValue(iter->second.y);
            }
        });

    connect(&this->save_btn->second, &QPushButton::released, [=]() {
        auto& object_offset_map = (*this->object_offset_dict_ptr)[this->type_box->second.currentIndex()];
        auto& offset = object_offset_map[this->state_box->second.value()];
        offset.x = this->offset_x_box->second.value();
        offset.y = this->offset_y_box->second.value();
    });
}

const QString KEY_UPDATE_INTERVAL = "update_interval";
const QString KEY_OPACITY = "opacity";

const std::vector<QString> KEY_INFO_FONT_TIP_VEC = {
    "plant_font",
    "zombie_font",
    "seed_font",
    "crater_font",
};

const std::vector<QString> KEY_INFO_COLOR_TIP_VEC = {
    "plant_color",
    "zombie_color",
    "seed_color",
    "crater_color",
};

const QString KEY_PLANT_STATE = "plant_state";
const QString KEY_ZOMBIE_STATE = "zombie_state";
const QString KEY_PLANT_TYPE = "plant_type";
const QString KEY_ZOMBIE_TYPE = "zombie_type";
const QString KEY_PLANT_FIND_THRESHOLD = "plant_find_threshold";
const QString KEY_ZOMBIE_FIND_THRESHOLD = "zombie_find_threshold";
const QString KEY_PLANT_OFFSET_DICT = "plant_offset_dict";
const QString KEY_ZOMBIE_OFFSET_DICT = "zombie_offset_dict";

void ShowMe::saveGlobalSettings(QSettings& settings)
{
    settings.beginGroup("global");
    settings.setValue(KEY_UPDATE_INTERVAL,
        this->update_interval_box->second.value());
    settings.setValue(KEY_OPACITY, this->opacity_box->second.value());

    for (int i = 0; i < this->info_font_vec.size(); ++i) {
        settings.setValue(KEY_INFO_FONT_TIP_VEC[i],
            QVariant::fromValue(this->info_font_vec[i]));
        settings.setValue(KEY_INFO_COLOR_TIP_VEC[i],
            QVariant::fromValue(this->info_color_vec[i]));
    }
    settings.endGroup();
}

void ShowMe::saveObjectPageSettings(QSettings& settings, int object_type)
{
    QString settings_str;
    QString key_state_str;
    QString key_type_str;
    std::list<int>* object_state_list_ptr;
    std::set<int>* object_type_set_ptr;

    switch (object_type) {
    case PLANT:
        settings_str = "plant_page";
        key_state_str = KEY_PLANT_STATE;
        key_type_str = KEY_PLANT_TYPE;
        object_state_list_ptr = &this->plant_state_list;
        object_type_set_ptr = &this->plant_type_set;
        break;

    default:
        settings_str = "zombie_page";
        key_state_str = KEY_ZOMBIE_STATE;
        key_type_str = KEY_ZOMBIE_TYPE;
        object_state_list_ptr = &this->zombie_state_list;
        object_type_set_ptr = &this->zombie_type_set;
        break;
    }
    settings.beginGroup(settings_str);
    QString state_str;
    for (const auto& index : *object_state_list_ptr) {
        state_str += QString("%1,").arg(index);
    }
    settings.setValue(key_state_str, state_str);

    QString type_str;
    for (const auto& index : *object_type_set_ptr) {
        type_str += QString("%1,").arg(index);
    }
    settings.setValue(key_type_str, type_str);
    settings.endGroup();
}

void ShowMe::saveObjectSettings(QSettings& settings, int object_type)
{
    QString settings_str;
    QString key_find_thresold_str;
    QString key_object_offset_dict_str;
    int object_find_threshold;
    std::vector<std::map<int, Offset>>* object_offset_dict_ptr;

    switch (object_type) {
    case PLANT:
        settings_str = "plant";
        key_find_thresold_str = KEY_PLANT_FIND_THRESHOLD;
        key_object_offset_dict_str = KEY_PLANT_OFFSET_DICT;
        object_offset_dict_ptr = &plant_offset_dict;
        object_find_threshold = plant_find_threshold;
        break;

    default:
        settings_str = "zombie";
        key_find_thresold_str = KEY_ZOMBIE_FIND_THRESHOLD;
        key_object_offset_dict_str = KEY_ZOMBIE_OFFSET_DICT;
        object_offset_dict_ptr = &zombie_offset_dict;
        object_find_threshold = zombie_find_threshold;
        break;
    }
    settings.beginGroup(settings_str);
    settings.setValue(key_find_thresold_str, object_find_threshold);
    QString object_offset_str;
    for (const auto& object_offset_map : *object_offset_dict_ptr) {
        for (const auto& object_offset : object_offset_map) {
            object_offset_str += QString("%1,%2,%3|")
                                     .arg(object_offset.first)
                                     .arg(object_offset.second.x)
                                     .arg(object_offset.second.y);
        }
        object_offset_str += "\n";
    }
    settings.setValue(key_object_offset_dict_str,
        QVariant::fromValue(object_offset_str));
    settings.endGroup();
}

void ShowMe::loadGlobalSettings(QSettings& settings)
{
    settings.beginGroup("global");
    this->update_interval_box->second.setValue(
        settings.value(KEY_UPDATE_INTERVAL).toInt());
    this->opacity_box->second.setValue(settings.value(KEY_OPACITY).toDouble());

    QVariant tmp_value;
    for (int i = 0; i < this->info_widget_vec.size(); ++i) {
        // 字体
        tmp_value = settings.value(KEY_INFO_FONT_TIP_VEC[i]);
        if (tmp_value.canConvert<QFont>()) {
            this->info_font_vec.push_back(tmp_value.value<QFont>());
        }
        this->info_widget_vec[i]->setFont(this->info_font_vec[i]);
        auto font_str = this->info_font_vec[i].family() + QString(", %1").arg(this->info_font_vec[i].pointSize());
        this->info_font_btn_vec[i]->second.setText(font_str);

        // 颜色
        tmp_value = settings.value(KEY_INFO_COLOR_TIP_VEC[i]);
        if (tmp_value.canConvert<QColor>()) {
            this->info_color_vec.push_back(tmp_value.value<QColor>());
        }
        this->info_widget_vec[i]->setColor(this->info_color_vec[i]);
        QPalette palette = this->info_color_btn_vec[i]->second.palette();
        palette.setColor(QPalette::Button, this->info_color_vec[i]);
        this->info_color_btn_vec[i]->second.setPalette(palette);
        this->info_color_btn_vec[i]->second.setAutoFillBackground(true);
        this->info_color_btn_vec[i]->second.setFlat(true);
    }
    settings.endGroup();
}

void ShowMe::loadObjectPageSettings(QSettings& settings, int object_type)
{
    QString settings_str;
    QString key_state_str;
    QString key_type_str;
    std::list<int>* object_state_list_ptr;
    std::set<int>* object_type_set_ptr;
    std::vector<CheckBoxWithIndex*>* object_state_box_vec_ptr;
    std::vector<CheckBoxWithIndex*>* object_type_box_vec_ptr;

    switch (object_type) {
    case PLANT:
        settings_str = "plant_page";
        key_state_str = KEY_PLANT_STATE;
        key_type_str = KEY_PLANT_TYPE;
        object_state_list_ptr = &this->plant_state_list;
        object_type_set_ptr = &this->plant_type_set;
        object_state_box_vec_ptr = &this->plant_state_box_vec;
        object_type_box_vec_ptr = &this->plant_type_box_vec;
        break;

    default:
        settings_str = "zombie_page";
        key_state_str = KEY_ZOMBIE_STATE;
        key_type_str = KEY_ZOMBIE_TYPE;
        object_state_list_ptr = &this->zombie_state_list;
        object_type_set_ptr = &this->zombie_type_set;
        object_state_box_vec_ptr = &this->zombie_state_box_vec;
        object_type_box_vec_ptr = &this->zombie_type_box_vec;
        break;
    }

    settings.beginGroup(settings_str);
    QString object_state_str = settings.value(key_state_str).toString();
    auto object_index_str_list = object_state_str.split(",");
    for (const auto& index_str : object_index_str_list) {
        if (index_str.size() == 0) {
            continue;
        }
        (*object_state_box_vec_ptr)[index_str.toInt()]->setChecked(true);
    }

    QString object_type_str = settings.value(key_type_str).toString();
    object_index_str_list = object_type_str.split(",");
    for (const auto& index_str : object_index_str_list) {
        if (index_str.size() == 0) {
            continue;
        }
        (*object_type_box_vec_ptr)[index_str.toInt()]->setChecked(true);
    }
    settings.endGroup();
}

void ShowMe::loadObjectSettings(QSettings& settings, int object_type)
{
    QString settings_str;
    QString key_find_thresold_str;
    QString key_object_offset_dict_str;
    std::vector<std::map<int, Offset>>* object_offset_dict_ptr;

    switch (object_type) {
    case PLANT:
        settings_str = "plant";
        key_find_thresold_str = KEY_PLANT_FIND_THRESHOLD;
        key_object_offset_dict_str = KEY_PLANT_OFFSET_DICT;
        object_offset_dict_ptr = &plant_offset_dict;
        object_find_threshold_ptr = &this->plant_find_threshold;
        break;

    default:
        settings_str = "zombie";
        key_find_thresold_str = KEY_ZOMBIE_FIND_THRESHOLD;
        key_object_offset_dict_str = KEY_ZOMBIE_OFFSET_DICT;
        object_offset_dict_ptr = &zombie_offset_dict;
        object_find_threshold_ptr = &this->zombie_find_threshold;
        break;
    }
    settings.beginGroup(settings_str);
    *object_find_threshold_ptr = settings.value(key_find_thresold_str).toDouble();
    this->find_threshold_box->second.setValue(*object_find_threshold_ptr);

    QString object_offset_str = settings.value(key_object_offset_dict_str).toString();
    auto object_offset_map_str_list = object_offset_str.split("\n");
    Offset offset;
    int state;
    object_offset_dict_ptr->clear();
    for (auto& object_offset_map_str : object_offset_map_str_list) {
        if (object_offset_map_str.size() == 0) {
            continue;
        }
        auto object_offset_str_list = object_offset_map_str.split("|");
        std::map<int, Offset> object_offset_map;
        for (auto& object_offset_str : object_offset_str_list) {
            if (object_offset_str.size() == 0) {
                continue;
            }
            auto object_offset_list = object_offset_str.split(",");
            state = object_offset_list[0].toInt();
            offset.x = object_offset_list[1].toFloat();
            offset.y = object_offset_list[2].toFloat();
            object_offset_map.insert({state, offset});
        }
        object_offset_dict_ptr->push_back(object_offset_map);
    }
    settings.endGroup();
}
