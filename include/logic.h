/**
 * @file logic.h
 * @brief 商品管理系统菜单逻辑类定义
 *
 * 该头文件定义了商品管理系统的各级菜单类及其交互逻辑
 */
#ifndef LOGIC_H
#define LOGIC_H

#include "engine.h"
#include "ui.h"


/**
 * @class BaseMenu
 * @brief 菜单基类
 *
 * 提供菜单系统的基础框架，包含引擎实例和界面交互对象
 */
class BaseMenu {
protected:
    Engine *engine;
    Interface menu;

public:
    /**
     * @brief 基类菜单构造函数
     * @param engine 业务逻辑引擎指针
     * @param question 菜单提示问题
     * @param is_add_end_option 是否自动添加退出选项
     */
    explicit BaseMenu(Engine *engine, std::string question, bool is_add_end_option);

    /**
     * @brief 启动菜单主循环
     * @return 操作状态码
     */
    int main() const;
};


/**
 * @class QueryItemMenu
 * @brief 商品查询菜单
 *
 * 提供按名称/代码查询商品的功能，继承自BaseMenu
 */
class QueryItemMenu : public BaseMenu {
public:
    /**
     * @brief 查询菜单构造函数
     * @param engine 业务逻辑引擎指针
     */
    explicit QueryItemMenu(Engine *engine);

    /**
     * @brief 按名称查询商品
     * @param name 商品名称（默认空字符串时触发输入）
     * @return 操作状态码（-1保持菜单，0退出）
     * @details 显示匹配商品详细信息，无结果时提示
     */
    int query_by_name(std::string name = std::string("")) const;

    /**
     * @brief 按名称模糊查询商品
     * @param name 商品名称（默认空字符串时触发输入）
     * @return 操作状态码（-1保持菜单，0退出）
     * @details 支持模糊匹配查询，显示匹配商品详细信息
     */
    int query_by_name_like(std::string name = std::string("")) const;

    /**
    * @brief 按代码查询商品
    * @param code 商品代码（默认-1时触发输入）
    * @return 操作状态码（-1保持菜单，0退出）
    * @details 支持精确代码查询，显示完整商品信息
    */
    int query_by_code(int code = -1) const;
};


/**
 * @class DeleteItemMenu
 * @brief 商品删除菜单
 *
 * 提供按名称/代码删除商品的功能，继承自BaseMenu
 */
class DeleteItemMenu : public BaseMenu {
public:
    /**
     * @brief 删除菜单构造函数
     * @param engine 业务逻辑引擎指针
     * @details 初始化删除选项：按名称删除和按代码删除
     */
    explicit DeleteItemMenu(Engine *engine);

    /**
     * @brief 按名称删除商品
     * @param name 商品名称（默认空字符串时触发输入）
     * @return 操作状态码（-1保持菜单，0退出）
     * @details 支持模糊匹配删除，批量删除同名商品
     *          删除失败时捕获out_of_range异常并提示
     */
    int delete_by_name(std::string name = std::string("")) const;

    /**
     * @brief 按代码删除商品
     * @param code 商品代码（默认-1时触发输入）
     * @return 操作状态码（-1保持菜单，0退出）
     * @details 精确匹配商品代码删除，确保编码唯一性
     *          删除失败时处理异常并给出用户反馈
     */
    int delete_by_code(int code = -1) const;
};


/**
 * @class ItemImportExport
 * @brief 商品出入库操作工具类
 *
 * 提供通用方法，供出入库菜单继承使用
 */
class ItemImportExport {
public:
    /**
     * @brief 调整商品库存数量
     * @param old_item 原始商品对象
     * @param greater 数量调整方向（true表示增加库存，false表示减少）
     * @return 调整后的新商品对象
     * @details 根据greater参数方向创建新商品对象，增加或减少库存数量
     *          库存调整量通过UI交互获取，保持原商品其他属性不变
     */
    static Item item_quantity_change(const Item& old_item, bool greater);

    /**
     * @brief 生成报表头部信息
     * @param is_import 报表类型标识（true=入库报表，false=出库报表）
     * @return 格式化报表头部字符串
     * @details 包含操作类型（入库/出库）、系统时间、操作人员等信息
     *          使用跨平台时间函数保证兼容性
     */
    static std::string generate_header(bool is_import);
};


/**
 * @class ExportItemMenu
 * @brief 商品出库菜单
 *
 * 处理商品出库操作，记录变更并生成出货统计报表
 */
class ExportItemMenu : public BaseMenu{
private:
    std::list<std::pair<Item, Item> > change; ///< 出库变更记录列表（旧商品，新商品）
public:
    /**
     * @brief 出库菜单构造函数
     * @param engine 业务逻辑引擎指针
     */
    explicit ExportItemMenu(Engine *engine);

