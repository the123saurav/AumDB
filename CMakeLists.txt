# Minimum required version of CMake
cmake_minimum_required(VERSION 3.19)

# Project name and language
project(AumDb CXX)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*)
set(CMAKE_CXX_CLANG_TIDY
  clang-tidy;
  -checks='*'
)


## Dependencies

# Include FetchContent module
include(FetchContent)

# Fetch spdlog
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.8.5  # Specify a version
)
FetchContent_MakeAvailable(spdlog)

# Fetch nlohmann_json
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2  # Specify a version
)
FetchContent_MakeAvailable(nlohmann_json)

# Define variables for directories
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Add include directories
include_directories(${INCLUDE_DIR})

# Automatically include all source files from the src directory
file(GLOB SOURCES "${SRC_DIR}/*.cpp")


# Define executable
add_executable(AumDb ${SOURCES})

target_link_libraries(AumDb PRIVATE spdlog::spdlog)
target_link_libraries(AumDb PRIVATE nlohmann_json::nlohmann_json)

# Copy config.json to the build directory
add_custom_command(TARGET AumDb POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/config.json
                   $<TARGET_FILE_DIR:AumDb>)
