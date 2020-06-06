#pragma once
#include <map>
#include <list>
#include <vector>
#include "Item.h"

class Data
{
private:
    std::vector<std::string> names;
    std::map <char, std::map<int, std::list< Item* >* >* > data;
public:
    Data();
    Data(int n);
    ~Data();

    int CountItems();
    int CountGroupItems(char c);
    int CountSubgroupItems(char c, int i);

    void Add(Item* it);
    void FillNames();
    std::string GetRandomName();

    void PrintSubgroupByNames(char c, int i);
    void PrintSubgroupByDates(char c, int i);

    void PrintAll();
    void PrintGroup(char c);
    void PrintItem(char c, int i, std::string s);

    bool RemoveItem(char c, int i, std::string s);
    bool RemoveSubgroup(char c, int i);
    bool RemoveGroup(char c);

    Item* GetItem(char c, int i, std::string s);
    std::list<Item*>* GetSubgroup(char c, int i);
    std::map<int, std::list< Item* >* >* GetGroup(char c);

    Item* InsertItem(char c, int i, std::string s, Date d);
    std::list<Item*>* InsertSubgroup(char c, int i, std::initializer_list<Item*> items);
    std::map<int, std::list<Item*>*>* InsertGroup(char c, std::initializer_list<int> subgroups,
        std::initializer_list<std::initializer_list<Item*>> items);
};