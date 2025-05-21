cmake_minimum_required(VERSION 3.10)
project(RFIDLabAttendance)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)
find_package(nlohmann_json 3.11.2 REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
set(SOURCES
    src/main.cpp
    src/AttendanceSystem.cpp
)

# Add executable
add_executable(rfid_system ${SOURCES})

# Link libraries
target_link_libraries(rfid_system PRIVATE 
    Threads::Threads
    nlohmann_json::nlohmann_json
)