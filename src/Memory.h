#pragma once
#pragma execution_character_set("utf-8")

#include <cstdint>
#include <initializer_list>
#include <qstring.h>
#include <vector>
#include <windows.h>

extern HWND g_hwnd;
extern HANDLE g_handle;
extern uintptr_t g_pvzbase;    //基址
extern uintptr_t g_mainobject; //游戏对象地址

#define PLANT_OFFSET 0xac
#define ZOMBIE_OFFSET 0x90

// 初始化内存信息
// 返回 true : 初始化成功
// 返回 false : 初始化失败，可能是因为游戏已关闭或者不在战斗界面
bool IsDisplayed();

// 读取内存函数
// ReadMemory<data type>(address);
// 此函数有返回值，需要变量接收读取结果
// 使用示例
// ReadMemory<int>(0x6A9EC0) -----读取基址
// ReadMemory<int>(0x6A9EC0, 0x768) ------读取当前游戏信息和对象
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

// 抽象内存类
class AbstractMemory {
protected:
    uintptr_t offset; // 地址偏移
    int index;        // 对象序列/栈位/序号...

public:
    AbstractMemory() { offset = index = 0; }
    // 设置对象序列
    void setIndex(int _index) { index = _index; }
    // 重新得到地址偏移信息
    virtual void getOffset() = 0;
};

// 卡片内存信息类
class SeedMemory : public AbstractMemory {
public:
    SeedMemory(int _index);
    SeedMemory();

    virtual void getOffset();

    // 返回卡槽数目
    int slotsCount()
    {
        return ReadMemory<int>(offset + 0x24);
    }

    // 判断卡片是否可用
    bool isUsable()
    {
        return ReadMemory<bool>(offset + 0x70 + 0x50 * index);
    }

    // 返回卡片冷却
    int CD()
    {
        return ReadMemory<int>(offset + 0x4C + index * 0x50);
    }

    // 返回卡片总冷却时间
    int initialCD()
    {
        return ReadMemory<int>(offset + 0x50 + index * 0x50);
    }

    // 返回模仿者卡片类型
    int imitatorType()
    {
        return ReadMemory<int>(offset + 0x60 + index * 0x50);
    }

    // 返回卡片类型
    int type()
    {
        return ReadMemory<int>(offset + 0x5C + index * 0x50);
    }

    // 返回卡片横坐标
    int abscissa()
    {
        return ReadMemory<int>(offset + 0xc + 0x24 + index * 0x50);
    }

    // 返回卡片纵坐标
    int ordinate()
    {
        return ReadMemory<int>(offset + 0x10 + 0x24 + index * 0x50);
    }

    // 返回卡片判定宽度
    int width()
    {
        return ReadMemory<int>(offset + 0x14 + 0x24 + index * 0x50);
    }

    // 返回卡片判定高度
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

    // 返回当前最大植物数目
    static int countMax()
    {
        return ReadMemory<int>(g_mainobject + 0xB0);
    }

    // 返回下一个植物的栈位/编号/对象序列
    static int nextIndex()
    {
        return ReadMemory<int>(g_mainobject + 0xB8);
    }

    // 返回植物所在行 数值范围：[0,5]
    int row()
    {
        return ReadMemory<int>(offset + 0x1C + 0x14C * index);
    }

    // 返回植物所在列 数值范围：[0,8]
    int col()
    {
        return ReadMemory<int>(offset + 0x28 + 0x14C * index);
    }

    // 判断植物是否消失
    bool isDisappeared()
    {
        return ReadMemory<bool>(offset + 0x141 + 0x14C * index);
    }

    // 判断植物是否被压扁
    bool isCrushed()
    {
        return ReadMemory<bool>(offset + 0x142 + 0x14C * index);
    }

    // 返回植物类型 与图鉴顺序一样，从0开始
    int type()
    {
        return ReadMemory<int>(offset + 0x24 + 0x14C * index);
    }

    // 返回植物横坐标 数值范围：[0,800]
    int abscissa()
    {
        return ReadMemory<int>(offset + 0x8 + 0x14C * index);
    }

