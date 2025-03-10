#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>

#include "../include/persister.h"

class PersistTest : public ::testing::Test {
protected:
    std::string data_file_path = "test_data.txt";
    std::string operation_file_path = "test_operations.txt";
    Persist* persist = nullptr;

    void SetUp() override {
        std::remove(data_file_path.c_str());
        std::remove(operation_file_path.c_str());
        persist = new Persist(data_file_path, operation_file_path, 10);
    }

    void TearDown() override {
        persist->close();
        delete persist;
        std::remove(data_file_path.c_str());
        std::remove(operation_file_path.c_str());
    }
};

TEST_F(PersistTest, SelectEmptyList) {
    const std::list<Item> items = persist->select();
    EXPECT_TRUE(items.empty());
}

TEST_F(PersistTest, InsertAndSelect) {
    const Item item1 = {"夏季短袖T恤",1001,"珊瑚红",150,{Brand{"棉质世家", 2001, 80, 89.99f},Brand{"简约风", 2002, 70, 79.50f}},2};;
    persist->insert(item1);

    const std::list<Item> items = persist->select();
    ASSERT_EQ(items.size(), 1);
    EXPECT_EQ(items.front().code, item1.code);
}

TEST_F(PersistTest, UpdateAndSelect) {
    const Item item1 = {"夏季短袖T恤",1001,"珊瑚红",150,{Brand{"棉质世家", 2001, 80, 89.99f},Brand{"简约风", 2002, 70, 79.50f}},2};;
    persist->insert(item1);

    const Item item2 = {"破洞牛仔裤",1001,"水洗蓝",75,{Brand{"Levi's", 3001, 30, 399.0f},Brand{"Lee", 3002, 25, 359.0f},Brand{"七匹狼", 3003, 20, 289.0f}},3};
    persist->update(item2);

    const std::list<Item> items = persist->select();
    ASSERT_EQ(items.size(), 1);
    EXPECT_EQ(items.front().code, item2.code);
}

TEST_F(PersistTest, DeleteAndSelect) {
    const Item item1 = {"夏季短袖T恤",1001,"珊瑚红",150,{Brand{"棉质世家", 2001, 80, 89.99f},Brand{"简约风", 2002, 70, 79.50f}},2};;
    persist->insert(item1);

    persist->del(1001);

    const std::list<Item> items = persist->select();
    EXPECT_TRUE(items.empty());
}

TEST_F(PersistTest, FlushAndSelect) {
    const Item item1 = {"夏季短袖T恤",1001,"珊瑚红",150,{Brand{"棉质世家", 2001, 80, 89.99f},Brand{"简约风", 2002, 70, 79.50f}},2};;
    persist->insert(item1);

    const Item item2 = {"破洞牛仔裤",2005,"水洗蓝",75,{Brand{"Levi's", 3001, 30, 399.0f},Brand{"Lee", 3002, 25, 359.0f},Brand{"七匹狼", 3003, 20, 289.0f}},3};
    persist->insert(item2);

    persist->flush();

    std::list<Item> items = persist->select();
    ASSERT_EQ(items.size(), 2);
    auto it = items.begin();
    EXPECT_EQ(*it++, item1);
    EXPECT_EQ(*it, item2);
}

TEST_F(PersistTest, CloseAndReopenSelect) {
    const Item item1 = {"夏季短袖T恤",1001,"珊瑚红",150,{Brand{"棉质世家", 2001, 80, 89.99f},Brand{"简约风", 2002, 70, 79.50f}},2};;
    persist->insert(item1);

    persist->close();
    persist = new Persist(data_file_path, operation_file_path, 10);

    const std::list<Item> items = persist->select();
    ASSERT_EQ(items.size(), 1);
    EXPECT_EQ(items.front().code, item1.code);
}

// int main(int argc, char* argv[]) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }