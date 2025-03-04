
#include "../include/file.h"

#include <sstream>
#include <iostream>

BaseFile::BaseFile(std::string file_path) {
    path = std::move(file_path);
}


bool BaseFile::open_file_object() {
    if (has_file_object) {
        return false;
    }

    std::fstream file;
    struct stat buffer{};
    if (stat(path.c_str(), &buffer) == 0) {
        file.open(path, std::ios::in | std::ios::out);
    }else {
        file.open(path, std::ios::in | std::ios::out | std::ios::trunc);
    }

    if (!file.is_open()) {
        const int errno_code = errno;
        std::cout << "Error opening file: " << errno_code << std::endl;
        return false;
    }

    file_object = std::move(file);
    has_file_object = true;
    return true;
}


bool BaseFile::close_file_object() {
    if (!has_file_object) {
        return false;
    }

    file_object.close();
    has_file_object = false;
    return true;
}


std::fstream BaseFile::get_file_object() {
    return std::move(file_object);
}


void BaseFile::clear_file_context() {
    if (has_file_object) {
        close_file_object();
    }

    std::fstream file;
    file.open(path, std::ios::in | std::ios::out | std::ios::trunc);
    file_object = std::move(file);

    has_file_object = true;
}



bool BaseFile::set_file_path(std::string new_file_path) {
    if (has_file_object) {
        return false;
    }

    path = std::move(new_file_path);
    return true;
}



std::string WriteDataFile::escape_csv_field(const std::string& field) {
    if (field.find('"') != std::string::npos || field.find(',') != std::string::npos) {
        std::string escaped;
        escaped.reserve(field.length() + 2);
        escaped += '"';

        for (const char c : field) {
            if (c == '"') escaped += "\"\""; // 双引号转义为两个双引号
            else escaped += c;
        }

        escaped += '"';
        return escaped;
    }
    return field;
}


std::string WriteDataFile::brand_to_csv(const Brand &brand) {
    std::ostringstream oss;
    oss << "BRAND|"
        << escape_csv_field(brand.name) << ","
        << brand.code << ","
        << brand.quantity << ","
        << brand.price;
    return oss.str();
}


std::string WriteDataFile::item_to_csv(const Item &item) {
    std::ostringstream oss;
    oss << "ITEM|"
        << escape_csv_field(item.name) << ","
        << item.code << ","
        << escape_csv_field(item.colour) << ","
        << item.quantity;
    return oss.str();
}


bool WriteDataFile::write(const std::list<Item>& items) {
    clear_file_context();
    std::fstream file = get_file_object();

    if (!file.is_open()) {
        return false;
    }

    for (auto &item : items) {
        std::string item_row = item_to_csv(item);
        file << item_row << std::endl;

        for (auto &brand: item.brand_list) {
            std::string brand_row = brand_to_csv(brand);
            file << brand_row << std::endl;
        }

        file << std::endl;
    }

    return true;
}


std::string ReadDataFile::unescape_csv_field(const std::string& field) {
    if (field.size() >= 2 && field.front() == '"' && field.back() == '"') {
        std::string unescaped;
        unescaped.reserve(field.length());

        for (size_t i = 1; i < field.size() - 1; ++i) {
            if (field[i] == '"' && field[i+1] == '"' && i+1 < field.size()-1) {
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


Brand ReadDataFile::parse_brand_line(const std::string& line) {
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


Item ReadDataFile::parse_item_line(const std::string& line) {
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


std::list<Item> ReadDataFile::read() {
    std::list<Item> items;
    std::fstream file = get_file_object();

    if (!file.is_open()) {
        return items;
    }

    std::string line;
    Item current_item;
    bool has_item = false;

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        if (line.find("ITEM|") == 0) {
            if (has_item) {
                current_item.brand_number = static_cast<int>(current_item.brand_list.size());
                items.push_back(current_item);
            }
            current_item = parse_item_line(line);
            current_item.brand_list.clear();
            has_item = true;
        } else if (line.find("BRAND|") == 0) {
            if (has_item) {
                current_item.brand_list.push_back(parse_brand_line(line));
            }
        }
    }

    if (has_item) {
        current_item.brand_number = static_cast<int>(current_item.brand_list.size());
        items.push_back(current_item);
    }

    return items;
}


bool OperationFile::append(const std::string &line) {
    std::fstream file = get_file_object();
    if (!file.is_open()) {
        return false;
    }

    file.seekp(0, std::ios::end);
    if (file.fail()) {
        std::cerr << "Seek operation failed" << std::endl;
        return false;
    }
    file << line << std::endl;

    return true;
}


std::string OperationFile::pop() {
    std::fstream file = get_file_object();
    if (!file.is_open()) {
        return "FOE";
    }

    std::list<std::string> lines;

    std::string fileLine;
    while (std::getline(file, fileLine)) {
        lines.push_back(fileLine);
    }

    if (lines.empty()) {
        return "FIE";
    }

    std::string last = lines.back();
    lines.pop_back();

    clear_file_context();
    std::fstream newFile = get_file_object();
    for (const auto& line : lines) {
        newFile << line << std::endl;
    }

    return last;
}


std::list<std::string> OperationFile::clear() {
    std::list<std::string> lines;
    std::fstream file = get_file_object();

    if (!file.is_open()) {
        lines.emplace_back("FOE");
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    clear_file_context();
    return lines;
}

