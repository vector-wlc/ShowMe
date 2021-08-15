/*
 * @Coding: utf-8
 * @Author: gbk
 * @Date: 2021-08-15 10:38:37
 * @Description: 
 */
#pragma once

#include "Memory.h"
#include <map>
#include <qstringlist.h>

struct MemoryDict {
    QString name;
    uint32_t address;
    uint8_t data_type; // 0 : int, 1 : float, 2 : bool, 3 : short, 4 : byte
};

struct Offset {
    float x;
    float y;
};

extern const std::vector<QString> PLANT_NAME;
extern const std::vector<QString> ZOMBIE_NAME;
extern std::vector<MemoryDict> plant_memory_dict;
extern std::vector<MemoryDict> zombie_memory_dict;
extern std::vector<std::map<int, Offset>> plant_offset_dict;
extern std::vector<std::map<int, Offset>> zombie_offset_dict;

std::pair<QStringList, double> PlantMemoryToString(const std::list<int>& dict_index, double find_threshold = 20);
std::pair<QStringList, double> ZombieMemoryToString(const std::list<int>& dict_index, double find_threshold = 50);
QStringList SeedMemoryToString();
QStringList PlaceMemoryToString();
QStringList OtherMemoryToString();
