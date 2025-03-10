/**
 * @file index.h
 * @brief 索引管理及模糊查询支持头文件
 */

#ifndef PERSISTER_H
#define PERSISTER_H

#include "datatype.h"
#include "storage.h"

#include <list>

/**
 * @class Persist
 * @brief 数据持久化处理类，负责数据文件与操作日志的协同工作
 *
 * @inherits WriteLogic, ReadLogic 继承读写逻辑接口
 */
class Persist : public WriteLogic, public ReadLogic {
private:
    DataFile data_file; ///< 数据文件存储对象（持久化主存储）
    OperationFile operation_file; ///< 操作日志文件对象（事务日志存储）
    int max_log_row; ///< 操作日志最大行数阈值（触发自动刷新的阈值）
    bool has_closed = false; ///< 资源关闭状态标记（防止重复关闭）

    /**
    * @brief 操作日志写入核心方法
    * @param item 需要操作的数据条目
    * @param keyword 操作类型标识（"[insert]" / "[update]"）
    * @return 操作是否成功
    * @note 本方法实现：
    * 1. 构建包含操作类型和条目数据的日志格式
    * 2. 自动处理关联的品牌数据
    * 3. 当日志行数超过阈值时触发自动刷新
    *
    * @warning 被insert()和update()方法调用，不应直接使用
    */
    bool write_operation(const Item &item, const std::string &keyword);

    /**
    * 应用待处理操作（内部辅助方法）
    * @param items 当前数据集
    * @param operation_code 操作类型标记
    * @param target 待操作数据项
    */
    static void apply_pending_operation(std::list<Item> &items, int operation_code, Item &target);

public:
    /**
     * @brief 构造函数
     * @param data_file_path 数据文件存储路径
     * @param operation_file_path 操作日志文件路径
     * @param max_row 日志文件最大行数阈值（达到阈值自动触发flush）
     */
    Persist(const std::string &data_file_path, const std::string &operation_file_path, int max_row);

    /**
     * @brief 析构函数（自动关闭未关闭的文件资源）
     */
    ~Persist();

    /**
     * @brief 查询数据集合
     * @return 返回当前数据文件的全部条目列表
     * @note 会先执行flush操作确保数据一致性
     */
    std::list<Item> select();

    /**
     * @brief 插入新条目
     * @param item 要插入的数据条目
     * @return 操作是否成功
     */
    bool insert(const Item &item);

    /**
     * @brief 更新现有条目
     * @param item 要更新的数据条目（根据code字段匹配）
     * @return 操作是否成功
     */
    bool update(const Item &item);

    /**
     * @brief 删除指定条目
     * @param index 要删除的条目唯一标识码
     * @return 操作是否成功
     */
    bool del(int index);

    /**
     * @brief 强制刷新操作日志到数据文件
     * @return 本次刷新的日志条目数量
     * @note 执行流程：
     * 1. 读取当前数据文件内容
     * 2. 按顺序重放所有操作日志
     * 3. 排序后写入数据文件
     */
    int flush();

    /**
     * @brief 关闭文件资源
     * @return 是否成功执行关闭操作（重复关闭返回false）
     * @note 关闭前会自动执行flush操作
     */
    bool close();
};

#endif //PERSISTER_H