    // 返回植物纵坐标
    int ordinate()
    {
        return ReadMemory<int>(offset + 0xC + 0x14C * index);
    }

    // 受伤判定宽度
    int hurtWidth()
    {
        return ReadMemory<int>(offset + 0x10 + 0x14C * index);
    }

    // 受伤判定高度
    int hurtHeight()
    {
        return ReadMemory<int>(offset + 0x14 + 0x14C * index);
    }

    // 返回植物血量
    int hp()
    {
        return ReadMemory<int>(offset + 0x40 + 0x14C * index);
    }

    // 返回植物血量上限
    int hpMax()
    {
        return ReadMemory<int>(offset + 0x44 + 0x14C * index);
    }

    // 生效倒计时
    int activeCountdown()
    {
        return ReadMemory<int>(offset + 0x50 + 0x14C * index);
    }

    // 发射子弹倒计时，该倒计时不论是否有僵尸一直不断减小并重置
    int bulletCountdown()
    {
        return ReadMemory<int>(offset + 0x58 + 0x14C * index);
    }

    // 子弹发射倒计时，该倒计时只在有僵尸时才一直不断减小并重置
    int countdownBullet()
    {
        return ReadMemory<int>(offset + 0x90 + 0x14C * index);
    }

    // 返回植物的状态
    // 35：空炮
    // 36：正在装填
    // 37：准备就绪
    // 38：正在发射
    int state()
    {
        return ReadMemory<int>(offset + 0x3C + 0x14C * index);
    }

    // 植物属性倒计时
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

    // 返回僵尸最大数目
    static int countMax()
    {
        return ReadMemory<int>(g_mainobject + 0x94);
    }

    // 判断僵尸是否存在
    bool isExist()
    {
        return ReadMemory<short>(offset + 0x15A + 0x15C * index);
    }

    // 返回僵尸所在行数 范围：[0,5]
    int row()
    {
        return ReadMemory<int>(offset + 0x1C + 0x15C * index);
    }

    // 返回僵尸所在横坐标 范围：[0,800]
    float abscissa()
    {
        return ReadMemory<float>(offset + 0x2C + 0x15C * index);
    }

    // 返回僵尸纵坐标
    float ordinate()
    {
        return ReadMemory<float>(offset + 0x30 + 0x15C * index);
    }

    // 受伤判定宽度
    int hurtWidth()
    {
        return ReadMemory<int>(offset + 0x94 + 0x15C * index);
    }

    // 受伤判定高度
    int hurtHeight()
    {
        return ReadMemory<int>(offset + 0x98 + 0x15C * index);
    }

    int standState()
    {
        return ReadMemory<int>(offset + 0x6C + 0x15C * index);
    }

    // 返回僵尸类型 与图鉴顺序一样，从0开始
    int type()
    {
        return ReadMemory<int>(offset + 0x24 + 0x15C * index);
    }

    // 返回僵尸本体当前血量
    int hp()
    {
        return ReadMemory<int>(offset + 0xC8 + 0x15C * index);
    }

    // 返回僵尸一类饰品当前血量
    int oneHp()
    {
        return ReadMemory<int>(offset + 0xD0 + 0x15C * index);
    }

    // 返回僵尸二类饰品血量
    int twoHp()
    {
        return ReadMemory<int>(offset + 0xDC + 0x15C * index);
    }

    // 判断僵尸是否啃食
    bool isEat()
    {
        return ReadMemory<bool>(offset + 0x51 + 0x15C * index);
    }

    // 返回僵尸状态
    int state()
    {
        return ReadMemory<int>(offset + 0x28 + 0x15C * index);
    }

    // 判断僵尸是否死亡
    bool isDead()
    {
        return state() == 1 || state() == 2 || state() == 3;
    }

    // 判断巨人是否举锤
    bool isHammering()
    {
        return state() == 70;
    }