    /**
     * @brief 按名称进行出库操作
     * @param name 商品名称（默认空字符串时触发输入）
     * @return 操作状态码
     * @details 支持批量出库，更新后自动计算库存总量
     */
    int export_by_name(std::string name = std::string(""));

    /*
     * @brief 按名称模糊进行出库操作
     * @param name 商品名称（默认空字符串时触发输入）
     * @return 操作状态码
     * @details 支持模糊匹配出库，更新后自动计算库存总量
     */
    int export_by_name_like(std::string name = std::string(""));

    /**
     * @brief 按代码进行出库操作
     * @param code 商品代码（默认-1时触发输入）
     * @return 操作状态码
     */
    int export_by_code(int code = -1);

    /**
    * @brief 生成出货统计报表
    * @return 格式化报表字符串
    * @details 包含时间戳、出货明细（品牌/数量/小计）、总金额
    *          自动处理跨平台时间格式化差异
    */
    std::string generate() const;
};


/**
 * @class ImportItemMenu
 * @brief 商品入库菜单
 *
 * 处理商品入库操作，记录变更并生成进货统计报表
 */
class ImportItemMenu : public BaseMenu{
private:
    std::list<std::pair<Item, Item> > change; ///< 入库变更记录列表（旧商品，新商品对）

public:
    /**
     * @brief 入库菜单构造函数
     * @param engine 业务逻辑引擎指针
     * @details 初始化入库选项：按名称入库和按代码入库
     */
    explicit ImportItemMenu(Engine *engine);

    /**
     * @brief 按名称进行入库操作
     * @param name 商品名称（默认空字符串时触发输入）
     * @return 操作状态码
     * @details 支持批量入库，自动更新品牌库存量
     *          计算新旧商品差异生成入库记录
     */
    int import_by_name(std::string name = std::string(""));

    /**
     * @brief 按名称模糊进行入库操作
     * @param name 商品名称（默认空字符串时触发输入）
     * @return 操作状态码
     * @details 支持模糊匹配入库，自动更新品牌库存量
     *          计算新旧商品差异生成入库记录
     */
    int import_by_name_like(std::string name = std::string(""));

    /**
     * @brief 按代码进行入库操作
     * @param code 商品代码（默认-1时触发输入）
     * @return 操作状态码
     * @details 精确匹配商品代码，支持多品牌同时入库
     *          自动重新计算商品总库存量
     */
    int import_by_code(int code = -1);

    /**
     * @brief 生成入库统计报表
     * @return 格式化报表字符串
     * @details 包含时间戳、进货明细（品牌/数量/单价）、小计
     *          使用跨平台安全时间函数（localtime_s/localtime_r）
     */
    std::string generate() const;
};


/**
 * @class Main
 * @brief 主菜单系统
 *
 * 集成所有子菜单功能，管理程序主循环和持久化操作
 */
class Main : public BaseMenu {
private:
    Engine engine; ///< 业务引擎实例
    QueryItemMenu query_item_menu; ///< 查询子菜单
    DeleteItemMenu delete_item_menu; ///< 删除子菜单
    ExportItemMenu export_item_menu; ///< 出库子菜单
    ImportItemMenu import_item_menu; ///< 入库子菜单

public:
    /**
     * @brief 主菜单构造函数
     * @details 初始化引擎参数，构建完整菜单选项
     */
    Main();

    /**
     * @brief 添加新商品
     * @return 操作状态码
     * @details 通过UI交互获取完整商品信息
     */
    int add_item();

    /**
     * @brief 显示所有商品
     * @return 操作状态码
     * @details 遍历显示库存中全部商品信息
     */
    int show_item();

    /**
     * @brief 启动查询子菜单
     * @return 子菜单返回的状态码
     * @details 委托给QueryItemMenu实例处理查询逻辑
     */
    int query_item() const;

    /**
     * @brief 启动删除子菜单
     * @return 子菜单返回的状态码
     * @details 委托给DeleteItemMenu实例处理删除逻辑
     */
    int delete_item() const;

    /**
     * @brief 启动出库子菜单
     * @return 子菜单返回的状态码
     * @details 委托给ExportItemMenu实例处理出库逻辑
     */
    int export_item() const;

    /**
     * @brief 启动入库子菜单
     * @return 子菜单返回的状态码
     * @details 委托给ImportItemMenu实例处理入库逻辑
     */
    int import_item() const;

    /**
     * @brief 主程序入口
     * @return 最终状态码
     * @details 退出时自动保存出/入库记录到文件
     *          生成export_list.txt和import_list.txt
     */
    int main() const;
};

#endif //LOGIC_H
