
#include "../include/datatype.h"

#include <sstream>

std::string ReadLogic::unescape_csv_field(const std::string &field) {
    if (field.size() >= 2 && field.front() == '"' && field.back() == '"') {
        std::string unescaped;
        unescaped.reserve(field.length());

        for (size_t i = 1; i < field.size() - 1; ++i) {
            if (field[i] == '"' && field[i + 1] == '"' && i + 1 < field.size() - 1) {
                unescaped += '"';
                ++i;
            } else {
                unescaped += field[i];
            }
        }
        return unescaped;
    }
    return field;
}


Brand ReadLogic::parse_brand_line(const std::string &line) {
    std::istringstream iss(line.substr(6)); // 跳过"BRAND|"
    std::string token;
    Brand brand;

    // 解析名称
    std::getline(iss, token, ',');
    brand.name = unescape_csv_field(token);

    // 解析code
    std::getline(iss, token, ',');
    brand.code = std::stoi(token);

    // 解析quantity
    std::getline(iss, token, ',');
    brand.quantity = std::stoi(token);

    // 解析price
    std::getline(iss, token);
    brand.price = std::stof(token);

    return brand;
}


Item ReadLogic::parse_item_line(const std::string &line) {
    std::istringstream iss(line.substr(5)); // 跳过"ITEM|"
    std::string token;
    Item item;

    // 解析名称
    std::getline(iss, token, ',');
    item.name = unescape_csv_field(token);

    // 解析code
    std::getline(iss, token, ',');
    item.code = std::stoi(token);

    // 解析colour
    std::getline(iss, token, ',');
    item.colour = unescape_csv_field(token);

    // 解析quantity
    std::getline(iss, token);
    item.quantity = std::stoi(token);

    // brand_number暂设为0
    item.brand_number = 0;

    return item;
}


std::string WriteLogic::escape_csv_field(const std::string &field) {
    if (field.find('"') != std::string::npos || field.find(',') != std::string::npos) {
        std::string escaped;
        escaped.reserve(field.length() + 2);
        escaped += '"';

        for (const char c: field) {
            if (c == '"') escaped += "\"\""; // 双引号转义为两个双引号
            else escaped += c;
        }

        escaped += '"';
        return escaped;
    }
    return field;
}


std::string WriteLogic::brand_to_csv(const Brand &brand) {
    std::ostringstream oss;
    oss << "BRAND|"
            << escape_csv_field(brand.name) << ","
            << brand.code << ","
            << brand.quantity << ","
            << brand.price;
    return oss.str();
}


std::string WriteLogic::item_to_csv(const Item &item) {
    std::ostringstream oss;
    oss << "ITEM|"
            << escape_csv_field(item.name) << ","
            << item.code << ","
            << escape_csv_field(item.colour) << ","
            << item.quantity;
    return oss.str();
}

