#include "../include/cache.h"

#include <stdexcept>


// 构造函数初始化缓存最大容量
LRUCache::LRUCache(const int max_cache_number) {
    max_cache = max_cache_number;   // 设置缓存容量上限
}


// 根据商品编码查询（会更新访问顺序）
Item LRUCache::select(const int index) {
    if (!code_to_item.count(index)) {
        throw std::out_of_range("No such item");
    }

    // 将找到的元素移动到链表头部（表示最近使用）
    const auto iter = code_to_item.at(index);
    cache.splice(cache.begin(), cache, iter); // 链表节点转移操作
    return *iter;
}


// 根据商品名称查询（会更新访问顺序）
Item LRUCache::select(const std::string &name) {
    if (!name_to_item.count(name)) {
        throw std::out_of_range("No such item");
    }

    // 将找到的元素移动到链表头部（表示最近使用）
    const auto iter = name_to_item.at(name);
    cache.splice(cache.begin(), cache, iter);  // 链表节点转移操作
    return *iter;
}


// 根据编码删除缓存项
bool LRUCache::del(const int index) {
    if (!code_to_item.count(index)) {
        return false; // 不存在直接返回
    }

    // 同时删除两个哈希表的映射关系
    const auto iter = code_to_item.at(index);
    code_to_item.erase(index);
    name_to_item.erase(iter->name);  // 通过迭代器获取商品名称

    cache.erase(iter);// 从链表移除
    return true;
}


// 根据名称删除缓存项
bool LRUCache::del(const std::string &name) {
    if (!name_to_item.count(name)) {
        return false;
    }

    // 删除双哈希表映射
    const auto iter = name_to_item.at(name);
    code_to_item.erase(iter->code);  // 通过迭代器获取商品编码
    name_to_item.erase(name);

    cache.erase(iter);
    return true;
}


// 插入/更新缓存项（核心方法）
void LRUCache::insert(const Item &item) {
    // 存在则更新值并移动位置
    if (code_to_item.count(item.code)) {
        const auto iter = code_to_item.at(item.code);
        *iter = item;  // 直接修改链表节点值
        cache.splice(cache.begin(), cache, iter);  // 移动到头部

        name_to_item.erase(item.name);
        name_to_item[item.name] = code_to_item[item.code];
        return;
    }

    // 插入新元素到链表头部
    cache.emplace_front(item);
    code_to_item[item.code] = cache.begin();   // 记录编码映射
    name_to_item[item.name] = cache.begin();   // 记录名称映射

    // 缓存淘汰机制：超过容量时移除末尾元素
    if (cache.size() > max_cache) {
        const Item value = cache.back();  // 获取要被淘汰的元素

        // 清理两个哈希表的映射关系
        code_to_item.erase(value.code);
        name_to_item.erase(value.name);
        cache.pop_back();  // 移除链表末尾
    }
}
