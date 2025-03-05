#include "../include/persister.h"

#include <algorithm>
#include <sstream>


Persist::Persist(const std::string &data_file_path, const std::string &operation_file_path,
                 int max_row): data_file(data_file_path), operation_file(operation_file_path) {
    max_log_row = max_row;
    operation_file.open_file_object(); // 启动时立即打开操作日志文件
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

    flush(); // 关闭前强制同步数据
    operation_file.close_file_object();
    has_closed = true; // 标记关闭状态

    return true;
}


std::list<Item> Persist::select() {
    flush();

    data_file.open_file_object();
    std::list<Item> result = data_file.read();
    data_file.close_file_object();

    return result;
}


bool Persist::write_operation(const Item &item, const std::string &keyword) {
    std::stringstream buffer;

    // 构建操作日志头
    buffer << keyword << std::endl;
    buffer << item_to_csv(item) << std::endl;

    // 序列化关联品牌数据
    for (auto &brand: item.brand_list) {
        buffer << brand_to_csv(brand) << std::endl;
    }

    // 写入日志文件并检查自动刷新条件
    const bool result = operation_file.append(buffer.str());
    if (operation_file.size() >= max_log_row) {
        flush();
    }
    return result;
}


bool Persist::insert(const Item &item) {
    return write_operation(item, "[insert]");
}


bool Persist::update(const Item &item) {
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
    const int size = operation_file.size(); // 记录原始日志量

    // 读取当前数据文件内容
    data_file.open_file_object();
    std::list<Item> items = data_file.read();
    data_file.close_file_object();

    // 获取并清空操作日志
    std::list<std::string> operations = operation_file.clear();

    /*----- 操作日志重放逻辑 -----*/
    int operation_code = 0; // 0:无操作 1:插入 2:更新
    Item target; // 当前操作的临时数据载体

    for (auto &operation: operations) {
        if (operation.empty()) {
            continue;
        }

        // 处理操作类型标记
        if (operation == "[insert]") {
            apply_pending_operation(items, operation_code, target);
            operation_code = 1; // 标记后续为插入操作
            continue;
        }

        if (operation == "[update]") {
            apply_pending_operation(items, operation_code, target);
            operation_code = 2; // 标记后续为更新操作
            continue;
        }

        // 处理删除操作
        if (operation.find("[delete]") == 0) {
            apply_pending_operation(items, operation_code, target);
            operation_code = 0; // 重置操作状态
            int code = std::stoi(operation.substr(8));
            items.remove_if([&code](const Item &item) { return item.code == code; });
            continue;
        }

        // 解析条目数据行
        if (operation.find("ITEM|") == 0) {
            target = parse_item_line(operation);
        }

        // 解析品牌数据行
        if (operation.find("BRAND|") == 0) {
            target.brand_list.emplace_back(parse_brand_line(operation));
        }
    }

    // 处理最后一个未提交的操作
    apply_pending_operation(items, operation_code, target);

    // 按code升序排列后写入数据文件
    items.sort([](const Item &lhs, const Item &rhs) { return lhs.code < rhs.code; });
    data_file.write(items);
    return size; // 返回处理的日志条目数
}


void Persist::apply_pending_operation(std::list<Item> &items, int operation_code, Item &target) {
    if (operation_code == 0) {
        return;
    }

    if (operation_code == 1) {
        // 插入操作
        items.push_back(target);
        return;
    }

    if (operation_code == 2) {
        // 更新操作
        std::replace_if(items.begin(), items.end(),
                        [&target](const Item &item) { return item.code == target.code; }, target);
    }
}
