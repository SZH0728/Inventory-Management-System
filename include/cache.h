/**
* @file cache.h
 * @brief LRU缓存机制实现头文件
 */

#ifndef CACHE_H
#define CACHE_H

#include "datatype.h"

#include <unordered_map>
#include <list>


/**
 * @class LRUCache
 * @brief 基于LRU（最近最少使用）算法的缓存容器
 *
 * 使用双链表维护访问顺序，哈希表实现快速查找，支持O(1)时间复杂度的插入、删除和查询操作
 */
class LRUCache {
private:
    /// @brief 商品编码到链表迭代器的映射表（key: 商品编码，value: 链表迭代器）
    std::unordered_map<int, std::list<Item>::iterator> code_to_item;

    /// @brief 商品名称到链表迭代器的映射表（key: 商品名称，value: 链表迭代器）
    std::unordered_map<std::string, std::list<Item>::iterator> name_to_item;

    /// @brief 维护商品访问顺序的双向链表（最新访问的在前）
    std::list<Item> cache;

    /// @brief 缓存最大容量限制（默认10）
    int max_cache = 10;

public:
    /**
     * @brief 构造函数初始化缓存容量
     * @param max_cache_number 缓存最大容量
     */
    explicit LRUCache(int max_cache_number);

    /**
     * @brief 插入或更新缓存项
     * @param item 要插入的商品对象
     * @note 如果商品已存在则更新值并移动至链表头部，不存在则插入新项，超容时淘汰尾部
     */
    void insert(const Item& item);

    /**
     * @brief 根据商品编码删除缓存项
     * @param index 商品编码
     * @return 删除成功返回true，无对应项返回false
     */
    bool del(int index);

    /**
     * @brief 根据商品名称删除缓存项
     * @param name 商品名称
     * @return 删除成功返回true，无对应项返回false
     */
    bool del(const std::string& name);

    /**
     * @brief 根据商品编码查询缓存项
     * @param index 商品编码
     * @return 对应的商品对象
     * @throw std::out_of_range 当商品不存在时抛出
     * @note 查询成功后会将项移动至链表头部
     */
    Item select(int index);

    /**
     * @brief 根据商品名称查询缓存项
     * @param name 商品名称
     * @return 对应的商品对象
     * @throw std::out_of_range 当商品不存在时抛出
     * @note 查询成功后会将项移动至链表头部
     */
    Item select(const std::string& name);
};

#endif //CACHE_H
