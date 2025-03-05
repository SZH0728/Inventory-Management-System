
#include "../include/persister.h"

#include <algorithm>
#include <sstream>


Persist::Persist(const std::string &data_file_path, const std::string &operation_file_path, int max_row):
data_file(data_file_path), operation_file(operation_file_path) {
    max_log_row = max_row;
    operation_file.open_file_object();
}


Persist::~Persist() {
    if (!has_closed) {
        close();
    }
}


bool Persist::close() {
    if (has_closed) {
        return false;
    }

    flush();
    operation_file.close_file_object();
    has_closed = true;

    return true;
}


std::list<Item> Persist::select() {
    data_file.open_file_object();
    std::list<Item> result = data_file.read();
    data_file.close_file_object();

    return result;
}

bool Persist::write_operation(const Item &item, const std::string& keyword) {
    std::stringstream buffer;

    buffer << keyword << std::endl;
    buffer << item_to_csv(item) << std::endl;

    for (auto &brand: item.brand_list) {
        buffer << brand_to_csv(brand) << std::endl;
    }

    const bool result = operation_file.append(buffer.str());

    if (operation_file.size() >= max_log_row) {
        flush();
    }

    return result;
}


bool Persist::insert(const Item& item) {
    return write_operation(item, "[insert]");
}

bool Persist::update(const Item& item) {
    return write_operation(item, "[update]");
}

bool Persist::del(const int index) {
    std::stringstream buffer;

    buffer << "[delete]" << index << std::endl;

    const bool result = operation_file.append(buffer.str());

    if (operation_file.size() >= max_log_row) {
        flush();
    }

    return result;
}


int Persist::flush() {
    const int size = operation_file.size();

    data_file.open_file_object();
    std::list<Item> items = data_file.read();
    std::list<std::string> operations = operation_file.clear();

    int operation_code = 0;
    Item target;
    for (auto &operation: operations) {
        if (operation.empty()) {
            continue;
        }

        if (operation == "[insert]") {
            if (operation_code == 1) {
                items.push_back(target);
            } else if (operation_code == 2) {
                std::replace_if(items.begin(), items.end(),
                    [&target](const Item &item) { return item.code == target.code; }, target);
            }

            operation_code = 1;
            continue;
        }

        if (operation == "[update]") {
            if (operation_code == 1) {
                items.push_back(target);
            } else if (operation_code == 2) {
                std::replace_if(items.begin(), items.end(),
                    [&target](const Item &item) { return item.code == target.code; }, target);
            }

            operation_code = 2;
            continue;
        }

        if (operation.find("[delete]") == 0) {
            operation_code = 0;
            int code = std::stoi(operation.substr(8));
            items.remove_if([&code](const Item &item) { return item.code == code;});
            continue;
        }

        if (operation.find("ITEM|") == 0) {
            target = parse_item_line(operation);
        }

        if (operation.find("BRAND|") == 0) {
            target.brand_list.emplace_back(parse_brand_line(operation));
        }
    }

    if (operation_code != 0) {
        if (operation_code == 1) {
            items.push_back(target);
        } else {
            std::replace_if(items.begin(), items.end(),
                [&target](const Item &item) { return item.code == target.code; }, target);
        }
    }
    items.sort([](const Item &lhs, const Item &rhs) { return lhs.code < rhs.code; });
    data_file.write(items);
    return size;
}
