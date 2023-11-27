cmake_minimum_required(VERSION 3.23)
project(http_lib)

set(CMAKE_CXX_STANDARD 14)

include(GenerateExportHeader)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
find_package(spdlog REQUIRED)

set(TargetName ${PROJECT_NAME})

message(TargetName = ${TargetName})

add_library(${TargetName} SHARED core/http_connect/include/http_connect.h core/http_connect/src/http_connect.cpp test/httpConnectUnitTest.cpp core/thread_pool/thread_pool.h)

#链接libcurl
target_link_libraries(${TargetName} ${CURL_LIBRARIES})
#链接Google Test
target_link_libraries(${TargetName} GTest::GTest GTest::Main)
#链接spdlog
target_link_libraries(${TargetName} ${CURL_LIBRARIES} spdlog::spdlog)

#export宏定义, 生成的文件名为 http_lib_export.h, 路径: ${PROJECT_BINARY_DIR}
GENERATE_EXPORT_HEADER(${TargetName})
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

message(PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR})

#设置库安装目录
SET(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/${PROJECT_NAME})
# 拷贝头文件 到 include 目录下
INSTALL(FILES core/http_connect/include/http_connect.h core/thread_pool/thread_pool.h ${PROJECT_BINARY_DIR}/http_lib_export.h  DESTINATION include)
# 将 dll 和 lib 库文件 分别放到 lib 和 bin 目录下
INSTALL(TARGETS ${TargetName} LIBRARY DESTINATION lib)





