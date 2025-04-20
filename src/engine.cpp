#include "../include/engine.h"

#include <algorithm>

// 初始化时默认设置获取数量为1
QueryBuilder::QueryBuilder(Engine *engine_) : engine(engine_) {
    number = 1;
}


// 添加查询条件并返回自身引用，支持链式调用
QueryBuilder &QueryBuilder::where(std::function<bool(const Item &)> condition) {
    conditions.emplace_back(std::move(condition));
    return *this;
}


// 实际执行查询的入口，将条件转发给Engine处理
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


// Engine 核心方法实现

// 初始化引擎：加载持久化数据，构建内存索引
Engine::Engine(const int max_cache, const int max_log,
              const std::string& operation_file_path,
              const std::string& data_file_path)
    : persist(data_file_path, operation_file_path, max_log),  // 初始化持久层
      cache(max_cache) {                                      // 初始化缓存
    items = persist.select(); // 从持久层加载全部数据

    // 构建内存索引
    for (auto &item : items) {
        index.insert(item.name, item.code); // 建立名称->编码的索引
    }
}


// 插入新条目：先持久化，成功后更新内存数据
Item Engine::insert(Item item) {
    if (persist.insert(item)) { // 持久化成功才更新内存
        items.push_back(item);
        index.insert(item.name, item.code); // 更新索引
    }

    return item;
}


// 更新条目：先删除旧数据，再插入新数据
Item Engine::update(Item item) {
    if (persist.update(item)) {
        // 从内存列表中移除旧数据
        items.remove_if([&item](const Item &i) {
            return i.code == item.code;
        });

        items.push_back(item);          // 添加新数据
        index.del(item.code);           // 删除旧索引
        index.insert(item.name, item.code); // 添加新索引
        cache.del(item.code);           // 使缓存失效
    }
    return item;
}


// 删除条目（通过编码）：需要遍历查找
Item Engine::del(const int code) {
    if (persist.del(code)) {
        // 线性搜索目标条目
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->code == code) {
                Item item = *it;
                items.erase(it);    // 从内存移除
                index.del(code);    // 删除索引
                cache.del(code);    // 清除缓存
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


// 查询执行核心：应用所有过滤条件，返回指定数量的结果
std::vector<Item> Engine::execute(std::list<std::function<bool(const Item &)> > conditions,
                                 const int number) {
    std::vector<Item> result;

    for (auto &item : items) {
        // 数量限制检查：当number>=0时生效
        if (number >= 0 && result.size() >= number) break;

        // 检查是否满足所有条件（AND逻辑）
        if (std::all_of(conditions.begin(), conditions.end(),
            [&item](const std::function<bool(const Item &)> &condition) {
                return condition(item);
            })) {
            result.push_back(item);
            }
    }
    return result;
}


// 按编码查询（带缓存机制）
std::vector<Item> Engine::select_by_code(const int code) {
    std::vector<Item> result;

    // 先尝试从缓存获取
    try {
        result.push_back(cache.select(code));
        return result;
    } catch (const std::out_of_range&) {}

    // 缓存未命中时遍历内存数据
    for (const auto &item : items) {
        if (item.code == code) {
            result.push_back(item);
            cache.insert(item); // 回填缓存
            return result;
        }
    }

    return result; // 未找到时返回空vector
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


// 模糊名称查询：利用索引加速查找
std::vector<Item> Engine::select_by_name_like(const std::string& name) {
    std::vector<Item> result;

    // 通过索引获取可能的编码列表
    const std::vector<int> codes = index.find(name);
    result.reserve(codes.size()); // 预分配空间优化性能

    // 逐个编码查询具体条目
    for (const auto &code : codes) {
        result.push_back(select_by_code(code)[0]);
    }

    return result;
}

