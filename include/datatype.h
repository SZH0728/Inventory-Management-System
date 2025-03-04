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
    std::string name;   ///< 品牌名称（唯一标识）
    int code;           ///< 品牌编码（4位数字）
    int quantity;       ///< 当前库存数量
    float price;        ///< 单品价格
};


/**
 * @struct Item
 * @brief 商品数据结构
 */
struct Item {
    std::string name;               ///< 商品名称
    int code;                       ///< 商品编码
    std::string colour;             ///< 商品色调
    int quantity;                   ///< 商品总库存
    std::list<Brand> brand_list;    ///< 关联品牌列表（最大10个）
    int brand_number;               ///< 当前关联品牌数量
};

#endif //DATATYPE_H