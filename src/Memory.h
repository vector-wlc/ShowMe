#pragma once
#pragma execution_character_set("utf-8")

#include <cstdint>
#include <initializer_list>
#include <qstring.h>
#include <vector>
#include <windows.h>

extern HWND g_hwnd;
extern HANDLE g_handle;
extern uintptr_t g_pvzbase;    //��ַ
extern uintptr_t g_mainobject; //��Ϸ�����ַ

#define PLANT_OFFSET 0xac
#define ZOMBIE_OFFSET 0x90

// ��ʼ���ڴ���Ϣ
// ���� true : ��ʼ���ɹ�
// ���� false : ��ʼ��ʧ�ܣ���������Ϊ��Ϸ�ѹرջ��߲���ս������
bool IsDisplayed();

// ��ȡ�ڴ溯��
// ReadMemory<data type>(address);
// �˺����з���ֵ����Ҫ�������ն�ȡ���
// ʹ��ʾ��
// ReadMemory<int>(0x6A9EC0) -----��ȡ��ַ
// ReadMemory<int>(0x6A9EC0, 0x768) ------��ȡ��ǰ��Ϸ��Ϣ�Ͷ���
template <typename T, typename... Args>
T ReadMemory(Args... args)
{
    std::initializer_list<uintptr_t> lst = {static_cast<uintptr_t>(args)...};
    uintptr_t buff = 0;
    T result = T();
    for (auto it = lst.begin(); it != lst.end(); ++it)
        if (it != lst.end() - 1)
            ReadProcessMemory(g_handle, (const void*)(buff + *it), &buff, sizeof(buff), nullptr);
        else
            ReadProcessMemory(g_handle, (const void*)(buff + *it), &result, sizeof(result), nullptr);
    return result;
}

// ########## memory classes ###############

// �����ڴ���
class AbstractMemory {
protected:
    uintptr_t offset; // ��ַƫ��
    int index;        // ��������/ջλ/���...

public:
    AbstractMemory() { offset = index = 0; }
    // ���ö�������
    void setIndex(int _index) { index = _index; }
    // ���µõ���ַƫ����Ϣ
    virtual void getOffset() = 0;
};

// ��Ƭ�ڴ���Ϣ��
class SeedMemory : public AbstractMemory {
public:
    SeedMemory(int _index);
    SeedMemory();

    virtual void getOffset();

    // ���ؿ�����Ŀ
    int slotsCount()
    {
        return ReadMemory<int>(offset + 0x24);
    }

    // �жϿ�Ƭ�Ƿ����
    bool isUsable()
    {
        return ReadMemory<bool>(offset + 0x70 + 0x50 * index);
    }

    // ���ؿ�Ƭ��ȴ
    int CD()
    {
        return ReadMemory<int>(offset + 0x4C + index * 0x50);
    }

    // ���ؿ�Ƭ����ȴʱ��
    int initialCD()
    {
        return ReadMemory<int>(offset + 0x50 + index * 0x50);
    }

    // ����ģ���߿�Ƭ����
    int imitatorType()
    {
        return ReadMemory<int>(offset + 0x60 + index * 0x50);
    }

    // ���ؿ�Ƭ����
    int type()
    {
        return ReadMemory<int>(offset + 0x5C + index * 0x50);
    }

    // ���ؿ�Ƭ������
    int abscissa()
    {
        return ReadMemory<int>(offset + 0xc + 0x24 + index * 0x50);
    }

    // ���ؿ�Ƭ������
    int ordinate()
    {
        return ReadMemory<int>(offset + 0x10 + 0x24 + index * 0x50);
    }

    // ���ؿ�Ƭ�ж����
    int width()
    {
        return ReadMemory<int>(offset + 0x14 + 0x24 + index * 0x50);
    }

    // ���ؿ�Ƭ�ж��߶�
    int height()
    {
        return ReadMemory<int>(offset + 0x18 + 0x24 + index * 0x50);
    }
};

class PlantMemory : public AbstractMemory {
public:
    PlantMemory(int _index);
    PlantMemory();

    virtual void getOffset();

    // ���ص�ǰ���ֲ����Ŀ
    static int countMax()
    {
        return ReadMemory<int>(g_mainobject + 0xB0);
    }

    // ������һ��ֲ���ջλ/���/��������
    static int nextIndex()
    {
        return ReadMemory<int>(g_mainobject + 0xB8);
    }

    // ����ֲ�������� ��ֵ��Χ��[0,5]
    int row()
    {
        return ReadMemory<int>(offset + 0x1C + 0x14C * index);
    }

