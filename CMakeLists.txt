cmake_minimum_required(VERSION 3.28)
project(C)
set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEAD "include/*.h")
add_executable(main ${SOURCES} ${HEAD})

#set(CMAKE_BUILD_TYPE Release)
#
#file(GLOB TEST_SOURCES "tests/*.cpp")
#
#file(GLOB SOURCES "src/*.cpp")
#file(GLOB HEAD "include/*.h")
#add_executable(my_test ${TEST_SOURCES} ${SOURCES} ${HEAD})
#target_compile_definitions(my_test PRIVATE TEST_MODE)
#
#find_package(GTest REQUIRED CONFIG)
#target_link_libraries(my_test PRIVATE GTest::gtest GTest::gtest_main)