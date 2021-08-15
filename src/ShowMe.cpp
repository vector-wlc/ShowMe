#include "ShowMe.h"

#include "MemoryString.h"
#include <cmath>
#include <qlayout.h>
#include <qsettings.h>
#include <qtabwidget.h>
#include <qtextcodec.h>

ShowMe::ShowMe(QWidget* parent)
    : QMainWindow(parent)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=]() {
        this->run();
    });

    timer->setInterval(100);
    timer->start();

    creatUi();
    loadSettings();
    connectSettingsPage();
}

ShowMe::~ShowMe()
{
    saveSettings();
    for (auto& info_wigdet : info_widget_vec) {
        delete info_wigdet;
    }
}

void ShowMe::run()
{
    if (!IsDisplayed()) {
        for (auto& info_widget : this->info_widget_vec) {
            info_widget->hide();
        }
        return;
    }

    std::vector<QStringList> text_list_vec;
    std::pair<QStringList, double> text_list_min_distance;
    text_list_min_distance = PlantMemoryToString(plant_dict_index, this->plant_find_threshold_box->second.value());
    text_list_vec.push_back(text_list_min_distance.first);
    this->plant_save_btn->first.setText(QString("%1").arg(text_list_min_distance.second));

    text_list_min_distance = ZombieMemoryToString(zombie_dict_index, this->zombie_find_threshold_box->second.value());
    text_list_vec.push_back(text_list_min_distance.first);
    this->zombie_save_btn->first.setText(QString("%1").arg(text_list_min_distance.second));

    text_list_vec.push_back(SeedMemoryToString());

    text_list_vec.push_back(PlaceMemoryToString());

    int j = 0;
    for (int i = 0; i < text_list_vec.size(); ++i) {
        if (text_list_vec[i].size() != 0) {
            info_widget_vec[i]->setText(text_list_vec[i]);
            int width_offset = info_widget_vec[i]->width() * int(j % 2);
            int height_offset = info_widget_vec[i]->height() * int(j < 2);
            info_widget_vec[i]->move(QCursor::pos().x() - width_offset, QCursor::pos().y() - height_offset);
            info_widget_vec[i]->show();
            ++j;
        } else {
            info_widget_vec[i]->hide();
        }
    }

    // 当页面在 "其他" 时显示其他内容
    if (tab_widget->currentIndex() == 2) {
        auto text_list = OtherMemoryToString();
        for (int i = 0; i < text_list.size(); ++i) {
            this->label_vec[i]->second.setText(text_list[i]);
        }
    }
}

void ShowMe::creatUi()
{
    this->setFont(QFont("Microsoft YaHei"));
    tab_widget = new QTabWidget(this);
    tab_widget->addTab(creatPlantPage(), "植物");
    tab_widget->addTab(creatZombiePage(), "僵尸");
    tab_widget->addTab(creatOtherPage(), "其他");
    tab_widget->addTab(creatSettingsPage(), "设置");
    this->setCentralWidget(tab_widget);
}

