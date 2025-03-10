/**
 * @file ui.h
 * @brief 用户界面交互类定义
 *
 * 该头文件定义了用于创建命令行交互界面的核心类和数据结构
 */

#ifndef UI_H
#define UI_H

#include "datatype.h"

#include <vector>
#include <string>
#include <functional>

/**
 * @struct Option
 * @brief 表示界面中的单个选项
 *
 * @var context 选项显示文本
 * @var function_pointer 选项关联的函数指针，返回int类型状态码
 */
struct Option {
    std::string context;
    std::function<int()> function_pointer;
};


namespace ui {
    /**
    * @brief 获取并验证整数输入
    * @return 验证通过的整数值
    * @details 循环验证直到输入合法，支持负数
    */
    int input_int(const std::string &question);

    /**
     * @brief 获取并验证浮点数输入
     * @return 验证通过的double值
     * @details 支持科学计数法（如3.14e-2）
     */
    double input_double(const std::string &question);

    /**
     * @brief 获取并验证非空字符串
     * @return 去除首尾空格后的有效字符串
     * @details 至少包含一个非空格字符
     */
    std::string input_string(const std::string &question);

    /**
     * @brief 从用户输入获取品牌信息
     * @return 包含完整品牌信息的Brand对象
     * @details 交互式收集品牌名称、编码、库存量和价格，进行非空和范围验证
     */
    Brand get_brand();

    /**
     * @brief 从用户输入获取商品信息
     * @return 包含完整商品信息的Item对象
     * @details 收集商品名称、编码、颜色，并支持添加多个关联品牌（最多MAX_NUMBER个）
     */
    Item get_item();

    /**
     * @brief 显示品牌详细信息
     * @param brand 要展示的Brand对象
     * @details 格式化输出品牌名称、编码、库存量和价格信息
     */
    void show_brand(const Brand &brand);

    /**
     * @brief 显示商品及其关联品牌信息
     * @param item 要展示的Item对象
     * @param show_brand 是否显示关联品牌信息
     * @details 输出商品基础信息并遍历展示所有关联品牌，显示库存总量
     */
    void show_item(const Item &item, bool show_brand = true);

    /**
     * @brief 更新品牌库存信息
     * @param brand 待更新的Brand对象
     * @return 更新后的Brand对象
     * @details 显示当前信息后，仅允许修改库存数量（需非负验证）
     */
    Brand update_brand(Brand brand);

    /**
     * @brief 更新商品关联品牌信息
     * @param item 待更新的Item对象
     * @return 更新后的Item对象
     * @details 遍历所有关联品牌进行更新，并重新计算商品总库存量
     */
    Item update_item(Item item);
}


/**
 * @class Interface
 * @brief 命令行交互界面管理类
 *
 * 提供动态选项管理、用户输入处理和界面展示功能
 */
class Interface {
private:
    std::string question; ///< 界面顶部显示的问题/提示语
    std::vector<Option> options; ///< 存储当前可用选项的容器
    bool end_option; ///< 是否自动添加退出选项标志位

    /**
     * @brief 渲染交互界面菜单
     * @details 输出问题提示和所有可用选项，自动处理退出选项的显示逻辑。
     * 当end_option为true时，始终在最后显示"0: 退出"选项
     */
    void show_menu() const;

    /**
     * @brief 验证用户输入合法性
     * @param input 待验证的原始输入字符串
     * @return 转换后的整数选项值，输入非法时返回-1
     * @details
     * - 检查输入是否为空或纯空格
     * - 验证是否为有效整数字符串
     * - 捕获所有转换异常并给出错误提示
     */
    static int validate_input(const std::string &input);

    /**
     * @brief 验证选项值的有效性
     * @param choice 待验证的选项值
     * @return 是否属于有效选项
     * @details
     * - 检查选项是否在[0, options.size())区间
     * - 当end_option为false时，0号选项视为无效
     * - 无效时输出错误提示信息
     */
    bool is_valid_choice(int choice) const;

    /**
     * @brief 获取有效用户输入
     * @return 经过验证的合法选项值
     * @details 循环获取输入直至得到有效选项，显示"> "提示符，
     * 组合调用validate_input和is_valid_choice进行双重验证
     */
    int get_valid_choice() const;

public:
    /**
     * @brief 构造函数
     * @param str 界面显示的问题文本
     * @param is_add_end_option 是否自动添加"退出"选项（选项0）
     */
    Interface(std::string str, bool is_add_end_option);

    /**
     * @brief 添加新选项到界面
     * @param target 要添加的选项结构体
     */
    void append(const Option &target);

    /**
     * @brief 移除指定索引的选项
     * @param index 要移除的选项索引（支持负数倒序索引）
     */
    int drop(int index);

    /**
     * @brief 清空所有选项
     */
    void clear();

    /**
     * @brief 启动交互界面
     *
     * 循环显示界面直到用户选择退出，处理输入验证和选项执行
     */
    int ask() const;
};

#endif //UI_H
