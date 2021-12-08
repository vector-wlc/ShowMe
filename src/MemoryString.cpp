#include "MemoryString.h"
#include <math.h>

const std::vector<QString> PLANT_NAME = {
    "豌豆射手",
    "向日葵",
    "樱桃炸弹",
    "坚果",
    "土豆雷",
    "寒冰射手",
    "大嘴花",
    "双发射手",
    "小喷菇",
    "阳光菇",
    "大喷菇",
    "墓被吞噬者",
    "魅惑菇",
    "胆小菇",
    "寒冰菇",
    "毁灭菇",
    "睡莲",
    "倭瓜",
    "三线射手",
    "缠绕海藻",
    "火爆辣椒",
    "地刺",
    "火炬树桩",
    "高坚果",
    "海蘑菇",
    "路灯花",
    "仙人掌",
    "三叶草",
    "裂荚射手",
    "杨桃",
    "南瓜头",
    "磁力菇",
    "卷心菜投手",
    "花盆",
    "玉米投手",
    "咖啡豆",
    "大蒜",
    "叶子保护伞",
    "金盏花",
    "西瓜投手",
    "机枪射手",
    "双子向日葵",
    "忧郁蘑菇",
    "香蒲",
    "冰瓜",
    "吸金磁",
    "地刺王",
    "玉米加农炮",
    "模仿者",
};

const std::vector<QString> ZOMBIE_NAME = {
    "普僵",
    "旗帜",
    "路障",
    "撑杆",
    "铁桶",
    "读报",
    "铁门",
    "橄榄",
    "舞王",
    "伴舞",
    "鸭子",
    "潜水",
    "冰车",
    "雪橇",
    "海豚",
    "小丑",
    "气球",
    "矿工",
    "跳跳",
    "雪人",
    "蹦极",
    "扶梯",
    "投篮",
    "白眼",
    "小鬼",
    "僵博",
    "豌豆",
    "坚果",
    "辣椒",
    "机枪",
    "倭瓜",
    "高墙",
    "红眼",
};

const int DEFAULT_STATE = 0xffff;

std::vector<MemoryDict> plant_memory_dict;
std::vector<MemoryDict> zombie_memory_dict;
std::vector<std::map<int, Offset>> plant_offset_dict;
std::vector<std::map<int, Offset>> zombie_offset_dict;

QStringList ObjectIndexToString(const std::list<int>& dict_index, int target_index, int object_type, int update_time)
{
    std::vector<MemoryDict>* memory_dict_vec;
    AbstractMemory* abstract_memory;
    int object_offset;
    int data_size;
    const std::vector<QString>* name;
    if (object_type == PLANT) {
        memory_dict_vec = &plant_memory_dict;
        abstract_memory = new PlantMemory(target_index);
        object_offset = PLANT_OFFSET;
        data_size = 0x14c;
        name = &PLANT_NAME;
    } else {
        memory_dict_vec = &zombie_memory_dict;
        abstract_memory = new ZombieMemory(target_index);
        object_offset = ZOMBIE_OFFSET;
        data_size = 0x15c;
        name = &ZOMBIE_NAME;
    }

    // 选中的对象发光
    uintptr_t offset = ReadMemory<uintptr_t>(g_mainobject + object_offset);
    if (object_type == PLANT) {
        WriteMemory(update_time > 1000 ? update_time / 10 : 100, offset + target_index * 0x14c + 0xb8);
    } else {
        WriteMemory(update_time > 1000 ? update_time / 10 : 100, offset + target_index * 0x15c + 0x54);
    }
    abstract_memory->setIndex(target_index);

    QStringList text_list;
    text_list.append(QString("类型 : %1").arg((*name)[abstract_memory->type()]));
    delete abstract_memory;
    for (const auto& index : dict_index) {
        const auto& memory_dict = (*memory_dict_vec)[index];
        if (index != 0) {
            if (memory_dict.data_type == "int") {
                text_list.append(QString("%1 : %2").arg(memory_dict.name).arg(ReadMemory<int>(offset + target_index * data_size + memory_dict.address)));
            } else if (memory_dict.data_type == "float") {
                text_list.append(QString("%1 : %2").arg(memory_dict.name).arg(ReadMemory<float>(offset + target_index * data_size + memory_dict.address)));
            } else if (memory_dict.data_type == "bool") {
                text_list.append(QString("%1 : %2").arg(memory_dict.name).arg(ReadMemory<bool>(offset + target_index * data_size + memory_dict.address)));
            } else if (memory_dict.data_type == "short") {
                text_list.append(QString("%1 : %2").arg(memory_dict.name).arg(ReadMemory<short>(offset + target_index * data_size + memory_dict.address)));
            } else if (memory_dict.data_type == "byte") {
                text_list.append(QString("%1 : %2").arg(memory_dict.name).arg(ReadMemory<byte>(offset + target_index * data_size + memory_dict.address)));
            }
        } else {
            text_list.append(QString("%1 : %2").arg(memory_dict.name).arg(target_index));
        }
    }

    return text_list;
}

