cmake_minimum_required(VERSION 3.25)

project(DadabaseTests LANGUAGES CXX)

find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME}
    DadabaseTest.cpp
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  spdlog::spdlog
  Dadabase::Dadabase
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