QWidget* ShowMe::creatPlantPage()
{
    // 读取 memory_dict.ini 文件获取读取列表
    QSettings settings("./ini/memory_dict.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    QString plant_memory_dict_str = settings.value("plant_memory").toString();
    auto plant_memory_str_list = plant_memory_dict_str.split("|");

    MemoryDict memory_dict;
    for (const auto& plant_memory_str : plant_memory_str_list) {
        if (plant_memory_str.size() == 0) {
            continue;
        }
        auto plant_memory = plant_memory_str.split(",");
        memory_dict.name = plant_memory[0];
        memory_dict.address = plant_memory[1].toInt();
        memory_dict.data_type = plant_memory[2].toInt();
        plant_memory_dict.push_back(memory_dict);
    }

    const int COL_CNT = 4;
    auto widget = new QWidget;
    auto grid_layout = new QGridLayout;
    CheckBoxWithIndex* check_box;
    for (int cnt = 0; cnt < plant_memory_dict.size(); ++cnt) {
        check_box = new CheckBoxWithIndex(plant_memory_dict[cnt].name, cnt);
        connect(check_box, &QCheckBox::stateChanged, [=]() {
            if (check_box->isChecked()) {
                this->plant_dict_index.push_back(check_box->getIndex());
            } else {
                auto iter = std::find(this->plant_dict_index.begin(), this->plant_dict_index.end(), check_box->getIndex());

                if (iter != this->plant_dict_index.end()) {
                    this->plant_dict_index.erase(iter);
                }
            }
        });
        this->plant_box_vec.push_back(check_box);
        grid_layout->addWidget(check_box, cnt / COL_CNT, cnt % COL_CNT);
    }

    auto cancel_all_btn = new QPushButton("取消所有选择");
    connect(cancel_all_btn, &QPushButton::released, [=]() {
        auto tmp = this->plant_dict_index;
        for (const auto& index : tmp) {
            plant_box_vec[index]->setChecked(false);
        }
    });

    grid_layout->addWidget(cancel_all_btn);

    widget->setLayout(grid_layout);
    return widget;
}

QWidget* ShowMe::creatZombiePage()
{
    // 读取 memory_dict.ini 文件获取读取列表
    QSettings settings("./ini/memory_dict.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    QString zombie_memory_dict_str = settings.value("zombie_memory").toString();
    auto zombie_memory_str_list = zombie_memory_dict_str.split("|");

    MemoryDict memory_dict;
    for (const auto& zombie_memory_str : zombie_memory_str_list) {
        if (zombie_memory_str.size() == 0) {
            continue;
        }
        auto zombie_memory = zombie_memory_str.split(",");
        memory_dict.name = zombie_memory[0];
        memory_dict.address = zombie_memory[1].toInt();
        memory_dict.data_type = zombie_memory[2].toInt();
        zombie_memory_dict.push_back(memory_dict);
    }
    const int COL_CNT = 4;
    auto widget = new QWidget;
    auto grid_layout = new QGridLayout;
    CheckBoxWithIndex* check_box;
    for (int cnt = 0; cnt < zombie_memory_dict.size(); ++cnt) {
        check_box = new CheckBoxWithIndex(zombie_memory_dict[cnt].name, cnt);
        connect(check_box, &QCheckBox::stateChanged, [=]() {
            if (check_box->isChecked()) {
                this->zombie_dict_index.push_back(check_box->getIndex());
            } else {
                auto iter = std::find(this->zombie_dict_index.begin(), this->zombie_dict_index.end(), check_box->getIndex());

                if (iter != this->zombie_dict_index.end()) {
                    this->zombie_dict_index.erase(iter);
                }
            }
        });
        this->zombie_box_vec.push_back(check_box);
        grid_layout->addWidget(check_box, cnt / COL_CNT, cnt % COL_CNT);
    }

    auto cancel_all_btn = new QPushButton("取消所有选择");
    connect(cancel_all_btn, &QPushButton::released, [=]() {
        auto tmp = this->zombie_dict_index;
        for (const auto& index : tmp) {
            zombie_box_vec[index]->setChecked(false);
        }
    });

    grid_layout->addWidget(cancel_all_btn);

    widget->setLayout(grid_layout);
    return widget;
}

QWidget* ShowMe::creatOtherPage()
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
        auto h_layout = new QHBoxLayout;
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

QWidget* ShowMe::creatSettingsPage()
{
    // 全局
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

    // 植物
    this->plant_find_threshold_box = new HLayoutPair<QLabel, QDoubleSpinBox>;
    this->plant_find_threshold_box->first.setText("搜索阈值 : ");
    this->plant_find_threshold_box->second.setMinimum(0);
    this->plant_type_box = new HLayoutPair<QLabel, QComboBox>;
    this->plant_type_box->first.setText("类型 : ");
    this->plant_state_box = new HLayoutPair<QLabel, QSpinBox>;
    this->plant_state_box->first.setText("状态 : ");
    this->plant_state_box->second.setMaximum(0xffff);
    this->plant_state_box->second.setMinimum(0);
    this->plant_offset_x_box = new HLayoutPair<QLabel, QDoubleSpinBox>;
    this->plant_offset_x_box->first.setText("X 偏移 : ");
    this->plant_offset_x_box->second.setMinimum(-100);
    this->plant_offset_y_box = new HLayoutPair<QLabel, QDoubleSpinBox>;
    this->plant_offset_y_box->first.setText("Y 偏移 : ");
    this->plant_offset_y_box->second.setMinimum(-100);
    this->plant_save_btn = new HLayoutPair<QLabel, QPushButton>;
    this->plant_save_btn->second.setText("保存修改");

    auto plant_layout = new QGridLayout;
    plant_layout->addWidget(this->plant_find_threshold_box, 0, 0);
    plant_layout->addWidget(this->plant_type_box, 0, 1);
    plant_layout->addWidget(this->plant_state_box, 1, 0);
    plant_layout->addWidget(this->plant_offset_x_box, 1, 1);
    plant_layout->addWidget(this->plant_offset_y_box, 2, 0);
    plant_layout->addWidget(this->plant_save_btn, 2, 1);

    auto plant_box = new QGroupBox("植物");
    plant_box->setLayout(plant_layout);

    // 僵尸
    this->zombie_find_threshold_box = new HLayoutPair<QLabel, QDoubleSpinBox>;
    this->zombie_find_threshold_box->first.setText("搜索阈值 : ");
    this->zombie_find_threshold_box->second.setMinimum(0);
    this->zombie_type_box = new HLayoutPair<QLabel, QComboBox>;
    this->zombie_type_box->first.setText("类型 : ");
    this->zombie_state_box = new HLayoutPair<QLabel, QSpinBox>;
    this->zombie_state_box->first.setText("状态 : ");
    this->zombie_state_box->second.setMaximum(0xffff);
    this->zombie_state_box->second.setMinimum(0);
    this->zombie_offset_x_box = new HLayoutPair<QLabel, QDoubleSpinBox>;
    this->zombie_offset_x_box->first.setText("X 偏移 : ");
    this->zombie_offset_x_box->second.setMinimum(-100);
    this->zombie_offset_y_box = new HLayoutPair<QLabel, QDoubleSpinBox>;
    this->zombie_offset_y_box->first.setText("Y 偏移 : ");
    this->zombie_offset_y_box->second.setMinimum(-100);
    this->zombie_save_btn = new HLayoutPair<QLabel, QPushButton>;
    this->zombie_save_btn->second.setText("保存修改");

    auto zombie_layout = new QGridLayout;
    zombie_layout->addWidget(this->zombie_find_threshold_box, 0, 0);
    zombie_layout->addWidget(this->zombie_type_box, 0, 1);
    zombie_layout->addWidget(this->zombie_state_box, 1, 0);
    zombie_layout->addWidget(this->zombie_offset_x_box, 1, 1);
    zombie_layout->addWidget(this->zombie_offset_y_box, 2, 0);
    zombie_layout->addWidget(this->zombie_save_btn, 2, 1);

    auto zombie_box = new QGroupBox("僵尸");
    zombie_box->setLayout(zombie_layout);

    auto total_layout = new QVBoxLayout;
    total_layout->addWidget(global_box);
    total_layout->addWidget(plant_box);
    total_layout->addWidget(zombie_box);
    auto widget = new QWidget(this);
    widget->setLayout(total_layout);
    return widget;
}

void ShowMe::connectSettingsPage()
{
    // 全局
    connect(&this->update_interval_box->second, (void (QSpinBox::*)(int))(&QSpinBox::valueChanged), [=](int value) {
        timer->setInterval(value);
    });

    connect(&this->opacity_box->second, (void (QDoubleSpinBox::*)(double))(&QDoubleSpinBox::valueChanged), [=](double value) {
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
        connect(&this->info_color_btn_vec[i]->second, &QPushButton::released, [=]() {
            bool ok;
            this->info_color_vec[i] = QColorDialog::getColor(this->info_color_vec[i], this);
            this->info_widget_vec[i]->setColor(this->info_color_vec[i]);
            QPalette palette = this->info_color_btn_vec[i]->second.palette();
            palette.setColor(QPalette::Button, this->info_color_vec[i]);
            this->info_color_btn_vec[i]->second.setPalette(palette);
            this->info_color_btn_vec[i]->second.setAutoFillBackground(true);
            this->info_color_btn_vec[i]->second.setFlat(true);
        });
    }

    // 植物
    for (const auto& name : PLANT_NAME) {
        this->plant_type_box->second.addItem(name);
    }

    auto& plant_offset_map = plant_offset_dict[0];
    auto iter = plant_offset_map.begin();
    this->plant_state_box->second.setValue(iter->first);
    this->plant_offset_x_box->second.setValue(iter->second.x);
    this->plant_offset_y_box->second.setValue(iter->second.y);

    connect(&this->plant_type_box->second, (void (QComboBox::*)(int))(&QComboBox::currentIndexChanged), [=](int index) {
        auto& plant_offset_map = plant_offset_dict[index];
        auto iter = plant_offset_map.begin();
        this->plant_state_box->second.setValue(iter->first);
        this->plant_offset_x_box->second.setValue(iter->second.x);
        this->plant_offset_y_box->second.setValue(iter->second.y);
    });

    connect(&this->plant_state_box->second, (void (QSpinBox::*)(int))(&QSpinBox::valueChanged), [=](int state) {
        auto& plant_offset_map = plant_offset_dict[this->plant_type_box->second.currentIndex()];
        auto iter = plant_offset_map.find(state);
        if (iter != plant_offset_map.end()) {
            this->plant_offset_x_box->second.setValue(iter->second.x);
            this->plant_offset_y_box->second.setValue(iter->second.y);
        }
    });

    connect(&this->plant_save_btn->second, &QPushButton::released, [=]() {
        auto& plant_offset_map = plant_offset_dict[this->plant_type_box->second.currentIndex()];
        auto& offset = plant_offset_map[this->plant_state_box->second.value()];
        offset.x = this->plant_offset_x_box->second.value();
        offset.y = this->plant_offset_y_box->second.value();
    });

    // 僵尸
    for (const auto& name : ZOMBIE_NAME) {
        this->zombie_type_box->second.addItem(name);
    }

    auto& zombie_offset_map = zombie_offset_dict[0];
    iter = zombie_offset_map.begin();
    this->zombie_state_box->second.setValue(iter->first);
    this->zombie_offset_x_box->second.setValue(iter->second.x);
    this->zombie_offset_y_box->second.setValue(iter->second.y);

    connect(&this->zombie_type_box->second, (void (QComboBox::*)(int))(&QComboBox::currentIndexChanged), [=](int index) {
        auto& zombie_offset_map = zombie_offset_dict[index];
        auto iter = zombie_offset_map.begin();
        this->zombie_state_box->second.setValue(iter->first);
        this->zombie_offset_x_box->second.setValue(iter->second.x);
        this->zombie_offset_y_box->second.setValue(iter->second.y);
    });

    connect(&this->zombie_state_box->second, (void (QSpinBox::*)(int))(&QSpinBox::valueChanged), [=](int state) {
        auto& zombie_offset_map = zombie_offset_dict[this->zombie_type_box->second.currentIndex()];
        auto iter = zombie_offset_map.find(state);
        if (iter != zombie_offset_map.end()) {
            this->zombie_offset_x_box->second.setValue(iter->second.x);
            this->zombie_offset_y_box->second.setValue(iter->second.y);
        }
    });

    connect(&this->zombie_save_btn->second, &QPushButton::released, [=]() {
        auto& zombie_offset_map = zombie_offset_dict[this->zombie_type_box->second.currentIndex()];
        auto& offset = zombie_offset_map[this->zombie_state_box->second.value()];
        offset.x = this->zombie_offset_x_box->second.value();
        offset.y = this->zombie_offset_y_box->second.value();
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

const QString KEY_PLANT_PAGE = "plant_page";
const QString KEY_ZOMBIE_PAGE = "zombie_page";
const QString KEY_PLANT_FIND_THRESHOLD = "plant_find_threshold";
const QString KEY_ZOMBIE_FIND_THRESHOLD = "zombie_find_threshold";
const QString KEY_PLANT_OFFSET_DICT = "plant_offset_dict";
const QString KEY_ZOMBIE_OFFSET_DICT = "zombie_offset_dict";

void ShowMe::saveSettings()
{
    QSettings settings("./ini/settings.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));

    // 植物页面
    settings.beginGroup("plant_page");
    QString plant_page_str;
    for (const auto& index : this->plant_dict_index) {
        plant_page_str += QString("%1,").arg(index);
    }
    settings.setValue(KEY_PLANT_PAGE, plant_page_str);
    settings.endGroup();

    // 僵尸页面
    settings.beginGroup("zombie_page");
    QString zombie_page_str;
    for (const auto& index : this->zombie_dict_index) {
        zombie_page_str += QString("%1,").arg(index);
    }
    settings.setValue(KEY_ZOMBIE_PAGE, zombie_page_str);
    settings.endGroup();

    // 全局
    settings.beginGroup("global");
    settings.setValue(KEY_UPDATE_INTERVAL, this->update_interval_box->second.value());
    settings.setValue(KEY_OPACITY, this->opacity_box->second.value());

    for (int i = 0; i < this->info_font_vec.size(); ++i) {
        settings.setValue(KEY_INFO_FONT_TIP_VEC[i], QVariant::fromValue(this->info_font_vec[i]));
        settings.setValue(KEY_INFO_COLOR_TIP_VEC[i], QVariant::fromValue(this->info_color_vec[i]));
    }
    settings.endGroup();

    // 植物
    settings.beginGroup("plant");
    settings.setValue(KEY_PLANT_FIND_THRESHOLD, this->plant_find_threshold_box->second.value());
    QString plant_offset_str;
    for (const auto& plant_offset_map : plant_offset_dict) {
        for (const auto& plant_offset : plant_offset_map) {
            plant_offset_str += QString("%1,%2,%3|").arg(plant_offset.first).arg(plant_offset.second.x).arg(plant_offset.second.y);
        }
        plant_offset_str += "\n";
    }
    settings.setValue(KEY_PLANT_OFFSET_DICT, QVariant::fromValue(plant_offset_str));
    settings.endGroup();

    // 僵尸
    settings.beginGroup("zombie");
    settings.setValue(KEY_ZOMBIE_FIND_THRESHOLD, this->zombie_find_threshold_box->second.value());

    QString zombie_offset_str;
    for (const auto& zombie_offset_map : zombie_offset_dict) {
        for (const auto& zombie_offset : zombie_offset_map) {
            zombie_offset_str += QString("%1,%2,%3|").arg(zombie_offset.first).arg(zombie_offset.second.x).arg(zombie_offset.second.y);
        }
        zombie_offset_str += "\n";
    }
    settings.setValue(KEY_ZOMBIE_OFFSET_DICT, zombie_offset_str);
    settings.endGroup();
}

void ShowMe::loadSettings()
{
    QSettings settings("./ini/settings.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));

    // 植物页面
    settings.beginGroup("plant_page");
    QString plant_page_str = settings.value(KEY_PLANT_PAGE).toString();
    auto plant_index_str_list = plant_page_str.split(",");
    for (const auto& index_str : plant_index_str_list) {
        if (index_str.size() == 0) {
            continue;
        }
        this->plant_box_vec[index_str.toInt()]->setChecked(true);
    }
    settings.endGroup();

    // 僵尸页面
    settings.beginGroup("zombie_page");
    QString zombie_page_str = settings.value(KEY_ZOMBIE_PAGE).toString();
    auto zombie_index_str_list = zombie_page_str.split(",");
    for (const auto& index_str : zombie_index_str_list) {
        if (index_str.size() == 0) {
            continue;
        }
        this->zombie_box_vec[index_str.toInt()]->setChecked(true);
    }
    settings.endGroup();

    // 全局
    settings.beginGroup("global");
    this->update_interval_box->second.setValue(settings.value(KEY_UPDATE_INTERVAL).toInt());
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

    // plant
    settings.beginGroup("plant");
    this->plant_find_threshold_box->second.setValue(settings.value(KEY_PLANT_FIND_THRESHOLD).toDouble());
    QString plant_offset_str = settings.value(KEY_PLANT_OFFSET_DICT).toString();
    auto plant_offset_map_str_list = plant_offset_str.split("\n");
    Offset offset;
    int state;
    plant_offset_dict.clear();
    for (auto& plant_offset_map_str : plant_offset_map_str_list) {
        if (plant_offset_map_str.size() == 0) {
            continue;
        }
        auto plant_offset_str_list = plant_offset_map_str.split("|");
        std::map<int, Offset> plant_offset_map;
        for (auto& plant_offset_str : plant_offset_str_list) {
            if (plant_offset_str.size() == 0) {
                continue;
            }
            auto plant_offset_list = plant_offset_str.split(",");
            state = plant_offset_list[0].toInt();
            offset.x = plant_offset_list[1].toFloat();
            offset.y = plant_offset_list[2].toFloat();
            plant_offset_map.insert({state, offset});
        }
        plant_offset_dict.push_back(plant_offset_map);
    }
    settings.endGroup();

    // zombie
    settings.beginGroup("zombie");
    this->zombie_find_threshold_box->second.setValue(settings.value(KEY_ZOMBIE_FIND_THRESHOLD).toDouble());
    QString zombie_offset_str = settings.value(KEY_ZOMBIE_OFFSET_DICT).toString();
    auto zombie_offset_map_str_list = zombie_offset_str.split("\n");
    zombie_offset_dict.clear();
    for (auto& zombie_offset_map_str : zombie_offset_map_str_list) {
        if (zombie_offset_map_str.size() == 0) {
            continue;
        }
        auto zombie_offset_str_list = zombie_offset_map_str.split("|");
        std::map<int, Offset> zombie_offset_map;
        for (auto& zombie_offset_str : zombie_offset_str_list) {
            if (zombie_offset_str.size() == 0) {
                continue;
            }
            auto zombie_offset_list = zombie_offset_str.split(",");
            state = zombie_offset_list[0].toInt();
            offset.x = zombie_offset_list[1].toFloat();
            offset.y = zombie_offset_list[2].toFloat();
            zombie_offset_map.insert({state, offset});
        }
        zombie_offset_dict.push_back(zombie_offset_map);
    }

    settings.endGroup();
}
