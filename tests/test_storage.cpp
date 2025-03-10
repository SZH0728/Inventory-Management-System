#include <gtest/gtest.h>
#include <cstdio>
#include "../include/storage.h"

// 测试基类 BaseFile
TEST(BaseFileTest, FileLifecycle) {
    BaseFile file("test.txt");

    // 测试初始状态
    EXPECT_FALSE(file.close_file_object()); // 未打开时关闭应失败

    // 测试打开文件
    EXPECT_TRUE(file.open_file_object());
    EXPECT_FALSE(file.open_file_object()); // 重复打开失败
}

// 测试WriteDataFile类
TEST(WriteDataFileTest, WriteItems) {
    WriteDataFile file("test.txt");

    std::list<Item> items;
    items.emplace_back(Item{"Item1", 1, "Red", 10});
    items.back().brand_number = 1;
    items.back().brand_list.emplace_back(Brand{"Brand1", 101, 5, 9.99});

    items.emplace_back(Item{"Item2", 2, "Blue", 20});
    items.back().brand_number = 2;
    items.back().brand_list.emplace_back(Brand{"Brand2", 102, 10, 12.99});
    items.back().brand_list.emplace_back(Brand{"Brand3", 103, 15, 14.99});

    ASSERT_TRUE(file.open_file_object());
    ASSERT_TRUE(file.write(items));
    file.close_file_object();
}

// 测试ReadDataFile类
TEST(ReadDataFileTest, ReadItems) {
    ReadDataFile file("test.txt");
    ASSERT_TRUE(file.open_file_object());

    std::list<Item> items = file.read();
    ASSERT_EQ(items.size(), 2);

    auto it = items.begin();
    ASSERT_EQ(it->name, "Item1");
    ASSERT_EQ(it->code, 1);
    ASSERT_EQ(it->colour, "Red");
    ASSERT_EQ(it->quantity, 10);
    ASSERT_EQ(it->brand_list.size(), 1);
    ASSERT_EQ(it->brand_list.front().name, "Brand1");

    ++it;
    ASSERT_EQ(it->name, "Item2");
    ASSERT_EQ(it->code, 2);
    ASSERT_EQ(it->colour, "Blue");
    ASSERT_EQ(it->quantity, 20);
    ASSERT_EQ(it->brand_list.size(), 2);

    file.close_file_object();
    std::remove("test.txt");
}

// 测试OperationFile类
TEST(OperationFileTest, AppendPop) {
    OperationFile file("log.txt");
    ASSERT_TRUE(file.open_file_object());

    ASSERT_TRUE(file.append("Operation 1"));
    ASSERT_TRUE(file.append("Operation 2"));
    ASSERT_EQ(file.pop(), "Operation 2");
    ASSERT_EQ(file.pop(), "Operation 1");
    ASSERT_EQ(file.pop(), "FIE");

    file.close_file_object();
    std::remove("log.txt");
}

TEST(OperationFileTest, Clear) {
    OperationFile file("log.txt");
    ASSERT_TRUE(file.open_file_object());

    file.append("Operation 1");
    file.append("Operation 2");

    const std::list<std::string> logs = file.clear();
    ASSERT_EQ(logs.size(), 2);
    ASSERT_EQ(logs.front(), "Operation 1");
    ASSERT_EQ(logs.back(), "Operation 2");

    file.close_file_object();
    std::remove("log.txt");
}

// int main(int argc, char* argv[]) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