std::pair<QStringList, double> PlantMemoryToString(const std::list<int>& dict_index, double find_threshold, const std::set<int>& type_set, int update_time)
{
    int cnt_max = PlantMemory::countMax();

    // 计算离鼠标最近的对象
    static int last_target_index = -1;
    int target_index = -1;
    int mouse_x = PvZMouseX();
    int mouse_y = PvZMouseY();
    float offset_x = 0;
    float offset_y = 0;
    float min_distance = 0xffff;
    PlantMemory plant;
    int plant_type;
    for (int index = 0; index < cnt_max; ++index) {
        plant.setIndex(index);
        if (plant.isCrushed() || plant.isDisappeared()) {
            continue;
        }
        plant_type = plant.type();
        if (type_set.find(plant_type) == type_set.end()) {
            continue;
        }
        const auto& plant_offset_map = plant_offset_dict[plant.type()];
        auto iter = plant_offset_map.find(plant.state());
        if (iter == plant_offset_map.end()) {
            iter = plant_offset_map.find(DEFAULT_STATE);
        }
        offset_x = plant.hurtWidth() * iter->second.x;
        offset_y = plant.hurtHeight() * iter->second.y;
        float x_distance = mouse_x - plant.abscissa() - offset_x;
        float y_distance = mouse_y - plant.ordinate() - offset_y;
        float distance = sqrt(x_distance * x_distance + y_distance * y_distance);
        if (distance < min_distance) {
            min_distance = distance;
            target_index = index;
        }
    }

    if (min_distance > find_threshold) {
        return {QStringList(), min_distance};
    }
    return {ObjectIndexToString(dict_index, target_index, PLANT, update_time), min_distance};
}

std::pair<QStringList, double> ZombieMemoryToString(const std::list<int>& dict_index, double find_threshold, const std::set<int>& type_set, int update_time)
{
    QStringList text_list;
    int cnt_max = ZombieMemory::countMax();

    // 计算离鼠标最近的对象
    static int last_target_index = -1;
    int target_index = -1;
    int mouse_x = PvZMouseX();
    int mouse_y = PvZMouseY();
    float offset_x = 0;
    float offset_y = 0;
    float min_distance = 0xffff;
    ZombieMemory zombie;
    int zombie_type;
    for (int index = 0; index < cnt_max; ++index) {

        zombie.setIndex(index);
        if (zombie.isDead() || zombie.isDisappeared()) {
            continue;
        }
        zombie_type = zombie.type();
        if (type_set.find(zombie_type) == type_set.end()) {
            continue;
        }
        const auto& zombie_offset_map = zombie_offset_dict[zombie_type];
        auto iter = zombie_offset_map.find(zombie.state());
        if (iter == zombie_offset_map.end()) {
            iter = zombie_offset_map.find(DEFAULT_STATE);
        }
        offset_x = zombie.hurtWidth() * iter->second.x;
        offset_y = zombie.hurtHeight() * iter->second.y;

        float x_distance = mouse_x - zombie.abscissa() - offset_x;
        float y_distance = mouse_y - zombie.ordinate() - offset_y;
        float distance = sqrt(x_distance * x_distance + y_distance * y_distance);
        if (distance < min_distance) {
            min_distance = distance;
            target_index = index;
        }
    }

    if (min_distance > find_threshold) {
        return {QStringList(), min_distance};
    }

    return {ObjectIndexToString(dict_index, target_index, ZOMBIE, update_time), min_distance};
}

