#include "../include/cache.h"
#include <gtest/gtest.h>

TEST(LRUCacheTest, ConstructorSetsCapacity) {
    LRUCache cache(5);
    // 验证初始容量设置
    for(int i = 0; i < 6; ++i) {
        cache.insert(Item{"item" + std::to_string(i), i, "red", 10, {}, 0});
    }
    EXPECT_EQ(cache.select(1).name, "item1");
    EXPECT_THROW(cache.select(0), std::out_of_range);
}

TEST(LRUCacheTest, InsertAndSelect) {
    LRUCache cache(2);
    Item item1{"item1", 1, "blue", 5, {}, 0};
    Item item2{"item2", 2, "green", 3, {}, 0};

    cache.insert(item1);
    cache.insert(item2);

    // 验证正常查询
    EXPECT_EQ(cache.select(1).name, "item1");
    EXPECT_EQ(cache.select("item2").code, 2);

    // 测试LRU淘汰
    cache.insert(Item{"item3", 3, "black", 2, {}, 0});
    EXPECT_THROW(cache.select(1), std::out_of_range); // item1应被淘汰
}

TEST(LRUCacheTest, UpdateExistingItem) {
    LRUCache cache(3);
    Item item1{"item1", 1, "red", 10, {}, 0};
    cache.insert(item1);

    // 更新现有项
    Item updated{"newName", 1, "blue", 5, {}, 0};
    cache.insert(updated);

    // 验证更新效果
    Item selected = cache.select(1);
    EXPECT_EQ(selected.name, "newName");
    EXPECT_EQ(selected.colour, "blue");
}

TEST(LRUCacheTest, DeleteOperations) {
    LRUCache cache(3);
    cache.insert(Item{"item1", 1, "red", 10, {}, 0});
    cache.insert(Item{"item2", 2, "blue", 5, {}, 0});

    // 删除存在的项
    EXPECT_TRUE(cache.del(1));
    EXPECT_THROW(cache.select(1), std::out_of_range);

    // 删除不存在的项
    EXPECT_FALSE(cache.del(99));

    // 通过名称删除
    EXPECT_TRUE(cache.del("item2"));
    EXPECT_THROW(cache.select(2), std::out_of_range);
}

TEST(LRUCacheTest, LRUOrderMaintenance) {
    LRUCache cache(3);
    cache.insert({"a", 1, "red", 1, {}, 0});
    cache.insert({"b", 2, "blue", 2, {}, 0});
    cache.insert({"c", 3, "green", 3, {}, 0});

    cache.select(1);

    // 插入新元素触发淘汰
    cache.insert({"d", 4, "black", 4, {}, 0});

    // 验证淘汰的是最早未访问的b
    EXPECT_THROW(cache.select(2), std::out_of_range);
    EXPECT_NO_THROW(cache.select(1)); // a应保留
}

// int main(int argc, char* argv[]) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
