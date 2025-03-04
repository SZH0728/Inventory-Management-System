#include "../include/ui.h"

#include <iostream>


Interface::Interface(std::string str, bool is_add_end_option) {
    question = std::move(str);
    end_option = is_add_end_option;
}


void Interface::append(const Option &target) {
    options.emplace_back(target);
}


int Interface::drop(const int index) {
    // 检查索引有效性（允许负索引）
    if (index < -options.size() || index >= options.size()) {
        return -1;
    }

    // 正索引直接删除，负索引转换为逆向删除
    if (index >= 0) {
        options.erase(options.begin() + index);
    } else {
        options.erase(options.end() + index + 1); // +1 修正负索引偏移
    }

    return 0;
}


void Interface::clear() {
    options.clear();
}


int Interface::ask() const {
    while (true) {
        show_menu(); // 显示菜单界面
        const int choice = get_valid_choice(); // 获取有效输入
        if (choice == 0) break; // 处理退出选项

        const Option target = options.at(choice - 1);
        if (target.function_pointer() == -1) {  // 执行选项关联的函数
            break; // 如果返回-1则退出循环
        }
    }
    return 0;
}


void Interface::show_menu() const {
    std::cout << question << std::endl;  // 显示问题描述

    // 遍历显示所有选项
    for (size_t i = 0; i < options.size(); i++) {
        std::cout << i + 1 << ": " << options[i].context << std::endl;
    }

    // 显示退出选项（如果启用）
    if (end_option) {
        std::cout << "0: 退出" << std::endl;
    }
}


int Interface::get_valid_choice() const {
    while (true) {
        std::string input;
        std::cout << "> ";
        std::getline(std::cin, input);

        const int choice = validate_input(input);  // 输入验证
        if (choice == -1) continue;  // 无效输入时重试

        if (is_valid_choice(choice)) {  // 检查选项有效性
            return choice;
        }
    }
}


int Interface::validate_input(const std::string &input) {
    // 检查空输入或纯空格输入
    if (input.empty() || input.find_first_not_of(' ') == std::string::npos) {
        std::cout << "输入不能为空！" << std::endl;
        return -1;
    }

    try {
        size_t pos = 0;
        const int choice = std::stoi(input, &pos);  // 尝试转换为整数

        // 检查是否包含非法后缀字符
        if (pos != input.length()) {
            throw std::invalid_argument("无效字符");
        }

        return choice;
    } catch (...) {  // 捕获所有转换异常
        std::cout << "输入包含非法字符" << std::endl;
        return -1;
    }
}


bool Interface::is_valid_choice(const int choice) const {
    // 检查退出选项的有效性
    if (choice == 0 && !end_option) {
        std::cout << "输入范围错误" << std::endl;
        return false;
    }

    // 检查选项索引越界
    if (choice < 0 || choice > options.size()) {
        std::cout << "输入范围错误" << std::endl;
        return false;
    }

    return true;
}
