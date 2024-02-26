cmake_minimum_required(VERSION 3.15)
project(WrappedTask CXX)



add_executable(WrappedTask src/WrappedTask.cpp src/main.cpp)



install(TARGETS WrappedTask DESTINATION "."
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        )
