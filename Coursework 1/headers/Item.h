#pragma once
#include "Date.h"
class Item
{
public:
    Item();
    Item(char c, int g, std::string s, Date d);
    Item(const Item & Original);
    Item(const std::string gs);
    ~Item() {}
    std::string toString();
    void SetGroup(char c)   { Group = c; }
    void SetSubgroup(int i) { Subgroup = i; }
    void SetName(std::string s)  { Name.assign(s); }
    void SetDate(Date d)    { Timestamp = d; }
public:
    char Group;
    int Subgroup;
    std::string Name;
    Date Timestamp;
};