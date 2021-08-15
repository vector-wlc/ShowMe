#include "MemoryString.h"
#include <math.h>

const std::vector<QString> PLANT_NAME = {
    "�㶹����",
    "���տ�",
    "ӣ��ը��",
    "���",
    "������",
    "��������",
    "���컨",
    "˫������",
    "С�繽",
    "���⹽",
    "���繽",
    "Ĺ��������",
    "�Ȼ�",
    "��С��",
    "������",
    "����",
    "˯��",
    "����",
    "��������",
    "���ƺ���",
    "������",
    "�ش�",
    "�����׮",
    "�߼��",
    "��Ģ��",
    "·�ƻ�",
    "������",
    "��Ҷ��",
    "�Ѽ�����",
    "����",
    "�Ϲ�ͷ",
    "������",
    "���Ĳ�Ͷ��",
    "����",
    "����Ͷ��",
    "���ȶ�",
    "����",
    "Ҷ�ӱ���ɡ",
    "��յ��",
    "����Ͷ��",
    "��ǹ����",
    "˫�����տ�",
    "����Ģ��",
    "����",
    "����",
    "�����",
    "�ش���",
    "���׼�ũ��",
    "ģ����",
};

const std::vector<QString> ZOMBIE_NAME = {
    "�ս�",
    "����",
    "·��",
    "�Ÿ�",
    "��Ͱ",
    "����",
    "����",
    "���",
    "����",
    "����",
    "Ѽ��",
    "Ǳˮ",
    "����",
    "ѩ��",
    "����",
    "С��",
    "����",
    "��",
    "����",
    "ѩ��",
    "�ļ�",
    "����",
    "Ͷ��",
    "����",
    "С��",
    "����",
    "�㶹",
    "���",
    "����",
    "��ǹ",
    "����",
    "��ǽ",
    "����",
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

    // �������������Ķ���
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
    text_list.append(QString("���� : %1").arg(PLANT_NAME[plant.type()]));
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

    // �������������Ķ���
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
    text_list.append(QString("���� : %1").arg(ZOMBIE_NAME[zombie.type()]));
    for (const auto& index : dict_index) {
        if (index == 0) { // ���
            text_list.append(QString("%1 : %2").arg(zombie_memory_dict[index].name).arg(target_index));
        } else { // ����
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

    // �������������Ķ���
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
    QString str = "���� : ";
    if (seed_type == 48) {
        seed_type = seed.imitatorType();
        str += "ģ��";
    }
    str += PLANT_NAME[seed_type];
    text_list.append(str);
    text_list.append(QString("��ȴ : %1 / %2").arg(seed.CD()).arg(seed.initialCD()));

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
    text_list.append(QString("���� : ����"));
    text_list.append(QString("��ʧ����ʱ : %1").arg(place.value()));

    return text_list;
}

enum MemoryIndex {
    PLANT_STACK,       // ֲ��ջλ
    ZOMBIE_STACK,      // ��ʬջλ
    ICE_ROAD_ABSCISSA, // ����������

    REFRESH_COUNTDOWN,      // ˢ�µ���ʱ
    HUGE_REFRESH_COUNTDOWN, // ��ˢ�µ���ʱ
    REFRESH_HP,             // ˢ��Ѫ��
    WAVE,                   // ��ǰ�Ĳ���
    CLOCK                   // ��ʱϵͳ
};

QStringList OtherMemoryToString()
{
    QStringList text_list;
    QString text;
    // ֲ��ջλ
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

    // ��ʬջλ
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

    // ����
    for (int row = 0; row < 5; ++row) {
        text += QString("%1 : %2 | ").arg(row + 1).arg(IceAbscissa(row));
    }
    text += QString("%1 : %2").arg(6).arg(IceAbscissa(5));
    text_list.append(text);

    // ˢ�µ���ʱ
    text = QString("%1 / %2").arg(Countdown()).arg(InitialCountdown());
    text_list.append(text);

    // ��ˢ�µ���ʱ
    text = QString("%1").arg(HugeWaveCountdown());
    text_list.append(text);

    // ˢ��Ѫ��
    text = QString("%1").arg(RefreshHp());
    text_list.append(text);

    // ��ǰ����
    text = QString("%1 / %2").arg(NowWave()).arg(TotalWave());
    text_list.append(text);

    // ��ʱϵͳ
    text = QString("%1").arg(GameClock());
    text_list.append(text);

    return text_list;
}