    // ����ֲ�������� ��ֵ��Χ��[0,8]
    int col()
    {
        return ReadMemory<int>(offset + 0x28 + 0x14C * index);
    }

    // �ж�ֲ���Ƿ���ʧ
    bool isDisappeared()
    {
        return ReadMemory<bool>(offset + 0x141 + 0x14C * index);
    }

    // �ж�ֲ���Ƿ�ѹ��
    bool isCrushed()
    {
        return ReadMemory<bool>(offset + 0x142 + 0x14C * index);
    }

    // ����ֲ������ ��ͼ��˳��һ������0��ʼ
    int type()
    {
        return ReadMemory<int>(offset + 0x24 + 0x14C * index);
    }

    // ����ֲ������� ��ֵ��Χ��[0,800]
    int abscissa()
    {
        return ReadMemory<int>(offset + 0x8 + 0x14C * index);
    }

    // ����ֲ��������
    int ordinate()
    {
        return ReadMemory<int>(offset + 0xC + 0x14C * index);
    }

    // �����ж����
    int hurtWidth()
    {
        return ReadMemory<int>(offset + 0x10 + 0x14C * index);
    }

    // �����ж��߶�
    int hurtHeight()
    {
        return ReadMemory<int>(offset + 0x14 + 0x14C * index);
    }

    // ����ֲ��Ѫ��
    int hp()
    {
        return ReadMemory<int>(offset + 0x40 + 0x14C * index);
    }

    // ����ֲ��Ѫ������
    int hpMax()
    {
        return ReadMemory<int>(offset + 0x44 + 0x14C * index);
    }

    // ��Ч����ʱ
    int activeCountdown()
    {
        return ReadMemory<int>(offset + 0x50 + 0x14C * index);
    }

    // �����ӵ�����ʱ���õ���ʱ�����Ƿ��н�ʬһֱ���ϼ�С������
    int bulletCountdown()
    {
        return ReadMemory<int>(offset + 0x58 + 0x14C * index);
    }

    // �ӵ����䵹��ʱ���õ���ʱֻ���н�ʬʱ��һֱ���ϼ�С������
    int countdownBullet()
    {
        return ReadMemory<int>(offset + 0x90 + 0x14C * index);
    }

    // ����ֲ���״̬
    // 35������
    // 36������װ��
    // 37��׼������
    // 38�����ڷ���
    int state()
    {
        return ReadMemory<int>(offset + 0x3C + 0x14C * index);
    }

    // ֲ�����Ե���ʱ
    int stateCountdown()
    {
        return ReadMemory<int>(offset + 0x54 + 0x14C * index);
    }
};

class ZombieMemory : public AbstractMemory {
public:
    ZombieMemory(int _index);
    ZombieMemory();

    virtual void getOffset();

    // ���ؽ�ʬ�����Ŀ
    static int countMax()
    {
        return ReadMemory<int>(g_mainobject + 0x94);
    }

    // �жϽ�ʬ�Ƿ����
    bool isExist()
    {
        return ReadMemory<short>(offset + 0x15A + 0x15C * index);
    }

    // ���ؽ�ʬ�������� ��Χ��[0,5]
    int row()
    {
        return ReadMemory<int>(offset + 0x1C + 0x15C * index);
    }

    // ���ؽ�ʬ���ں����� ��Χ��[0,800]
    float abscissa()
    {
        return ReadMemory<float>(offset + 0x2C + 0x15C * index);
    }

    // ���ؽ�ʬ������
    float ordinate()
    {
        return ReadMemory<float>(offset + 0x30 + 0x15C * index);
    }

    // �����ж����
    int hurtWidth()
    {
        return ReadMemory<int>(offset + 0x94 + 0x15C * index);
    }

    // �����ж��߶�
    int hurtHeight()
    {
        return ReadMemory<int>(offset + 0x98 + 0x15C * index);
    }

    int standState()
    {
        return ReadMemory<int>(offset + 0x6C + 0x15C * index);
    }

    // ���ؽ�ʬ���� ��ͼ��˳��һ������0��ʼ
    int type()
    {
        return ReadMemory<int>(offset + 0x24 + 0x15C * index);
    }

    // ���ؽ�ʬ���嵱ǰѪ��
    int hp()
    {
        return ReadMemory<int>(offset + 0xC8 + 0x15C * index);
    }

