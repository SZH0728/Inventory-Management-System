/**
 * @file storage.h
 * @brief 文件存储模块，定义文件操作相关类
 */

#ifndef FILE_H
#define FILE_H

#include "../include/datatype.h"

#include <string>
#include <list>
#include <fstream>


/**
 * @class BaseFile
 * @brief 基础文件操作类
 */
class BaseFile {
private:
    std::string path;
    bool has_file_object = false;
    std::fstream file_object;

protected:
    /**
     * @brief 获取文件流对象
     * @return 当前打开的文件流对象引用
     * @warning 需确保文件已通过open_file_object()成功打开
     */
    std::fstream &get_file_object();

    /**
     * @brief 设置文件路径
     * @param new_file_path 新文件路径
     * @return 路径修改成功返回true，文件已打开时修改失败返回false
     */
    bool set_file_path(std::string new_file_path);

    /**
     * @brief 清空文件内容
     * @note 会立即清空文件所有内容
     */
    void clear_file_context();

    /**
      * @brief 重置文件流状态
      * @details 执行以下清理操作：
      * 1. 清除文件流的错误状态标志
      * 2. 重置读写位置到文件起始位置
      * @note 适用于需要重新从头读写文件的场景，需确保文件已通过open_file_object()成功打开
      */
    void reduction();

public:
    /**
     * @brief 构造函数
     * @param file_path 初始文件路径
     * @note 不会自动打开文件，需显式调用open_file_object()
     */
    explicit BaseFile(std::string file_path);

    /**
        * @brief 打开文件流
        * @return 成功打开返回true，文件已打开或路径无效时返回false
        * @note 以读写模式打开文件，保留原有内容
        */
    bool open_file_object();

    /**
     * @brief 关闭文件流
     * @return 成功关闭返回true，文件未打开时返回false
     */
    bool close_file_object();

    /**
     * @brief 析构函数
     * @note 关闭文件流
     */
    virtual ~BaseFile();
};


/**
 * @class WriteDataFile
 * @brief CSV数据写入操作类
 */
class WriteDataFile : virtual public BaseFile {
private:
    /**
     * @brief 转义CSV字段特殊字符
     * @param field 原始字段字符串
     * @return 转义后的合规CSV字段
     * @details 处理规则：
     * - 包含逗号、换行符、双引号时添加外围双引号
     * - 内部双引号转换为两个连续双引号
     */
    static std::string escape_csv_field(const std::string &field);

    /**
     * @brief 序列化Brand对象为CSV行
     * @param brand 品牌数据对象
     * @return 格式为 "name,code,quantity,price"的CSV行
     */
    static std::string brand_to_csv(const Brand &brand);

    /**
     * @brief 序列化Item对象为CSV行
     * @param item 商品数据对象
     * @return 格式为"name,code,colour,quantity"的CSV行
     * @note 关联的Brand数据会作为后续行单独写入
     */
    static std::string item_to_csv(const Item &item);

public:
    using BaseFile::BaseFile;

    /**
     * @brief 写入完整商品数据
     * @param items 商品数据列表
     * @return 写入成功返回true，文件未打开或写入失败返回false
     * @details 写入流程：
     * 1. 清空文件内容
     * 2. 按层次写入Item及其关联的Brand数据
     * 3. 每个Item后跟其brand_list中的所有Brand记录
     */
    bool write(const std::list<Item> &items);
};


/**
 * @class ReadDataFile
 * @brief CSV数据读取操作类
 */
class ReadDataFile : virtual public BaseFile {
private:
    /**
     * @brief 反转义CSV字段
     * @param field 转义后的CSV字段
     * @return 原始字段内容
     */
    static std::string unescape_csv_field(const std::string &field);

    /**
     * @brief 解析品牌CSV行
     * @param line CSV格式字符串
     * @return 解析后的Brand对象
     */
    static Brand parse_brand_line(const std::string &line);

    /**
     * @brief 解析商品CSV行
     * @param line CSV格式字符串
     * @return 解析后的Item对象
     */
    static Item parse_item_line(const std::string &line);

public:
    using BaseFile::BaseFile;

    /**
     * @brief 读取完整商品数据
     * @return 包含所有商品及其品牌数据的列表
     * @details 解析规则：
     * - 每个Item行后跟若干Brand行构成完整商品数据
     * - 自动建立Item与Brand的关联关系
     */
    std::list<Item> read();
};


/**
 * @class DataFile
 * @brief 数据文件综合操作类
 */
class DataFile : public ReadDataFile, public WriteDataFile {
public:
    /**
     * @brief 构造数据文件对象
     * @param file_path 数据文件路径
     * @note 同时初始化读写功能的基类组件
     */
    explicit DataFile(const std::string &file_path)
        : BaseFile(file_path),
          ReadDataFile(file_path),
          WriteDataFile(file_path) {
    }
};


/**
 * @class OperationFile
 * @brief 操作日志文件管理类
 */
class OperationFile : public BaseFile {
public:
    using BaseFile::BaseFile;

    /**
     * @brief 追加操作记录
     * @param line 操作日志内容
     * @return 写入成功返回true，文件未打开时返回false
     * @note 自动添加换行符
     */
    bool append(const std::string &line);

    /**
     * @brief 弹出最后一条操作记录
     * @return 被移除的操作记录内容
     * @warning 文件未打开时返回空字符串
     * @note 会物理删除文件最后一行
     */
    std::string pop();

    /**
     * @brief 清空所有操作记录
     * @return 被清除的操作记录列表
     * @note 清空后文件内容为空
     */
    std::list<std::string> clear();
};

#endif //FILE_H
