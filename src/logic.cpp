
#include "../include/logic.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>


// 构造函数初始化业务引擎和交互界面
BaseMenu::BaseMenu(Engine *engine, std::string question, const bool is_add_end_option):
engine(engine), menu(std::move(question), is_add_end_option) {}


// 启动菜单主循环，直接调用界面对象的询问功能
int BaseMenu::main() const {
    return menu.ask();
}


// 初始化查询选项：1.按名称查询 2.按代码查询
QueryItemMenu::QueryItemMenu(Engine *engine): BaseMenu(engine, "查询菜单: ", true) {
    menu.append(Option{"按商品品种名称查询", [this]{ return this->query_by_name(); }});
    menu.append(Option{"按商品品种名称模糊查询", [this]{ return this->query_by_name_like(); }});
    menu.append(Option{"按商品品种代码查询", [this]{ return this->query_by_code(); }});
}


// 按代码查询核心逻辑：
// 1. 当code=-1时触发用户输入
// 2. 调用引擎select_by_code查询
// 3. 空结果处理：提示无商品
// 4. 遍历显示商品详细信息
int QueryItemMenu::query_by_code(int code) const {
    if (code == -1) {
        code = ui::input_int("请输入商品品种代码:");
    }

    const std::vector<Item> items = engine->select_by_code(code);
    if (items.empty()) {
        std::cout << "没有找到该商品" << std::endl;
        return 0;
    }

    for (const Item &item : items) {
        ui::show_item(item);
    }
    return -1;
}


// 按名称查询（逻辑同上，调用select_by_name）
int QueryItemMenu::query_by_name(std::string name) const {
    if (name.empty()) {
        name = ui::input_string("请输入商品品种名称:");
    }

    const std::vector<Item> items = engine->select_by_name(name);
    if (items.empty()) {
        std::cout << "没有找到该商品" << std::endl;
        return 0;
    }

    for (const Item &item : items) {
        ui::show_item(item);
    }
    return -1;
}


int QueryItemMenu::query_by_name_like(std::string name) const {
    if (name.empty()) {
        name = ui::input_string("请输入商品品种名称:");
    }

    const std::vector<Item> items = engine->select_by_name_like(name);
    if (items.empty()) {
        std::cout << "没有找到该商品" << std::endl;
        return 0;
    }

    for (const Item &item : items) {
        ui::show_item(item);
    }
    return -1;
}


// 删除操作统一异常处理流程：
// try块内执行删除 -> 成功打印提示
// catch out_of_range异常 -> 失败提示
DeleteItemMenu::DeleteItemMenu(Engine *engine): BaseMenu(engine, "删除菜单: ", true) {
    menu.append(Option{"按商品品种名称删除", [this]{ return this->delete_by_name(); }});
    menu.append(Option{"按商品品种代码删除", [this]{ return this->delete_by_code(); }});
}


int DeleteItemMenu::delete_by_code(int code) const {
    if (code == -1) {
        code = ui::input_int("请输入商品品种代码:");
    }

    try {
        engine->del(code);
        std::cout << "删除成功" << std::endl;
    } catch (const std::out_of_range&) {
        std::cout << "删除失败" << std::endl;
    }
    return -1;
}


// 按名称删除特殊逻辑：
// 1. 先查询获取所有匹配项
// 2. 批量删除同名商品
// 3. 空结果时主动抛出异常触发失败提示
int DeleteItemMenu::delete_by_name(std::string name) const {
    if (name.empty()) {
        name = ui::input_string("请输入商品品种名称:");
    }

    try {
        const std::vector<Item> items = engine->select_by_name(name);
        if (items.empty()) {
            throw std::out_of_range("item not found");
        }

        for (const Item &item : items) {
            engine->del(item);
        }
        std::cout << "删除成功" << std::endl;
    } catch (const std::out_of_range&) {
        std::cout << "删除失败" << std::endl;
    }
    return -1;
}


Item ItemImportExport::item_quantity_change(const Item& old_item, const bool greater) {
    Item new_item;

    while (true) {
        new_item = ui::update_item(old_item);

        bool valid = true;
        for (const Brand& old_brand : old_item.brand_list) {
            // 查找对应新品牌
            auto it = std::find_if(new_item.brand_list.begin(), new_item.brand_list.end(),
                [&](const Brand& b){ return b.code == old_brand.code; });

            bool condition;
            if (greater) {
                condition = it->quantity < old_brand.quantity;
            }else {
                condition = it->quantity > old_brand.quantity;
            }

            // 校验条件
            if (it == new_item.brand_list.end() || condition) {  // 新数量不能小于旧数量
                std::cout << "错误：品牌" << old_brand.name;

                if (condition) {
                    std::cout << "库存不能增加！请重新输入" << std::endl;
                }else {
                    std::cout << "库存不能减少！请重新输入" << std::endl;
                }

                valid = false;
                break;
            }
        }

        if (valid) {
            break;
        }
    }

    return new_item;
}