    // ���ؽ�ʬһ����Ʒ��ǰѪ��
    int oneHp()
    {
        return ReadMemory<int>(offset + 0xD0 + 0x15C * index);
    }

    // ���ؽ�ʬ������ƷѪ��
    int twoHp()
    {
        return ReadMemory<int>(offset + 0xDC + 0x15C * index);
    }

    // �жϽ�ʬ�Ƿ��ʳ
    bool isEat()
    {
        return ReadMemory<bool>(offset + 0x51 + 0x15C * index);
    }

    // ���ؽ�ʬ״̬
    int state()
    {
        return ReadMemory<int>(offset + 0x28 + 0x15C * index);
    }

    // �жϽ�ʬ�Ƿ�����
    bool isDead()
    {
        return state() == 1 || state() == 2 || state() == 3;
    }

    // �жϾ����Ƿ�ٴ�
    bool isHammering()
    {
        return state() == 70;
    }

    // �жϽ�ʬ�Ƿ�����
    bool isStealth()
    {
        return ReadMemory<bool>(offset + 0x18 + 0x15C * index);
    }

    // �仯��(ǰ���������ͼ��ٵı����ȵ�ǰ���ٶ�)
    float speed()
    {
        return ReadMemory<float>(offset + 0x34 + 0x15C * index);
    }

    // ��ʬ�Ѵ���ʱ��
    int existTime()
    {
        return ReadMemory<int>(offset + 0x60 + 0x15C * index);
    }

    // ��ʬ���Ե���ʱ
    int stateCountdown()
    {
        return ReadMemory<int>(offset + 0x68 + 0x15C * index);
    }

    // �жϽ�ʬ�Ƿ���ʧ
    bool isDisappeared()
    {
        return ReadMemory<bool>(offset + 0xEC + 0x15C * index);
    }

    // ��ʬ�е��ж��ĺ�����
    int bulletAbscissa()
    {
        return ReadMemory<int>(offset + 0x8C + 0x15C * index);
    }

    // ��ʬ�е��ж���������
    int bulletOrdinate()
    {
        return ReadMemory<int>(offset + 0x90 + 0x15C * index);
    }

    // ��ʬ�����ж��ĺ�����
    int attackAbscissa()
    {
        return ReadMemory<int>(offset + 0x9C + 0x15C * index);
    }

    // ��ʬ�����ж���������
    int attackOrdinate()
    {
        return ReadMemory<int>(offset + 0xA0 + 0x15C * index);
    }

    // ��ʬ���ٵ���ʱ
    int slowCountdown()
    {
        return ReadMemory<int>(offset + 0xAC + 0x15C * index);
    }

    // ��ʬ���͹̶�����ʱ
    int fixationCountdown()
    {
        return ReadMemory<int>(offset + 0xB0 + 0x15C * index);
    }

    // ��ʬ���ᵹ��ʱ
    int freezeCountdown()
    {
        return ReadMemory<int>(offset + 0xB4 + 0x15C * index);
    }
};

class PlaceMemory : public AbstractMemory {
public:
    PlaceMemory(int _index);
    PlaceMemory();

    virtual void getOffset();

    // ���س�����Ʒ�����Ŀ
    // ���������� Ĺ�� ���ӵ�
    static int countMax()
    {
        return ReadMemory<int>(g_mainobject + 0x120);
    }

    // ���ظø�����Ʒ������
    int type()
    {
        return ReadMemory<int>(offset + 0x8 + 0xEC * index);
    }

    // ������Ʒ������ ��Χ��[0,5]
    int row()
    {
        return ReadMemory<int>(offset + 0x14 + 0xEC * index);
    }

    // ������Ʒ������ ��Χ��[0,8]
    int col()
    {
        return ReadMemory<int>(offset + 0x10 + 0xEC * index);
    }

    int value()
    {
        return ReadMemory<int>(offset + 0x18 + 0xEC * index);
    }
    // �ж���Ʒ�Ƿ����
    bool isExist()
    {
        return ReadMemory<short>(offset + 0xEA + 0xEC * index);
    }

    // �ж���Ʒ�Ƿ���ʧ
    bool isDisappeared()
    {
        return ReadMemory<bool>(offset + 0x20 + 0xEC * index);
    }
};

class ItemMemory : public AbstractMemory {
public:
    ItemMemory(int _index);
    ItemMemory();

    virtual void getOffset();

    // ������Ҫ�ռ���Ʒ����Ŀ
    // ������� ��ʯ ��е�
    static int count()
    {
        return ReadMemory<int>(g_mainobject + 0xF4);
    }