    // 判断僵尸是否隐形
    bool isStealth()
    {
        return ReadMemory<bool>(offset + 0x18 + 0x15C * index);
    }

    // 变化量(前进的舞王和减速的冰车等的前进速度)
    float speed()
    {
        return ReadMemory<float>(offset + 0x34 + 0x15C * index);
    }

    // 僵尸已存在时间
    int existTime()
    {
        return ReadMemory<int>(offset + 0x60 + 0x15C * index);
    }

    // 僵尸属性倒计时
    int stateCountdown()
    {
        return ReadMemory<int>(offset + 0x68 + 0x15C * index);
    }

    // 判断僵尸是否消失
    bool isDisappeared()
    {
        return ReadMemory<bool>(offset + 0xEC + 0x15C * index);
    }

    // 僵尸中弹判定的横坐标
    int bulletAbscissa()
    {
        return ReadMemory<int>(offset + 0x8C + 0x15C * index);
    }

    // 僵尸中弹判定的纵坐标
    int bulletOrdinate()
    {
        return ReadMemory<int>(offset + 0x90 + 0x15C * index);
    }

    // 僵尸攻击判定的横坐标
    int attackAbscissa()
    {
        return ReadMemory<int>(offset + 0x9C + 0x15C * index);
    }

    // 僵尸攻击判定的纵坐标
    int attackOrdinate()
    {
        return ReadMemory<int>(offset + 0xA0 + 0x15C * index);
    }

    // 僵尸减速倒计时
    int slowCountdown()
    {
        return ReadMemory<int>(offset + 0xAC + 0x15C * index);
    }

    // 僵尸黄油固定倒计时
    int fixationCountdown()
    {
        return ReadMemory<int>(offset + 0xB0 + 0x15C * index);
    }

    // 僵尸冻结倒计时
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

    // 返回场景物品最大数目
    // 包括：弹坑 墓碑 罐子等
    static int countMax()
    {
        return ReadMemory<int>(g_mainobject + 0x120);
    }

    // 返回该格子物品的类型
    int type()
    {
        return ReadMemory<int>(offset + 0x8 + 0xEC * index);
    }

    // 返回物品的行数 范围：[0,5]
    int row()
    {
        return ReadMemory<int>(offset + 0x14 + 0xEC * index);
    }

    // 返回物品的列数 范围：[0,8]
    int col()
    {
        return ReadMemory<int>(offset + 0x10 + 0xEC * index);
    }

    int value()
    {
        return ReadMemory<int>(offset + 0x18 + 0xEC * index);
    }
    // 判断物品是否存在
    bool isExist()
    {
        return ReadMemory<short>(offset + 0xEA + 0xEC * index);
    }

    // 判断物品是否消失
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

    // 返回需要收集物品的数目
    // 包括金币 钻石 礼盒等
    static int count()
    {
        return ReadMemory<int>(g_mainobject + 0xF4);
    }

    // 返回需要收集物品的最大数目
    static int countMax()
    {
        return ReadMemory<int>(g_mainobject + 0xE8);
    }

    // 判断物品是否消失
    bool isDisappeared()
    {
        return ReadMemory<bool>(offset + 0x38 + 0xD8 * index);
    }

    // 判断物品是否被收集
    bool isCollected()
    {
        return ReadMemory<bool>(offset + 0x50 + 0xD8 * index);
    }

    // 返回物品横坐标
    float abscissa()
    {
        return ReadMemory<float>(offset + 0x24 + 0xD8 * index);
    }

    // 返回物品纵坐标
    float ordinate()
    {
        return ReadMemory<float>(offset + 0x28 + 0xD8 * index);
    }
};

// ########## memory functions ###############

// 返回鼠标所在行
inline int MouseRow()
{
    return ReadMemory<int>(g_mainobject + 0x13C, 0x28) + 1;
}

// 返回鼠标所在列
inline float MouseCol()
{
    return 1.0 * ReadMemory<int>(g_pvzbase + 0x320, 0xE0) / 80;
}

