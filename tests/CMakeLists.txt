cmake_minimum_required(VERSION 3.25)

project(TaskQueueTests LANGUAGES CXX)

find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME}
    TaskQueueTests.cpp
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  TaskQueue::TaskQueue
  spdlog::spdlog
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
