cmake_minimum_required(VERSION 3.25)

project(TaskQueueExample LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE 
    TaskQueue::TaskQueue
    spdlog::spdlog
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
