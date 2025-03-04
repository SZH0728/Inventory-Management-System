/**
 * @file ui.h
 * @brief 用户界面交互类定义
 *
 * 该头文件定义了用于创建命令行交互界面的核心类和数据结构
 */

#ifndef UI_H
#define UI_H

#include <vector>
#include <string>

/**
 * @struct Option
 * @brief 表示界面中的单个选项
 *
 * @var context 选项显示文本
 * @var function_pointer 选项关联的函数指针，返回int类型状态码
 */
struct Option {
    std::string context;

    int (*function_pointer)();
};

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
