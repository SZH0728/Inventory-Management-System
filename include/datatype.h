/**
* @file datatype.h
 * @brief 数据结构定义
 */

#ifndef DATATYPE_H
#define DATATYPE_H

#include <string>
#include <list>

constexpr int MAX_NUMBER = 10; ///< 商品品牌最大数量限制

/**
 * @struct Brand
 * @brief 品牌数据结构
 */
struct Brand {
    std::string name; ///< 品牌名称（唯一标识）
    int code; ///< 品牌编码（4位数字）
    int quantity; ///< 当前库存数量
    float price; ///< 单品价格
};


/**
 * @struct Item
 * @brief 商品数据结构
 */
struct Item {
    std::string name; ///< 商品名称
    int code; ///< 商品编码
    std::string colour; ///< 商品色调
    int quantity; ///< 商品总库存
    std::list<Brand> brand_list; ///< 关联品牌列表（最大10个）
    int brand_number; ///< 当前关联品牌数量
};


class ReadLogic {
private:
    /**
        * @brief 反转义CSV字段
        * @param field 转义后的CSV字段
        * @return 原始字段内容
        */
    static std::string unescape_csv_field(const std::string &field);

protected:
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
};


class WriteLogic {
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

protected:
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
};

#endif //DATATYPE_H
