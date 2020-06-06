#include "Item.h"
#include <string>
#include <sstream>

Item::Item(){
    std::random_device rd;
    std::mt19937 seed(rd());
    //uniform_int_distribution<> rrand((unsigned int)'A', (unsigned int)'Z');
    std::uniform_int_distribution<> rrand((unsigned int)'A', (unsigned int)'C');
    Group = rrand(seed);
    //rrand = uniform_int_distribution<>((unsigned int)0, (unsigned int)99);
    rrand = std::uniform_int_distribution<>((unsigned int)0, (unsigned int)5);
    Subgroup = rrand(seed);
    Name.assign("<noname>");
    Timestamp = Date::CreateRandomDate(Date(1, 1, 2000), Date());
}

Item::Item(char c, int g, std::string s, Date d){
    Group = c;
    Subgroup = g;
    Name.assign(s);
    Timestamp = d;
}

Item::Item(const Item& Original){
    this->SetGroup(Original.Group);
    this->SetSubgroup(Original.Subgroup);
    this->SetName(Original.Name);
}

Item::Item(const std::string gs) {
    std::stringstream ss(gs);

    ss >> Group;
    ss >> Subgroup;

    char c = '.';
    std::string str;
    while (c != '<') ss >> c;
    ss >> c;
    do {
        str.push_back(c);
        ss >> std::noskipws >> c;
    } while (c != '>');
    ss >> std::skipws;
    Name.assign(str);

    int day, year;
    ss >> day >> str >> year;
    const char MonthNames[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    
    int iMonth = std::find(std::begin(MonthNames), std::end(MonthNames), str) - std::begin(MonthNames);
    if (iMonth < 0 || iMonth > 11) {
        std::cout << "unknown month" << std::endl;
        iMonth = 0;
    }
    Timestamp = Date(day, iMonth + 1, year);
}

std::string Item::toString() {
    std::stringstream ss;
    ss << Group << " " << Subgroup << " " 
        << Name << " " << Timestamp.ToString();
    return ss.str();
}