QStringList SeedMemoryToString()
{
    QStringList text_list;

    // 计算离鼠标最近的对象
    int target_index = -1;
    int mouse_x = PvZMouseX();
    int mouse_y = PvZMouseY();
    SeedMemory seed;
    int cnt_max = seed.slotsCount();
    int seed_x;
    int seed_y;
    int seed_width;
    int seed_height;
    for (int index = 0; index < cnt_max; ++index) {
        seed.setIndex(index);
        seed_x = seed.abscissa() + seed.xOffset();
        seed_y = seed.ordinate();
        seed_width = seed.width();
        seed_height = seed.height();
        if (mouse_x - seed_x > 0 && mouse_x - seed_x < seed_width && mouse_y - seed_y > 0 && mouse_y - seed_y < seed_height) {
            target_index = index;
            break;
        }
    }

    if (target_index == -1) {
        return text_list;
    }
    seed.setIndex(target_index);
    int seed_type = seed.type();

    if (seed_type < 0 || seed_type >= PLANT_NAME.size()) {
        return text_list; // 超出索引值直接返回空窗口
    }
    QString str = "类型 : ";
    if (seed_type == 48) {
        seed_type = seed.imitatorType();
        str += "模仿";
    }
    str += PLANT_NAME[seed_type];
    text_list.append(str);
    text_list.append(QString("冷却 : %1 / %2").arg(seed.CD()).arg(seed.initialCD()));

    return text_list;
}

QStringList PlaceMemoryToString()
{
    QStringList text_list;
    int cnt_max = PlaceMemory::countMax();

    int target_index = -1;
    PlaceMemory place;

    for (int index = 0; index < cnt_max; ++index) {
        place.setIndex(index);
        if (!place.isExist() || place.isDisappeared() || place.type() != 2) {
            continue;
        }

        if ((MouseRow() == (place.row() + 1)) && (std::abs(MouseCol() - place.col() - 1) < 0.5)) {
            target_index = index;
            break;
        }
    }
    if (target_index == -1) {
        return text_list;
    }

    place.setIndex(target_index);
    text_list.append(QString("类型 : 弹坑"));
    text_list.append(QString("消失倒计时 : %1").arg(place.value()));

    return text_list;
}

enum MemoryIndex {
    PLANT_STACK,       // 植物栈位
    ZOMBIE_STACK,      // 僵尸栈位
    ICE_ROAD_ABSCISSA, // 冰道的坐标

    REFRESH_COUNTDOWN,      // 刷新倒计时
    HUGE_REFRESH_COUNTDOWN, // 大波刷新倒计时
    REFRESH_HP,             // 刷新血量
    WAVE,                   // 当前的波数
    CLOCK                   // 计时系统
};

QStringList OtherMemoryToString()
{
    QStringList text_list;
    QString text;
    // 植物栈位
    uintptr_t plant_offset = ReadMemory<uintptr_t>(g_mainobject + 0xac);
    int plant_cnt_max = PlantMemory::countMax();
    int stack_top = ReadMemory<int>(g_mainobject + 0xb8);

    text = QString("%1 ").arg(stack_top);
    int next_index = stack_top;
    int max_show_item = 20;
    int item_num = 0;
    while (next_index != plant_cnt_max && item_num < max_show_item) {
        next_index = ReadMemory<int>(plant_offset + 0x148 + next_index * 0x14c);
        text += QString("%1 ").arg(next_index);
        ++item_num;
    }

    text_list.append(text);

    // 僵尸栈位
    uintptr_t zombie_offset = ReadMemory<uintptr_t>(g_mainobject + 0x90);
    int zombie_cnt_max = ZombieMemory::countMax();
    stack_top = ReadMemory<int>(g_mainobject + 0x9c);
    item_num = 0;
    text = QString("%1 ").arg(stack_top);
    next_index = stack_top;
    while (next_index != zombie_cnt_max && item_num < max_show_item) {
        next_index = ReadMemory<int>(zombie_offset + 0x158 + next_index * 0x15c);
        text += QString("%1 ").arg(next_index);
        ++item_num;
    }

    text_list.append(text);
    text.clear();

    // 冰道
    for (int row = 0; row < 5; ++row) {
        text += QString("%1 : %2 | ").arg(row + 1).arg(IceAbscissa(row));
    }
    text += QString("%1 : %2").arg(6).arg(IceAbscissa(5));
    text_list.append(text);

    // 刷新倒计时
    text = QString("%1 / %2").arg(Countdown()).arg(InitialCountdown());
    text_list.append(text);

    // 大波刷新倒计时
    text = QString("%1").arg(HugeWaveCountdown());
    text_list.append(text);

    // 刷新血量
    text = QString("%1").arg(RefreshHp());
    text_list.append(text);

    // 当前波数
    text = QString("%1 / %2").arg(NowWave()).arg(TotalWave());
    text_list.append(text);

    // 计时系统
    text = QString("%1").arg(GameClock());
    text_list.append(text);

    return text_list;
}
