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

std::pair<QStringList, double> PlantMemoryToString(const std::list<int>& dict_index, double find_threshold)
{
    QStringList text_list;
    int cnt_max = PlantMemory::countMax();

    // 计算离鼠标最近的对象
    int target_index = 0;
    int mouse_x = PvZMouseX();
    int mouse_y = PvZMouseY();
    float offset_x = 0;
    float offset_y = 0;
    int plant_type = 0;
    float min_distance = 0xffff;
    PlantMemory plant;
    for (int index = 0; index < cnt_max; ++index) {
        plant.setIndex(index);
        if (plant.isCrushed() || plant.isDisappeared()) {
            continue;
        }
        auto plant_offset_map = plant_offset_dict[plant.type()];
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
        return {text_list, min_distance};
    }

    uintptr_t offset = ReadMemory<uintptr_t>(g_mainobject + PLANT_OFFSET);
    plant.setIndex(target_index);
    text_list.append(QString("类型 : %1").arg(PLANT_NAME[plant.type()]));
    for (const auto& index : dict_index) {
        if (index == 0) {
            text_list.append(QString("%1 : %2").arg(plant_memory_dict[index].name).arg(target_index));
        } else {
            switch (plant_memory_dict[index].data_type) {
            case 0:
                text_list.append(QString("%1 : %2").arg(plant_memory_dict[index].name).arg(ReadMemory<int>(offset + target_index * 0x14c + plant_memory_dict[index].address)));
                break;
            case 1:
                text_list.append(QString("%1 : %2").arg(plant_memory_dict[index].name).arg(ReadMemory<float>(offset + target_index * 0x14c + plant_memory_dict[index].address)));
                break;
            case 2:
                text_list.append(QString("%1 : %2").arg(plant_memory_dict[index].name).arg(ReadMemory<bool>(offset + target_index * 0x14c + plant_memory_dict[index].address)));
                break;
            case 3:
                text_list.append(QString("%1 : %2").arg(plant_memory_dict[index].name).arg(ReadMemory<short>(offset + target_index * 0x14c + plant_memory_dict[index].address)));
                break;
            default:
                text_list.append(QString("%1 : %2").arg(plant_memory_dict[index].name).arg(ReadMemory<byte>(offset + target_index * 0x14c + plant_memory_dict[index].address)));
                break;
            }
        }
    }

    return {text_list, min_distance};
}

std::pair<QStringList, double> ZombieMemoryToString(const std::list<int>& dict_index, double find_threshold)
{
    QStringList text_list;
    int cnt_max = ZombieMemory::countMax();

    // 计算离鼠标最近的对象
    int target_index = 0;
    int mouse_x = PvZMouseX();
    int mouse_y = PvZMouseY();
    float offset_x = 0;
    float offset_y = 0;
    int zombie_type = 0;
    float min_distance = 0xffff;
    ZombieMemory zombie;
    for (int index = 0; index < cnt_max; ++index) {

        zombie.setIndex(index);
        if (zombie.isDead() || !zombie.isExist()) {
            continue;
        }
        auto zombie_offset_map = zombie_offset_dict[zombie.type()];
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
        return {text_list, min_distance};
    }

    uintptr_t offset = ReadMemory<uintptr_t>(g_mainobject + ZOMBIE_OFFSET);
    zombie.setIndex(target_index);
    text_list.append(QString("类型 : %1").arg(ZOMBIE_NAME[zombie.type()]));
    for (const auto& index : dict_index) {
        if (index == 0) { // 编号
            text_list.append(QString("%1 : %2").arg(zombie_memory_dict[index].name).arg(target_index));
        } else { // 其他
            switch (zombie_memory_dict[index].data_type) {
            case 0:
                text_list.append(QString("%1 : %2").arg(zombie_memory_dict[index].name).arg(ReadMemory<int>(offset + target_index * 0x15c + zombie_memory_dict[index].address)));
                break;
            case 1:
                text_list.append(QString("%1 : %2").arg(zombie_memory_dict[index].name).arg(ReadMemory<float>(offset + target_index * 0x15c + zombie_memory_dict[index].address)));
                break;
            case 2:
                text_list.append(QString("%1 : %2").arg(zombie_memory_dict[index].name).arg(ReadMemory<bool>(offset + target_index * 0x15c + zombie_memory_dict[index].address)));
                break;
            case 3:
                text_list.append(QString("%1 : %2").arg(zombie_memory_dict[index].name).arg(ReadMemory<short>(offset + target_index * 0x15c + zombie_memory_dict[index].address)));
                break;
            default:
                text_list.append(QString("%1 : %2").arg(zombie_memory_dict[index].name).arg(ReadMemory<byte>(offset + target_index * 0x15c + zombie_memory_dict[index].address)));
                break;
            }
        }
    }

    return {text_list, min_distance};
}

QStringList SeedMemoryToString()
{
    QStringList text_list;

    // 计算离鼠标最近的对象
    int target_index = -1;
    int mouse_x = PvZMouseX();
    int mouse_y = PvZMouseY();
    float offset_x = 0;
    float offset_y = 0;
    int seed_type = 0;
    float min_distance = 0xffff;
    SeedMemory seed;
    int cnt_max = seed.slotsCount();
    for (int index = 0; index < cnt_max; ++index) {
        seed.setIndex(index);
        if (mouse_x - seed.abscissa() > 0 && mouse_x - seed.abscissa() < seed.width() && mouse_y - seed.ordinate() > 0 && mouse_y - seed.ordinate() < seed.height()) {
            target_index = index;
            break;
        }
    }

    if (target_index == -1) {
        return text_list;
    }

    seed_type = seed.type();
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
    int place_type = 0;
    float min_distance = 0xffff;
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
