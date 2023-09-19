cmake_minimum_required(VERSION 3.25)

project(DadabaseExample LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE 
  Dadabase::Dadabase
  spdlog::spdlog
  #  Crow::Crow
)

file(COPY ${CMAKE_SOURCE_DIR}/dadoneliners.txt 
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