std::string ItemImportExport::generate_header(bool is_import) {
    std::stringstream report;
    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);

    // 安全转换时间
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &time);  // Windows 安全版本
#else
    localtime_r(&time, &tm);  // Linux/macOS 安全版本
#endif

    if (is_import) {
        report << "======== 进货统计表 ========" << std::endl;
    }else {
        report << "======== 出货统计表 ========" << std::endl;
    }

    // 使用 std::put_time 格式化输出
    report << "生成时间: " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << std::endl << std::endl;

    // 表头
    if (is_import) {
        report << "|  商品品种  | 品牌名称  |  进货数量  |  单价    | 小计     |" << std::endl;
    }else {
        report << "|  商品品种  | 品牌名称  |  出货数量  |  单价    | 小计     |" << std::endl;
    }
    report << "|----------|----------|----------|---------|---------|" << std::endl;

    return report.str();
}


ExportItemMenu::ExportItemMenu(Engine *engine): BaseMenu(engine, "商品出库: ", true) {
    menu.append(Option{"按商品品种名称查询商品出库", [this]{ return this->export_by_name(); }});
    menu.append(Option{"按商品品种名称模糊查询商品出库", [this]{ return this->export_by_name_like(); }});
    menu.append(Option{"按商品品种代码查询商品出库", [this]{ return this->export_by_code(); }});
}


// 出库操作核心流程：
// 1. 查询目标商品（按code/name）
// 2. 调用ui::update_item更新库存量
// 3. 记录新旧商品到change列表
// 4. 批量更新引擎数据
int ExportItemMenu::export_by_code(int code) {
    if (code == -1) {
        code = ui::input_int("请输入商品品种代码:");
    }

    const std::vector<Item> item = engine->select_by_code(code);
    if (item.empty()) {
        std::cout << "没有找到该商品" << std::endl;
        return 0;
    }

    for (const Item& old_item : item) {
        Item new_item = item_quantity_change(old_item, false);
        change.emplace_back(old_item, new_item);
    }

    for (const std::pair<Item, Item> &pair : change) {
        engine->update(pair.second);
    }

    return -1;
}


int ExportItemMenu::export_by_name(std::string name) {
    if (name.empty()) {
        name = ui::input_string("请输入商品品种名称:");
    }

    const std::vector<Item> items = engine->select_by_name(name);
    if (items.empty()) {
        std::cout << "没有找到该商品" << std::endl;
        return 0;
    }
    for (const Item &old_item : items) {
        Item new_item = item_quantity_change(old_item, false);
        change.emplace_back(old_item, new_item);
    }

    for (const std::pair<Item, Item> &pair : change) {
        engine->update(pair.second);
    }

    return -1;
}


int ExportItemMenu::export_by_name_like(std::string name) {
    if (name.empty()) {
        name = ui::input_string("请输入商品品种名称:");
    }

    const std::vector<Item> items = engine->select_by_name_like(name);
    if (items.empty()) {
        std::cout << "没有找到该商品" << std::endl;
        return 0;
    }

    for (const Item &old_item : items) {
        Item new_item = item_quantity_change(old_item, false);
        change.emplace_back(old_item, new_item);
    }

    for (const std::pair<Item, Item> &pair : change) {
        engine->update(pair.second);
    }

    return -1;
}



// 生成出货报表关键技术点：
// 1. 跨平台时间处理（localtime_s/localtime_r）
// 2. 使用string stream构建格式化表格
// 3. 计算库存变化量（delta = old - new）
// 4. 只处理出货量>0的记录
std::string ExportItemMenu::generate() const {
    std::stringstream report;
    report << generate_header(false);

    double total = 0.0;
    for (const std::pair<Item, Item> &pair : change) {
        Item old_item = pair.first;
        Item new_item = pair.second;

        // 遍历每个品牌的变更
        for (const Brand &old_brand : old_item.brand_list) {
            // 查找对应的新品牌
            auto it = std::find_if(new_item.brand_list.begin(), new_item.brand_list.end(),
                [&old_brand](const Brand &b) { return b.code == old_brand.code;});

            if (it != new_item.brand_list.end()) {
                const int delta = old_brand.quantity - it->quantity;
                if (delta > 0) { // 只显示出货记录
                    const double subtotal = delta * old_brand.price;
                    report << "| " << old_item.name << "(" << old_item.code << ")"
                           << " | " << std::setw(8) << old_brand.name
                           << " | " << std::setw(8) << delta
                           << " | " << std::setw(8) << std::fixed << std::setprecision(2) << old_brand.price
                           << " | " << std::setw(7) << subtotal << " |" << std::endl;
                    total += subtotal;
                }
            }
        }
    }

    // 汇总信息
    report << std::endl << "总出货价格: " << std::fixed << std::setprecision(2) << total;
    return report.str();
}

ImportItemMenu::ImportItemMenu(Engine *engine): BaseMenu(engine, "商品进库: ", true) {
    menu.append(Option{"按商品品种名称查询商品进库", [this]{ return this->import_by_name(); }});
    menu.append(Option{"按商品品种名称模糊查询商品进库", [this]{ return this->import_by_name_like(); }});
    menu.append(Option{"按商品品种查询商品代码进库", [this]{ return this->import_by_code(); }});
}


