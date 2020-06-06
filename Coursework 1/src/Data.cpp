#include <string>
#include <iostream>
#include <fstream>
#include "Data.h"

using namespace std;

Data::Data() { FillNames(); };
Data::Data(int n){
    FillNames();
    random_device rd;
    mt19937 seed(rd());
    uniform_int_distribution<> rrand((unsigned int)0, (unsigned int)(names.size() - 1));

    Item* it;
    for (int i = 0; i < n; ++i)
    {
        it = new Item();
        it->SetName(names.at(rrand(seed)));
        Add(it);
    }
}

Data::~Data(){
    for (auto& pair_group : data)
    {
        for (auto& pair_subgr : *(pair_group.second))
        {
            for (auto& item : *(pair_subgr.second))
            {
                delete item;
            }
            delete pair_subgr.second;
        }
        delete pair_group.second;
    }
}

int Data::CountItems(){
    int count = 0;
    for (auto group : data)
        for (auto subgr : *(group.second))
            count += subgr.second->size();
    return count;
}

int Data::CountGroupItems(char c){
    auto group = GetGroup(c);
    if (group == nullptr) return 0;
    int count = 0;
    for (auto subgr : *group)
        count += subgr.second->size();
    return count;
}

int Data::CountSubgroupItems(char c, int i){
    auto subgr = GetSubgroup(c, i);
    if (subgr == nullptr) return 0;
    return subgr->size();
}

void Data::Add(Item* it){
    data.insert({ it->Group, new map<int, list< Item* >* > }).first->second->
        insert({ it->Subgroup, new list< Item* > }).first->second->push_back(it);
}

void Data::FillNames(){
    string s;
    ifstream fin;
    fin.open("Birds.txt");
    while (getline(fin, s))
        if (s.compare("") != 0)
            names.push_back(s);
    fin.close();
}

string Data::GetRandomName(){
    random_device rd;
    mt19937 seed(rd());
    uniform_int_distribution<> rrand((unsigned int)0, (unsigned int)(names.size() - 1));
    return names.at(rrand(seed));
}

void Data::PrintSubgroupByNames(char c, int i){
    auto list = GetSubgroup(c, i);
    if (list == nullptr)
        throw invalid_argument("Data::PrintSubgroupByNames::No subgroup found");
    list->sort([](const Item* const& a, const Item* const& b) {return a->Name.compare(b->Name) <= 0; });
    cout << c << ": " << endl;
    for (auto x : *list)
    {
        cout << i << ": " << x->toString() << endl;
    }
}

void Data::PrintSubgroupByDates(char c, int i){
    auto list = GetSubgroup(c, i);
    if (list == nullptr)
        throw invalid_argument("Data::PrintSubgroupByNames::No subgroup found");
    list->sort([](const Item* const& a, const Item* const& b) {return a->Timestamp < b->Timestamp; });
    cout << c << ": " << endl;
    for (auto x : *list)
    {
        cout << i << ": " << x->toString() << endl;
    }
}

void Data::PrintAll(){
    cout << " > Printing all\n";
    for (auto group_pair : data)
        PrintGroup(group_pair.first);
    cout << endl;
}

void Data::PrintGroup(char c){
    auto group = GetGroup(c);
    if (group == nullptr) throw invalid_argument("Data::PrintGroup::No group found");
    cout << c << ":\n";
    for (auto subgr_pair : *group)
    {
        for (auto item : *(subgr_pair.second))
        {
            cout << subgr_pair.first << ": " << item->toString() << endl;
        }
    }
    cout << endl;
}

void Data::PrintItem(char c, int i, string s){
    auto item = GetItem(c, i, s);
    if (item == nullptr)
        throw invalid_argument("Data::PrintItem::No item found");
    else cout << item->toString() << endl;
}

bool Data::RemoveItem(char c, int i, string s){
    auto item = GetItem(c, i, s);
    if (item == nullptr) return false;

    auto subgr = GetSubgroup(c, i);
    subgr->remove(item);
    delete item;

    if (subgr->empty())
        RemoveSubgroup(c, i);

    return true;
}

bool Data::RemoveSubgroup(char c, int i){
    auto subgr = GetSubgroup(c, i);
    if (subgr == nullptr)
        return 0;
    for (auto& item : *(subgr))
        delete item;

    auto group = GetGroup(c);
    group->erase(i);
    delete subgr;

    if (group->empty())
        RemoveGroup(c);

    return true;
}

bool Data::RemoveGroup(char c){
    auto group = GetGroup(c);
    if (group == nullptr) return false;

    for (auto& subgr_pair : *(group))
    {
        for (auto& item : *(subgr_pair.second))
            delete item;
        delete subgr_pair.second;
    }

    data.erase(c);
    delete group;
    return true;
}

Item* Data::GetItem(char c, int i, string s){
    auto subgr = GetSubgroup(c, i);
    if (subgr == nullptr) return nullptr;
    for (Item* item : *subgr)
        if (item->Name.compare(s) == 0)
            return item;
    return nullptr;
}

list<Item*>* Data::GetSubgroup(char c, int i){
    auto group = GetGroup(c);
    if (group == nullptr) return nullptr;
    auto subgr_pair = group->find(i);
    if (subgr_pair == group->end()) return nullptr;
    return subgr_pair->second;
}

map<int, list< Item* >* >* Data::GetGroup(char c){
    auto group_pair = data.find(c);
    if (group_pair != data.end())
        return group_pair->second;
    else return nullptr;
}

Item* Data::InsertItem(char c, int i, string s, Date d){
    data.insert({ c, new map<int, list< Item* >* > }).first->second->
        insert({ i, new list< Item* > });
    auto list = GetSubgroup(c, i);
    Item* item = new Item(c, i, s, d);

    for (auto list_item : *list)
        if (list_item->Name.compare(s) == 0)
            return nullptr;

    list->push_back(item);

    return nullptr;
}

list<Item*>* Data::InsertSubgroup(char c, int i, initializer_list<Item*> items){
    list<Item*>* l = new list<Item*>;
    Item* it;
    for (auto x : items)
    {
        if (x->Group != c || x->Subgroup != i)
            return nullptr;
        it = new Item();
        *it = *x;
        l->push_back(it);
    }

    auto p1 = data.insert({ c, new map<int, list< Item* >* > });
    if (p1.first->second->find(i) != p1.first->second->end())
        return nullptr;

    auto p2 = p1.first->second->insert({ i, l });
    return l;
}

map<int, list<Item*>*>* Data::InsertGroup(char c, initializer_list<int> subgroups,
    initializer_list<initializer_list<Item*>> items){
    if (GetGroup(c) != nullptr) return nullptr;
    auto p1 = data.insert({ c, new map<int, list<Item*>*> }).first;
    auto i1 = subgroups.begin();
    auto i2 = items.begin();
    for (int i = 0; i < subgroups.size(); ++i, ++i1, ++i2)
        if (InsertSubgroup(c, *i1, *i2) == nullptr)
            return nullptr;
    return nullptr;
}