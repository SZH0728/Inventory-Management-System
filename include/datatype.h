
#ifndef DATATYPE_H
#define DATATYPE_H

#include <string>
#include <list>

constexpr int MAX_NUMBER = 10;

struct Brand {
    std::string name;
    int code;
    int quantity;
    float price;
};


struct Item {
    std::string name;
    int code;
    std::string colour;
    int quantity;
    std::list<Brand> brand_list;
    int brand_number;
};

#endif //DATATYPE_H