// 返回鼠标横坐标
inline int PvZMouseX()
{
    return ReadMemory<int>(g_pvzbase + 0x320, 0xE0);
}

// 返回鼠标纵坐标
inline int PvZMouseY()
{
    return ReadMemory<int>(g_pvzbase + 0x320, 0xE4);
}

inline bool IsMouseInPvZ()
{
    return ReadMemory<bool>(g_pvzbase + 0x320, 0xdc);
}

//获取游戏信息
//1: 主界面, 2: 选卡, 3: 正常游戏/战斗, 4: 僵尸进屋, 7: 模式选择.
inline int GameUi()
{
    return ReadMemory<int>(g_pvzbase + 0x7FC);
}

//获取游戏当前游戏时钟
inline int GameClock()
{
    return ReadMemory<int>(g_mainobject + 0x5568);
}

//返回刷新血量
inline int RefreshHp()
{
    return ReadMemory<int>(g_mainobject + 0x5594);
}

//返回刷新倒计时
inline int Countdown()
{
    return ReadMemory<int>(g_mainobject + 0x559c);
}

//返回大波刷新倒计时
inline int HugeWaveCountdown()
{
    return ReadMemory<int>(g_mainobject + 0x55A4);
}

//返回已刷新波数
inline int NowWave()
{
    return ReadMemory<int>(g_mainobject + 0x557c);
}

//返回总波数
inline int TotalWave()
{
    return ReadMemory<int>(g_mainobject + 0x5564);
}

//返回初始刷新倒计时
inline int InitialCountdown()
{
    return ReadMemory<int>(g_mainobject + 0x55A0);
}

//返回一行的冰道坐标 范围：[0,800]
//使用示例：
//IceAbscissa(0)------得到第一行的冰道坐标
inline int IceAbscissa(int i)
{
    return ReadMemory<int>(g_mainobject + 0x60C + 4 * i);
}

// 植物类型
enum PlantType {
    PEASHOOTER = 0, // 豌豆射手
    SUNFLOWER,      // 向日葵
    CHERRY_BOMB,    // 樱桃炸弹
    WALL_NUT,       // 坚果
    POTATO_MINE,    // 土豆地雷
    SNOW_PEA,       // 寒冰射手
    CHOMPER,        // 大嘴花
    REPEATER,       // 双重射手
    PUFF_SHROOM,    // 小喷菇
    SUN_SHROOM,     // 阳光菇
    FUME_SHROOM,    // 大喷菇
    GRAVE_BUSTER,   // 墓碑吞噬者
    HYPNO_SHROOM,   // 魅惑菇
    SCAREDY_SHROOM, // 胆小菇
    ICE_SHROOM,     // 寒冰菇
    DOOM_SHROOM,    // 毁灭菇
    LILY_PAD,       // 荷叶
    SQUASH,         // 倭瓜
    THREEPEATER,    // 三发射手
    TANGLE_KELP,    // 缠绕海藻
    JALAPENO,       // 火爆辣椒
    SPIKEWEED,      // 地刺
    TORCHWOOD,      // 火炬树桩
    TALL_NUT,       // 高坚果
    SEA_SHROOM,     // 水兵菇
    PLANTERN,       // 路灯花
    CACTUS,         // 仙人掌
    BLOVER,         // 三叶草
    SPLIT_PEA,      // 裂荚射手
    STARFRUIT,      // 杨桃
    PUMPKIN,        // 南瓜头
    MAGNET_SHROOM,  // 磁力菇
    CABBAGE_PULT,   // 卷心菜投手
    FLOWER_POT,     // 花盆
    KERNEL_PULT,    // 玉米投手
    COFFEE_BEAN,    // 咖啡豆
    GARLIC,         // 大蒜
    UMBRELLA_LEAF,  // 叶子保护伞
    MARIGOLD,       // 金盏花
    MELON_PULT,     // 西瓜投手
    GATLING_PEA,    // 机枪射手
    TWIN_SUNFLOWER, // 双子向日葵
    GLOOM_SHROOM,   // 忧郁菇
    CATTAIL,        // 香蒲
    WINTER_MELON,   // 冰西瓜投手
    GOLD_MAGNET,    // 吸金磁
    SPIKEROCK,      // 地刺王
    COB_CANNON,     // 玉米加农炮