    // ������Ҫ�ռ���Ʒ�������Ŀ
    static int countMax()
    {
        return ReadMemory<int>(g_mainobject + 0xE8);
    }

    // �ж���Ʒ�Ƿ���ʧ
    bool isDisappeared()
    {
        return ReadMemory<bool>(offset + 0x38 + 0xD8 * index);
    }

    // �ж���Ʒ�Ƿ��ռ�
    bool isCollected()
    {
        return ReadMemory<bool>(offset + 0x50 + 0xD8 * index);
    }

    // ������Ʒ������
    float abscissa()
    {
        return ReadMemory<float>(offset + 0x24 + 0xD8 * index);
    }

    // ������Ʒ������
    float ordinate()
    {
        return ReadMemory<float>(offset + 0x28 + 0xD8 * index);
    }
};

// ########## memory functions ###############

// �������������
inline int MouseRow()
{
    return ReadMemory<int>(g_mainobject + 0x13C, 0x28) + 1;
}

// �������������
inline float MouseCol()
{
    return 1.0 * ReadMemory<int>(g_pvzbase + 0x320, 0xE0) / 80;
}

// ������������
inline int PvZMouseX()
{
    return ReadMemory<int>(g_pvzbase + 0x320, 0xE0);
}

// �������������
inline int PvZMouseY()
{
    return ReadMemory<int>(g_pvzbase + 0x320, 0xE4);
}

inline bool IsMouseInPvZ()
{
    return ReadMemory<bool>(g_pvzbase + 0x320, 0xdc);
}

//��ȡ��Ϸ��Ϣ
//1: ������, 2: ѡ��, 3: ������Ϸ/ս��, 4: ��ʬ����, 7: ģʽѡ��.
inline int GameUi()
{
    return ReadMemory<int>(g_pvzbase + 0x7FC);
}

//��ȡ��Ϸ��ǰ��Ϸʱ��
inline int GameClock()
{
    return ReadMemory<int>(g_mainobject + 0x5568);
}

//����ˢ��Ѫ��
inline int RefreshHp()
{
    return ReadMemory<int>(g_mainobject + 0x5594);
}

//����ˢ�µ���ʱ
inline int Countdown()
{
    return ReadMemory<int>(g_mainobject + 0x559c);
}

//���ش�ˢ�µ���ʱ
inline int HugeWaveCountdown()
{
    return ReadMemory<int>(g_mainobject + 0x55A4);
}

//������ˢ�²���
inline int NowWave()
{
    return ReadMemory<int>(g_mainobject + 0x557c);
}

//�����ܲ���
inline int TotalWave()
{
    return ReadMemory<int>(g_mainobject + 0x5564);
}

//���س�ʼˢ�µ���ʱ
inline int InitialCountdown()
{
    return ReadMemory<int>(g_mainobject + 0x55A0);
}

//����һ�еı������� ��Χ��[0,800]
//ʹ��ʾ����
//IceAbscissa(0)------�õ���һ�еı�������
inline int IceAbscissa(int i)
{
    return ReadMemory<int>(g_mainobject + 0x60C + 4 * i);
}

// ֲ������
enum PlantType {
    PEASHOOTER = 0, // �㶹����
    SUNFLOWER,      // ���տ�
    CHERRY_BOMB,    // ӣ��ը��
    WALL_NUT,       // ���
    POTATO_MINE,    // ��������
    SNOW_PEA,       // ��������
    CHOMPER,        // ���컨
    REPEATER,       // ˫������
    PUFF_SHROOM,    // С�繽
    SUN_SHROOM,     // ���⹽
    FUME_SHROOM,    // ���繽
    GRAVE_BUSTER,   // Ĺ��������
    HYPNO_SHROOM,   // �Ȼ�
    SCAREDY_SHROOM, // ��С��
    ICE_SHROOM,     // ������
    DOOM_SHROOM,    // ����
    LILY_PAD,       // ��Ҷ
    SQUASH,         // ����
    THREEPEATER,    // ��������
    TANGLE_KELP,    // ���ƺ���
    JALAPENO,       // ������
    SPIKEWEED,      // �ش�
    TORCHWOOD,      // �����׮
    TALL_NUT,       // �߼��
    SEA_SHROOM,     // ˮ����
    PLANTERN,       // ·�ƻ�
    CACTUS,         // ������
    BLOVER,         // ��Ҷ��
    SPLIT_PEA,      // �Ѽ�����
    STARFRUIT,      // ����
    PUMPKIN,        // �Ϲ�ͷ
    MAGNET_SHROOM,  // ������
    CABBAGE_PULT,   // ���Ĳ�Ͷ��
    FLOWER_POT,     // ����
    KERNEL_PULT,    // ����Ͷ��
    COFFEE_BEAN,    // ���ȶ�
    GARLIC,         // ����
    UMBRELLA_LEAF,  // Ҷ�ӱ���ɡ
    MARIGOLD,       // ��յ��
    MELON_PULT,     // ����Ͷ��
    GATLING_PEA,    // ��ǹ����
    TWIN_SUNFLOWER, // ˫�����տ�
    GLOOM_SHROOM,   // ������
    CATTAIL,        // ����
    WINTER_MELON,   // ������Ͷ��
    GOLD_MAGNET,    // �����
    SPIKEROCK,      // �ش���
    COB_CANNON,     // ���׼�ũ��

    // ģ�������� + M
    M_PEASHOOTER,     // �㶹����
    M_SUNFLOWER,      // ���տ�
    M_CHERRY_BOMB,    // ӣ��ը��
    M_WALL_NUT,       // ���
    M_POTATO_MINE,    // ��������
    M_SNOW_PEA,       // ��������
    M_CHOMPER,        // ���컨
    M_REPEATER,       // ˫������
    M_PUFF_SHROOM,    // С�繽
    M_SUN_SHROOM,     // ���⹽
    M_FUME_SHROOM,    // ���繽
    M_GRAVE_BUSTER,   // Ĺ��������
    M_HYPNO_SHROOM,   // �Ȼ�
    M_SCAREDY_SHROOM, // ��С��
    M_ICE_SHROOM,     // ������
    M_DOOM_SHROOM,    // ����
    M_LILY_PAD,       // ��Ҷ
    M_SQUASH,         // ����
    M_THREEPEATER,    // ��������
    M_TANGLE_KELP,    // ���ƺ���
    M_JALAPENO,       // ������
    M_SPIKEWEED,      // �ش�
    M_TORCHWOOD,      // �����׮
    M_TALL_NUT,       // �߼��
    M_SEA_SHROOM,     // ˮ����
    M_PLANTERN,       // ·�ƻ�
    M_CACTUS,         // ������
    M_BLOVER,         // ��Ҷ��
    M_SPLIT_PEA,      // �Ѽ�����
    M_STARFRUIT,      // ����
    M_PUMPKIN,        // �Ϲ�ͷ
    M_MAGNET_SHROOM,  // ������
    M_CABBAGE_PULT,   // ���Ĳ�Ͷ��
    M_FLOWER_POT,     // ����
    M_KERNEL_PULT,    // ����Ͷ��
    M_COFFEE_BEAN,    // ���ȶ�
    M_GARLIC,         // ����
    M_UMBRELLA_LEAF,  // Ҷ�ӱ���ɡ
    M_MARIGOLD,       // ��յ��
    M_MELON_PULT,     // ����Ͷ��
};

enum ZombieType {
    ZOMBIE = 0,             // �ս�
    FLAG_ZOMBIE,            // ����
    CONEHEAD_ZOMBIE,        // ·��
    POLE_VAULTING_ZOMBIE,   // �Ÿ�
    BUCKETHEAD_ZOMBIE,      // ��Ͱ
    NEWSPAPER_ZOMBIE,       // ����
    SCREEN_DOOR_ZOMBIE,     // ����
    FOOTBALL_ZOMBIE,        // ���
    DANCING_ZOMBIE,         // ����
    BACKUP_DANCER,          // ����
    DUCKY_TUBE_ZOMBIE,      // Ѽ��
    SNORKEL_ZOMBIE,         // Ǳˮ
    ZOMBONI,                // ����
    ZOMBIE_BOBSLED_TEAM,    // ѩ��
    DOLPHIN_RIDER_ZOMBIE,   // ����
    JACK_IN_THE_BOX_ZOMBIE, // С��
    BALLOON_ZOMBIE,         // ����
    DIGGER_ZOMBIE,          // ��
    POGO_ZOMBIE,            // ����
    ZOMBIE_YETI,            // ѩ��
    BUNGEE_ZOMBIE,          // �ļ�
    LADDER_ZOMBIE,          // ����
    CATAPULT_ZOMBIE,        // Ͷ��
    GARGANTUAR,             // ����
    IMP,                    // С��
    DR_ZOMBOSS,             // ����
    GIGA_GARGANTUAR = 32    // ����
};
