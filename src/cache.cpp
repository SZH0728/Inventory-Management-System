#include "../include/cache.h"

#include <stdexcept>

LRUCache::LRUCache(const int max_cache_number) {
    max_cache = max_cache_number;
}


Item LRUCache::select(const int index) {
    if (!code_to_item.count(index)) {
        throw std::out_of_range("No such item");
    }

    const auto iter = code_to_item.at(index);
    cache.splice(cache.begin(), cache, iter);
    return *iter;
}


Item LRUCache::select(const std::string &name) {
    if (!name_to_item.count(name)) {
        throw std::out_of_range("No such item");
    }

    const auto iter = name_to_item.at(name);
    cache.splice(cache.begin(), cache, iter);
    return *iter;
}


bool LRUCache::del(const int index) {
    if (!code_to_item.count(index)) {
        return false;
    }

    const auto iter = code_to_item.at(index);

    code_to_item.erase(index);
    name_to_item.erase(iter->name);

    cache.erase(iter);

    return true;
}


bool LRUCache::del(const std::string &name) {
    if (!name_to_item.count(name)) {
        return false;
    }

    const auto iter = name_to_item.at(name);

    code_to_item.erase(iter->code);
    name_to_item.erase(name);

    cache.erase(iter);

    return true;
}


void LRUCache::insert(const Item &item) {
    if (code_to_item.count(item.code)) {
        // 更新已有键的值并移动到头部
        const auto iter = code_to_item.at(item.code);
        *iter = item;
        cache.splice(cache.begin(), cache, iter);
        return;
    }

    // 插入新键到链表头部
    cache.emplace_front(item);
    code_to_item[item.code] = cache.begin();
    name_to_item[item.name] = cache.begin();

    // 超过容量则淘汰尾部元素
    if (cache.size() > max_cache) {
        const Item value = cache.back();

        code_to_item.erase(value.code);
        name_to_item.erase(value.name);
        cache.pop_back();
    }
}
