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


/**
 * @class QueryBuilder
 * @brief 提供链式查询构建功能的工具类
 */
class QueryBuilder {
private:
    Engine *engine; ///< 关联的引擎实例指针
    std::list<std::function<bool(const Item &)> > conditions; ///< 存储查询条件列表
    int number; ///< 结果集最大数量限制

    /**
     * @brief 执行构建好的查询条件
     * @return 符合条件的结果集合
     */
    std::vector<Item> execute() const;

public:
    /**
     * @brief 构造函数，关联指定引擎实例
     * @param engine_ 要关联的Engine对象指针
     */
    explicit QueryBuilder(Engine *engine_);

    /**
     * @brief 添加查询条件
     * @param condition 接受Item对象并返回bool的lambda表达式
     * @return 当前QueryBuilder对象的引用（支持链式调用）
     */
    QueryBuilder &where(std::function<bool(const Item &)> condition);

    /// @brief 获取符合条件的第一条记录
    std::vector<Item> first();

    /// @brief 获取所有符合条件的记录
    std::vector<Item> all();

    /**
     * @brief 限制查询结果的最大数量
     * @param max 最大结果数量
     * @return 不超过max条的结果集合
     */
    std::vector<Item> limit(int max);
};


/**
 * @class Engine
 * @brief 数据引擎核心类，提供数据操作和查询功能
 */
class Engine {
private:
    Persist persist; ///< 持久化操作对象
    LRUCache cache; ///< 缓存管理对象
    Index index; ///< 索引管理对象
    std::list<Item> items; ///< 内存中维护的数据集合

    /**
     * @brief 执行查询条件过滤
     * @param conditions 查询条件集合
     * @param number 最大返回数量
     * @return 过滤后的结果集合
     */
    std::vector<Item> execute(std::list<std::function<bool(const Item &)> > conditions, int number);

public:
    /// @brief 声明友元类以允许访问私有成员
    friend class QueryBuilder;

    /**
     * @brief 构造函数
     * @param max_cache 缓存最大容量
     * @param max_log 日志最大条目数
     * @param operation_file_path 操作日志文件路径
     * @param data_file_path 数据文件路径
     */
    Engine(int max_cache, int max_log, const std::string &operation_file_path, const std::string &data_file_path);

    /**
     * @brief 插入新数据项
     * @param item 要插入的Item对象
     * @return 插入成功的Item副本
     */
    Item insert(Item item);

    /**
     * @brief 通过Item对象删除数据
     * @param item 要删除的Item对象
     * @return 被删除的Item副本
     */
    Item del(const Item &item);

    /**
     * @brief 通过code删除数据
     * @param code 要删除的数据项唯一编码
     * @return 被删除的Item副本
     */
    Item del(int code);

    /**
     * @brief 更新数据项
     * @param item 包含新数据的Item对象
     * @return 更新后的Item副本
     */
    Item update(Item item);

    /// @brief 创建查询构建器实例
    QueryBuilder select();

    /**
     * @brief 通过唯一编码查询数据
     * @param code 要查询的数据项编码
     * @return 包含单个元素的vector（未找到则返回空vector）
     */
    std::vector<Item> select_by_code(int code);

    /**
     * @brief 精确匹配名称查询
     * @param name 要查询的完整名称
     * @return 匹配的结果集合
     */
    std::vector<Item> select_by_name(const std::string &name);

    /**
     * @brief 模糊匹配名称查询
     * @param name 要模糊匹配的名称片段
     * @return 匹配的结果集合
     */
    std::vector<Item> select_by_name_like(const std::string &name);
};

#endif //ENGINE_H