int ImportItemMenu::import_by_code(int code) {
    if (code == -1) {
        code = ui::input_int("请输入商品品种代码:");
    }

    const std::vector<Item> items = engine->select_by_code(code);
    if (items.empty()) {
        std::cout << "没有找到该商品" << std::endl;
        return 0;
    }

    for (const Item& old_item : items) {
        Item new_item = item_quantity_change(old_item, true);
        change.emplace_back(old_item, new_item);
    }

    for (const std::pair<Item, Item> &pair : change) {
        engine->update(pair.second);
    }

    return -1;
}


int ImportItemMenu::import_by_name(std::string name) {
    if (name.empty()) {
        name = ui::input_string("请输入商品品种名称:");
    }

    const std::vector<Item> items = engine->select_by_name(name);
    if (items.empty()) {
        std::cout << "没有找到该商品" << std::endl;
        return 0;
    }

    for (const Item &old_item : items) {
        Item new_item = item_quantity_change(old_item, true);
        change.emplace_back(old_item, new_item);
    }

    for (const std::pair<Item, Item> &pair : change) {
        engine->update(pair.second);
    }

    return -1;
}


int ImportItemMenu::import_by_name_like(std::string name) {
    if (name.empty()) {
        name = ui::input_string("请输入商品品种名称:");
    }

    for (const Item &old_item : engine->select_by_name_like(name)) {
        Item new_item = item_quantity_change(old_item, true);
        change.emplace_back(old_item, new_item);
    }

    for (const std::pair<Item, Item> &pair : change) {
        engine->update(pair.second);
    }

    return -1;
}



std::string ImportItemMenu::generate() const{
    std::stringstream report;
    report << generate_header(true);

    for (const std::pair<Item, Item> &pair : change) {
        Item old_item = pair.first;
        Item new_item = pair.second;

        // 遍历每个品牌的变更
        for (const Brand& old_brand : old_item.brand_list) {
            // 查找对应的新品牌
            auto it = std::find_if(new_item.brand_list.begin(), new_item.brand_list.end(),
                [&](const Brand& b){ return b.code == old_brand.code; });

            if (it != new_item.brand_list.end()) {
                const int delta = it->quantity - old_brand.quantity; // 计算进货量
                if (delta > 0) { // 只显示有效进货记录
                    const double subtotal = delta * old_brand.price;
                    report << "| " << old_item.name << "(" << old_item.code << ")"
                           << " | " << std::setw(8) << old_brand.name
                           << " | " << std::setw(8) << delta
                           << " | " << std::setw(8) << std::fixed << std::setprecision(2) << old_brand.price
                           << " | " << std::setw(7) << subtotal << " |" << std::endl;
                }
            }
        }
    }

    return report.str();
}


// 构造函数关键初始化：
// 1. 引擎参数：最大品牌10，商品100，日志文件operation.log
// 2. 初始化所有子菜单实例
// 3. 构建6个主功能选项
Main::Main(): BaseMenu(&engine, "主菜单:", true),
              engine(10, 100, "operation.log", "data.csv"),
              query_item_menu(&engine), delete_item_menu(&engine),
              export_item_menu(&engine), import_item_menu(&engine) {
    menu.append(Option{"添加商品品种", [this]{ return this->add_item(); }});
    menu.append(Option{"显示商品品种", [this]{ return this->show_item();}});
    menu.append(Option{"查询商品", [this]{ return this->query_item();}});
    menu.append(Option{"删除商品", [this]{ return this->delete_item();}});
    menu.append(Option{"商品出库", [this]{ return this->export_item();}});
    menu.append(Option{"商品进库", [this]{ return this->import_item();}});
}


int Main::add_item() {
    const Item item = ui::get_item();
    engine.insert(item);
    return 0;
}


int Main::show_item() {
    const std::vector<Item> items = engine.select().all();
    if (items.empty()) {
        std::cout << "没有商品" << std::endl;
    }

    for (const Item &item: items) {
        ui::show_item(item);
    }
    return 0;
}



int Main::query_item() const {
    return query_item_menu.main();
}


int Main::delete_item() const {
    return delete_item_menu.main();
}


int Main::export_item() const {
    return export_item_menu.main();
}


int Main::import_item() const {
    return import_item_menu.main();
}


// 主程序退出处理：
// 1. 生成出货/进货报表
// 2. 写入export_list.txt/import_list.txt
// 3. 使用trunc模式覆盖已有文件
// 4. 输出保存成功提示
int Main::main() const {
    const int state = BaseMenu::main();
    if (state == 0) {
        std::ofstream export_list("export_list.txt", std::ios::out | std::ios::trunc);
        export_list << export_item_menu.generate();
        export_list.close();

        std::ofstream import_list("import_list.txt", std::ios::out | std::ios::trunc);
        import_list << import_item_menu.generate();
        import_list.close();

        std::cout << "数据已保存至文件: export_list.txt, import_list.txt" << std::endl;
    }

    return state;
}

