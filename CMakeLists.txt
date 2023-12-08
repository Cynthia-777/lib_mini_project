cmake_minimum_required(VERSION 3.23)
project(http_lib)

set(CMAKE_CXX_STANDARD 14)

# 添加 Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

find_package(spdlog REQUIRED)
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})


# 添加动态库源文件
add_library(http_lib SHARED
        core/http_connect/include/http_connect.h
        core/http_connect/src/http_connect.cpp
        core/thread_pool/thread_pool.h
        logger/include/logger.h
        logger/src/logger.cpp
        )

# 链接 Google Test 和动态库
add_executable(unit_tests
        test/httpConnectUnitTest.cpp
        )

target_include_directories(http_lib
        PUBLIC
        core/http_connect/include
        logger/include
        )
target_include_directories(unit_tests
        PRIVATE
        core/http_connect/include
        logger/include
        )

target_link_libraries(unit_tests
        http_lib
        GTest::GTest
        GTest::Main
        )

target_link_libraries(http_lib ${CURL_LIBRARIES} spdlog::spdlog)
target_link_libraries(http_lib ${CURL_LIBRARIES})

# 使用 gtest_discover_tests 宏
include(GoogleTest)
gtest_discover_tests(unit_tests)
