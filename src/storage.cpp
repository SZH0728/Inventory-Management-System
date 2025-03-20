#include "../include/storage.h"

#include <sstream>
#include <iostream>

BaseFile::BaseFile(std::string file_path) {
    path = std::move(file_path);
}

BaseFile::~BaseFile() {
    if (has_file_object) {
        close_file_object();
    }
}


bool BaseFile::open_file_object() {
    if (has_file_object) {
        return false; // 防止重复打开
    }

    std::fstream file;
    struct stat buffer{};
    // 根据文件存在性选择打开模式：存在则保留内容，不存在则创建
    if (stat(path.c_str(), &buffer) == 0) {
        file.open(path, std::ios::in | std::ios::out); // 读写模式
    } else {
        file.open(path, std::ios::in | std::ios::out | std::ios::trunc); // 新建文件
    }

    if (!file.is_open()) {
        // 错误处理
        std::cout << "Error opening file: " << errno << std::endl;
        return false;
    }

    file_object = std::move(file); // 转移文件流所有权
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


std::fstream &BaseFile::get_file_object() {
    return file_object;
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


void BaseFile::reduction() {
    if (!has_file_object) {
        return;
    }

    std::fstream &file = get_file_object();
    file.clear();

    file.seekg(0, std::ios::beg);
    file.seekp(0, std::ios::beg);
}


bool BaseFile::set_file_path(std::string new_file_path) {
    if (has_file_object) {
        return false;
    }

    path = std::move(new_file_path);
    return true;
}


bool WriteDataFile::write(const std::list<Item> &items) {
    clear_file_context(); // 清空原有内容
    std::fstream &file = get_file_object();
    std::stringstream buffer; // 内存缓冲提升写入效率

    if (!file.is_open()) {
        return false;
    }

    for (auto &item: items) {
        std::string item_row = item_to_csv(item);
        buffer << item_row << std::endl; // 写入商品行

        // 写入关联品牌数据
        for (auto &brand: item.brand_list) {
            std::string brand_row = brand_to_csv(brand);
            buffer << brand_row << std::endl;
        }

        buffer << std::endl; // 商品数据块分隔
    }

    try {
        file << buffer.rdbuf();
    } catch (const std::ios_base::failure& e) {
        std::cerr << "写入失败: " << e.what();
        return false;
    }

    reduction(); // 重置文件指针
    return true;
}


std::list<Item> ReadDataFile::read() {
    std::list<Item> items;
    std::fstream &file = get_file_object();

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

    reduction();
    return items;
}


bool OperationFile::append(const std::string &line) {
    std::fstream &file = get_file_object();
    if (!file.is_open()) {
        return false;
    }

    file.seekp(0, std::ios::end);
    if (file.fail()) {
        std::cerr << "Seek operation failed" << std::endl;
        return false;
    }
    file << line << std::endl;
    file.flush();

    reduction();
    return true;
}


std::string OperationFile::pop() {
    std::fstream &file = get_file_object();
    if (!file.is_open()) {
        return "FOE";
    }

    std::list<std::string> lines;

    std::string fileLine;
    while (std::getline(file, fileLine)) {
        lines.push_back(fileLine);
    }

    if (lines.empty()) {
        return "FIE"; // 空文件标识
    }

    std::string last = lines.back();
    lines.pop_back();

    clear_file_context(); // 清空文件后重新写入剩余内容
    std::fstream &newFile = get_file_object();
    for (const auto &line: lines) {
        newFile << line << std::endl;
    }

    newFile.flush();
    reduction();
    return last; // 返回被删除的最后一条记录
}


std::list<std::string> OperationFile::clear() {
    std::list<std::string> lines;
    std::fstream &file = get_file_object();

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


int OperationFile::size() {
    int count = 0;
    std::fstream& file = get_file_object();

    if (!file.is_open()) {
        return 0; // 文件未打开返回0
    }

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line[0] == '[') {
            ++count;
        }
    }

    reduction(); // 重置指针以便后续操作
    return count;
}

