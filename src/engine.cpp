//
// Created by User on 25-3-6.
//


#include "../include/engine.h"

#include <algorithm>

QueryBuilder::QueryBuilder(Engine *engine_) : engine(engine_) {
    number = 1;
}


QueryBuilder &QueryBuilder::where(std::function<bool(const Item &)> condition) {
    conditions.emplace_back(std::move(condition));
    return *this;
}


std::vector<Item> QueryBuilder::execute() const {
    return engine->execute(conditions, number);
}


std::vector<Item> QueryBuilder::all() {
    number = -1;
    return execute();
}


std::vector<Item> QueryBuilder::first() {
    number = 1;
    return execute();
}


std::vector<Item> QueryBuilder::limit(const int max) {
    number = max;
    return execute();
}

Engine::Engine(const int max_cache, const int max_log, const std::string& operation_file_path, const std::string& data_file_path) :
persist(data_file_path, operation_file_path, max_log),  cache(max_cache){
    items = persist.select();

    for (auto &item : items) {
        index.insert(item.name, item.code);
    }
}


Item Engine::insert(Item item) {
    if (persist.insert(item)) {
        items.push_back(item);
        index.insert(item.name, item.code);
    }

    return item;
}


Item Engine::update(Item item) {
    if (persist.update(item)) {
        items.remove_if([&item](const Item &i) { return i.code == item.code; });
        items.push_back(item);

        index.del(item.code);
        index.insert(item.name, item.code);

        cache.del(item.code);
    }

    return item;
}


Item Engine::del(const int code) {
    if (persist.del(code)) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->code == code) {
                Item item = *it;
                items.erase(it);

                index.del(code);
                cache.del(code);

                return item;
            }
        }
    }

    throw std::out_of_range("Item not found");
}


Item Engine::del(const Item& item) {
    return del(item.code);
}


QueryBuilder Engine::select() {
    return QueryBuilder(this);
}


std::vector<Item> Engine::execute(std::list<std::function<bool(const Item &)> > conditions, const int number) {
    std::vector<Item> result;

    for (auto &item : items) {
        if (number >= 0 && result.size() >= number) {
            break;
        }

        if (std::all_of(conditions.begin(), conditions.end(), [&item](const auto &condition) { return condition(item); })) {
            result.push_back(item);
        }
    }

    return result;
}


std::vector<Item> Engine::select_by_code(const int code) {
    std::vector<Item> result;

    try {
        result.push_back(cache.select(code));
        return result;
    }catch (const std::out_of_range&) {}

    for (const auto &item : items) {
        if (item.code == code) {
            result.push_back(item);
            cache.insert(item);

            return result;
        }
    }

    return result;
}


std::vector<Item> Engine::select_by_name(const std::string& name) {
    std::vector<Item> result;

    try {
        result.push_back(cache.select(name));
        return result;
    }catch (const std::out_of_range&) {}

    try {
        const int code = index.select(name);
        return select_by_code(code);
    } catch (const std::out_of_range&) {}

    return result;
}


std::vector<Item> Engine::select_by_name_like(const std::string& name) {
    std::vector<Item> result;

    const std::vector<int> codes = index.find(name);
    result.reserve(codes.size());

    for (const auto &code : codes) {
        result.push_back(select_by_code(code)[0]);
    }

    return result;
}


