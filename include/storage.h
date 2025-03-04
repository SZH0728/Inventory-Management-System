#ifndef FILE_H
#define FILE_H

#include "../include/datatype.h"

#include <string>
#include <list>
#include <fstream>


class BaseFile {
private:
    std::string path;
    bool has_file_object = false;
    std::fstream file_object;
protected:
    std::fstream get_file_object();
    bool set_file_path(std::string new_file_path);
    void clear_file_context();
public:
    bool open_file_object();
    bool close_file_object();
    explicit BaseFile(std::string file_path);
};


class WriteDataFile: virtual public BaseFile {
private:
    static std::string escape_csv_field(const std::string& field);
    static std::string brand_to_csv(const Brand& brand);
    static std::string item_to_csv(const Item& item);
public:
    using BaseFile::BaseFile;
    bool write(const std::list<Item>& items);
};


class ReadDataFile: virtual public BaseFile {
private:
    static std::string unescape_csv_field(const std::string& field);
    static Brand parse_brand_line(const std::string& line);
    static Item parse_item_line(const std::string& line);
public:
    using BaseFile::BaseFile;
    std::list<Item> read();
};


class DataFile: public ReadDataFile, public WriteDataFile {
public:
    explicit DataFile(const std::string& file_path)
        : BaseFile(file_path),
          ReadDataFile(file_path),
          WriteDataFile(file_path) {}
};


class OperationFile: public BaseFile {
public:
    using BaseFile::BaseFile;
    bool append(const std::string &line);
    std::string pop();
    std::list<std::string> clear();
};

#endif //FILE_H
