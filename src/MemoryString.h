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
#include <set>

enum ObjectType {
    PLANT,
    ZOMBIE
};

struct MemoryDict {
    QString name;
    uint32_t address;
    QString data_type;
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

QStringList ObjectIndexToString(const std::list<int>& dict_index, int target_index, int object_type, int update_time);
std::pair<QStringList, double> PlantMemoryToString(const std::list<int>& dict_index, double find_threshold, const std::set<int>& type_set, int update_time);
std::pair<QStringList, double> ZombieMemoryToString(const std::list<int>& dict_index, double find_threshold, const std::set<int>& type_set, int update_time);
QStringList SeedMemoryToString();
QStringList PlaceMemoryToString();
QStringList OtherMemoryToString();
