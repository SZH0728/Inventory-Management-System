#include "../include/ui.h"

#include <iostream>
#include <stdexcept>


namespace ui{
    int input_int(const std::string& question) {
        std::cout << question << std::endl;
        while(true) {
            std::string input;
            std::cout << "> ";
            std::getline(std::cin, input);

            // 空输入检查
            if(input.empty() || input.find_first_not_of(' ') == std::string::npos) {
                std::cout << "输入不能为空！" << std::endl;
                continue;
            }

            try {
                size_t pos = 0;
                const int value = std::stoi(input, &pos);

                // 检查尾部非法字符
                if(pos != input.length()) {
                    throw std::invalid_argument("非法字符");
                }
                return value;
            } catch(const std::out_of_range&) {
                std::cout << "数值超出int范围！" << std::endl;
            } catch(...) {
                std::cout << "请输入有效整数！" << std::endl;
            }
        }
    }


    double input_double(const std::string& question) {
        std::cout << question << std::endl;
        while(true) {
            std::string input;
            std::cout << "> ";
            std::getline(std::cin, input);

            if(input.empty() || input.find_first_not_of(' ') == std::string::npos) {
                std::cout << "输入不能为空！" << std::endl;
                continue;
            }

            try {
                size_t pos = 0;
                const double value = std::stod(input, &pos);

                // 允许小数点开头（如.5）
                if(pos != input.length()) {
                    throw std::invalid_argument("非法字符");
                }
                return value;
            } catch(const std::out_of_range&) {
                std::cout << "数值超出范围！" << std::endl;
            } catch(...) {
                std::cout << "请输入有效浮点数！" << std::endl;
            }
        }
    }


    std::string input_string(const std::string& question) {
        std::cout << question << std::endl;
        while(true) {
            std::cout << "> ";
            std::string input;
            std::getline(std::cin, input);

            // 去除首尾空格
            const size_t start = input.find_first_not_of(" \t");
            const size_t end = input.find_last_not_of(" \t");

            if(start == std::string::npos) {
                std::cout << "输入不能为空！" << std::endl;
                continue;
            }

            return input.substr(start, end - start + 1);
        }
    }


    Item get_item() {
        Item item;
        std::cout << "=== 输入商品信息 ===" << std::endl;

        // 商品名称（非空验证）
        item.name = input_string("请输入商品名称：");

        // 商品编码（非负验证）
        do {
            item.code = input_int("请输入商品编码（≥0）：");
        } while(item.code < 0);

        // 商品颜色（非空验证）
        item.colour = input_string("请输入商品颜色：");

        // 初始化品牌信息
        item.brand_list.clear();
        item.brand_number = 0;

        // 添加品牌流程
        while(item.brand_number < MAX_NUMBER) {
            std::cout << std::endl
                      << "当前品牌数：" << item.brand_number
                      << "/" << MAX_NUMBER << std::endl;

            const std::string answer = input_string("添加品牌？(y/n): ");
            if (answer[0] != 'y' && answer[0] != 'Y') break;

            Brand brand = get_brand();
            item.brand_list.push_back(brand);
            item.brand_number++;
        }

        item.quantity = 0;
        for (const auto &brand : item.brand_list) {
            item.quantity += brand.quantity;
        }

        return item;
    }


    Brand get_brand() {
        Brand brand;
        std::cout << "=== 输入品牌信息 ===" << std::endl;

        // 名称输入（唯一标识）
        do {
            brand.name = input_string("品牌名称: ");
        } while(brand.name.empty());

        // 编码验证（唯一性需外部保证）
        brand.code = input_int("品牌编码: ");

        // 库存量验证
        do {
            brand.quantity = input_int("库存数量: ");
        } while(brand.quantity < 0);

        // 价格验证
        do {
            brand.price = input_double("单品价格: ");
        } while(brand.price < 0.0);

        return brand;
    }


    // 显示单个品牌详细信息
    void show_brand(const Brand& brand) {
        std::cout << "  品牌名称: " << brand.name << std::endl
                  << "  品牌编码: " << brand.code << std::endl
                  << "  当前库存: " << brand.quantity << std::endl
                  << "  单品价格: " << brand.price << std::endl
                  << "  ----------";
    }


    // 显示单个商品品种及其关联品牌
    void show_item(const Item& item, const bool show_brand) {
        std::cout << "商品名称: " << item.name << std::endl
                  << "商品编号: " << item.code << std::endl
                  << "商品颜色: " << item.colour << std::endl
                  << "库存总量: " << item.quantity << std::endl
                  << "关联品牌: ";

        if (!show_brand) {
            return;
        }

        if (item.brand_list.empty()) {
            std::cout << "无品牌信息";
        } else {
            std::cout << std::endl;
            for (const Brand &brand : item.brand_list) {
                ui::show_brand(brand);
                std::cout << std::endl;  // 品牌间换行分隔
            }
        }

        std::cout << std::endl << "====================" << std::endl;
    }


    Brand update_brand(Brand brand) {
        std::cout << "=== 更新品牌信息 ===" << std::endl;
        std::cout << "当前品牌信息：" << std::endl;
        show_brand(brand);
        std::cout << std::endl;

        do {
            brand.quantity = input_int("库存数量: ");
        } while(brand.quantity < 0);

        return brand;
    }


    Item update_item(Item item) {
        std::cout << "=== 更新商品信息 ===" << std::endl;
        std::cout << "当前商品信息：" << std::endl;
        show_item(item);

        if (item.brand_list.empty()) {
            std::cout << "当前商品无品牌信息" << std::endl;
            return item;
        }

        for (auto &brand : item.brand_list) {
            brand = update_brand(brand);
        }

        item.quantity = 0;
        for (const auto &brand : item.brand_list) {
            item.quantity += brand.quantity;
        }

        return item;
    }


}




Interface::Interface(std::string str, const bool is_add_end_option) {
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
        std::cout << std::endl;
        show_menu(); // 显示菜单界面
        const int choice = get_valid_choice(); // 获取有效输入
        if (choice == 0) break; // 处理退出选项

        const Option target = options.at(choice - 1);
        if (target.function_pointer() == -1) {
            // 执行选项关联的函数
            break; // 如果返回-1则退出循环
        }
    }
    return 0;
}


void Interface::show_menu() const {
    std::cout << question << std::endl; // 显示问题描述

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

        const int choice = validate_input(input); // 输入验证
        if (choice == -1) continue; // 无效输入时重试

        if (is_valid_choice(choice)) {
            // 检查选项有效性
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
        const int choice = std::stoi(input, &pos); // 尝试转换为整数

        // 检查是否包含非法后缀字符
        if (pos != input.length()) {
            throw std::invalid_argument("无效字符");
        }

        return choice;
    } catch (...) {
        // 捕获所有转换异常
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
