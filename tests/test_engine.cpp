#include <gtest/gtest.h>
#include <fstream>
#include "../include/engine.h"

const std::string TEST_DATA_FILE = "test_data.csv";
const std::string TEST_LOG_FILE = "test_log.csv";

// 测试固件类
class EngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 清理测试文件
        std::remove(TEST_DATA_FILE.c_str());
        std::remove(TEST_LOG_FILE.c_str());

        // 初始化引擎（缓存大小3，日志上限5）
        engine = new Engine(3, 5, TEST_LOG_FILE, TEST_DATA_FILE);
    }

    void TearDown() override {
        delete engine;
        std::remove(TEST_DATA_FILE.c_str());
        std::remove(TEST_LOG_FILE.c_str());
    }

    static Item createTestItem(int code) {
        return {"Item" + std::to_string(code), code, "Red", 100, {}, 0};
    }

    Engine* engine = nullptr;
};

// 测试插入功能
TEST_F(EngineTest, InsertItem) {
    Item newItem = createTestItem(1);
    engine->insert(newItem);

    auto result = engine->select_by_code(1);
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].code, 1);
}

// 测试删除功能
TEST_F(EngineTest, DeleteItem) {
    Item item = createTestItem(2);
    engine->insert(item);

    engine->del(2);
    EXPECT_EQ(engine->select_by_code(2).size(), 0);
}

// 测试更新功能
TEST_F(EngineTest, UpdateItem) {
    Item item = createTestItem(3);
    engine->insert(item);

    item.name = "UpdatedItem";
    engine->update(item);

    auto result = engine->select_by_code(3);
    EXPECT_EQ(result[0].name, "UpdatedItem");
}

// 测试缓存功能
TEST_F(EngineTest, CacheBehavior) {
    // 插入3个Item填满缓存
    engine->insert(createTestItem(4));
    engine->insert(createTestItem(5));
    engine->insert(createTestItem(6));

    // 访问前两个保持活跃
    engine->select_by_code(4);
    engine->select_by_code(5);

    // 插入第四个Item触发缓存淘汰
    engine->insert(createTestItem(7));

    EXPECT_EQ(engine->select_by_code(6)[0].code, 6);
}

// 测试复合查询
TEST_F(EngineTest, ComplexQuery) {
    for(int i=10; i<20; i++) {
        engine->insert({ "TestItem", i, "Blue", i*10, {}, 0 });
    }

    auto results = engine->select()
        .where([](const Item& item) { return item.quantity > 150; })
        .where([](const Item& item) { return item.code < 20; })
        .limit(2);

    EXPECT_EQ(results.size(), 2);
    for(const auto& item : results) {
        EXPECT_GT(item.quantity, 150);
        EXPECT_LT(item.code, 18);
    }
}

// 测试异常处理
TEST_F(EngineTest, InvalidOperations) {
    EXPECT_THROW(engine->del(999), std::out_of_range);
}

// 测试持久化恢复
TEST_F(EngineTest, PersistenceRecovery) {
    { // 第一个作用域
        Engine localEngine(3, 5, TEST_LOG_FILE, TEST_DATA_FILE);
        localEngine.insert(createTestItem(9));
    } // 析构时自动持久化

    // 重新创建引擎验证数据恢复
    Engine newEngine(3, 5, TEST_LOG_FILE, TEST_DATA_FILE);
    auto result = newEngine.select_by_code(9);
    EXPECT_EQ(result.size(), 1);
}

// 测试模糊查询
TEST_F(EngineTest, LikeQuery) {
    engine->insert({"Apple", 20, "Red", 50, {}, 0});
    engine->insert({"App", 21, "Green", 30, {}, 0});
    engine->insert({"Banana", 22, "Yellow", 40, {}, 0});

    auto results = engine->select_by_name_like("App");
    EXPECT_EQ(results.size(), 2);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}