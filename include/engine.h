//
// Created by User on 25-3-6.
//

#ifndef ENGINE_H
#define ENGINE_H

#include <functional>

#include "datatype.h"
#include "persister.h"
#include "cache.h"
#include "index.h"


class Engine;


class QueryBuilder {
private:
    Engine *engine;
    std::list<std::function<bool(const Item&)>> conditions;
    int number;

    std::vector<Item> execute() const;
public:
    explicit QueryBuilder(Engine *engine_);

    QueryBuilder& where(std::function<bool(const Item&)> condition);

    std::vector<Item> first();
    std::vector<Item> all();
    std::vector<Item> limit(int max);
};

class Engine {
private:
    Persist persist;
    LRUCache cache;
    Index index;

    std::list<Item> items;

    std::vector<Item> execute(std::list<std::function<bool(const Item&)>> conditions, int number);
public:
    friend class QueryBuilder;

    Engine(int max_cache, int max_log, const std::string& operation_file_path, const std::string &data_file_path);

    Item insert(Item item);
    Item del(const Item& item);
    Item del(int code);
    Item update(Item item);

    QueryBuilder select();

    std::vector<Item> select_by_code(int code);
    std::vector<Item> select_by_name(const std::string& name);
    std::vector<Item> select_by_name_like(const std::string& name);
};

#endif //ENGINE_H
