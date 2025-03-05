
#ifndef CACHE_H
#define CACHE_H

#include "datatype.h"

#include <unordered_map>
#include <list>

class LRUCache {
private:
    std::unordered_map<int, std::list<Item>::iterator> code_to_item;
    std::unordered_map<std::string, std::list<Item>::iterator> name_to_item;
    std::list<Item> cache;
    int max_cache = 10;

public:
    explicit LRUCache(int max_cache_number);
    void insert(const Item& item);

    bool del(int index);
    bool del(const std::string& name);

    Item select(int index);
    Item select(const std::string& name);
};

#endif //CACHE_H
