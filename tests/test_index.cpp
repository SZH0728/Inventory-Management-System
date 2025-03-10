#include "gtest/gtest.h"
#include "../include/index.h"

class IndexTest : public ::testing::Test {
protected:
    Index index;

    void SetUp() override {
        // 公共初始化
        index.insert("apple", 1001);
        index.insert("banana", 1002);
        index.insert("orange", 1003);
        index.insert("grape", 1004);
    }
};

TEST_F(IndexTest, InsertAndSelect) {
    // 测试正常插入和查询
    index.insert("test", 999);
    EXPECT_EQ(index.select("test"), 999);

    // 测试重复插入覆盖
    index.insert("apple", 1005);
    EXPECT_EQ(index.select("apple"), 1005);
}

TEST_F(IndexTest, SelectException) {
    // 测试异常抛出
    EXPECT_THROW(index.select("unknown"), std::out_of_range);
}

TEST_F(IndexTest, DeleteEntries) {
    // 测试删除存在的条目
    auto deleted = index.del(1002);
    ASSERT_EQ(deleted.size(), 1);
    EXPECT_EQ(deleted[0], "banana");
    EXPECT_THROW(index.select("banana"), std::out_of_range);

    // 测试删除不存在的条目
    auto empty = index.del(9999);
    EXPECT_TRUE(empty.empty());
}

TEST_F(IndexTest, FuzzySearch) {
    // 精确匹配测试
    const auto exact = index.find("apple", 0);
    ASSERT_EQ(exact.size(), 1);
    EXPECT_EQ(exact[0], 1001);

    // 模糊匹配测试（编辑距离3）
    auto fuzzy = index.find("aple", 3);
    ASSERT_TRUE(!fuzzy.empty());
    EXPECT_TRUE(std::find(fuzzy.begin(), fuzzy.end(), 1001) != fuzzy.end());

    // 无匹配测试
    auto none = index.find("xyz", 1);
    EXPECT_TRUE(none.empty());
}

TEST_F(IndexTest, MultiCodeHandling) {
    // 测试同一编码多个条目
    index.insert("berry", 1005);
    index.insert("cherry", 1005);

    const auto deleted = index.del(1005);
    ASSERT_EQ(deleted.size(), 2);
    EXPECT_THROW(index.select("berry"), std::out_of_range);
    EXPECT_THROW(index.select("cherry"), std::out_of_range);
}

// int main(int argc, char* argv[]) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