    // 模仿者命名 + M
    M_PEASHOOTER,     // 豌豆射手
    M_SUNFLOWER,      // 向日葵
    M_CHERRY_BOMB,    // 樱桃炸弹
    M_WALL_NUT,       // 坚果
    M_POTATO_MINE,    // 土豆地雷
    M_SNOW_PEA,       // 寒冰射手
    M_CHOMPER,        // 大嘴花
    M_REPEATER,       // 双重射手
    M_PUFF_SHROOM,    // 小喷菇
    M_SUN_SHROOM,     // 阳光菇
    M_FUME_SHROOM,    // 大喷菇
    M_GRAVE_BUSTER,   // 墓碑吞噬者
    M_HYPNO_SHROOM,   // 魅惑菇
    M_SCAREDY_SHROOM, // 胆小菇
    M_ICE_SHROOM,     // 寒冰菇
    M_DOOM_SHROOM,    // 毁灭菇
    M_LILY_PAD,       // 荷叶
    M_SQUASH,         // 倭瓜
    M_THREEPEATER,    // 三发射手
    M_TANGLE_KELP,    // 缠绕海藻
    M_JALAPENO,       // 火爆辣椒
    M_SPIKEWEED,      // 地刺
    M_TORCHWOOD,      // 火炬树桩
    M_TALL_NUT,       // 高坚果
    M_SEA_SHROOM,     // 水兵菇
    M_PLANTERN,       // 路灯花
    M_CACTUS,         // 仙人掌
    M_BLOVER,         // 三叶草
    M_SPLIT_PEA,      // 裂荚射手
    M_STARFRUIT,      // 杨桃
    M_PUMPKIN,        // 南瓜头
    M_MAGNET_SHROOM,  // 磁力菇
    M_CABBAGE_PULT,   // 卷心菜投手
    M_FLOWER_POT,     // 花盆
    M_KERNEL_PULT,    // 玉米投手
    M_COFFEE_BEAN,    // 咖啡豆
    M_GARLIC,         // 大蒜
    M_UMBRELLA_LEAF,  // 叶子保护伞
    M_MARIGOLD,       // 金盏花
    M_MELON_PULT,     // 西瓜投手
};

enum ZombieType {
    ZOMBIE = 0,             // 普僵
    FLAG_ZOMBIE,            // 旗帜
    CONEHEAD_ZOMBIE,        // 路障
    POLE_VAULTING_ZOMBIE,   // 撑杆
    BUCKETHEAD_ZOMBIE,      // 铁桶
    NEWSPAPER_ZOMBIE,       // 读报
    SCREEN_DOOR_ZOMBIE,     // 铁门
    FOOTBALL_ZOMBIE,        // 橄榄
    DANCING_ZOMBIE,         // 舞王
    BACKUP_DANCER,          // 伴舞
    DUCKY_TUBE_ZOMBIE,      // 鸭子
    SNORKEL_ZOMBIE,         // 潜水
    ZOMBONI,                // 冰车
    ZOMBIE_BOBSLED_TEAM,    // 雪橇
    DOLPHIN_RIDER_ZOMBIE,   // 海豚
    JACK_IN_THE_BOX_ZOMBIE, // 小丑
    BALLOON_ZOMBIE,         // 气球
    DIGGER_ZOMBIE,          // 矿工
    POGO_ZOMBIE,            // 跳跳
    ZOMBIE_YETI,            // 雪人
    BUNGEE_ZOMBIE,          // 蹦极
    LADDER_ZOMBIE,          // 扶梯
    CATAPULT_ZOMBIE,        // 投篮
    GARGANTUAR,             // 白眼
    IMP,                    // 小鬼
    DR_ZOMBOSS,             // 僵博
    GIGA_GARGANTUAR = 32    // 红眼
};
