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

# Include ExternalProject module
include(ExternalProject)


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

# Fetch CRCpp
FetchContent_Declare(
  CRCpp
  GIT_REPOSITORY https://github.com/d-bahr/CRCpp.git
  GIT_TAG release-1.2.0.0  
)
FetchContent_MakeAvailable(CRCpp)


ExternalProject_Add(
    sqlparser_project                 # Name for the external project
    PREFIX ${CMAKE_BINARY_DIR}/_deps   # Download location
    GIT_REPOSITORY https://github.com/hyrise/sql-parser.git     # Repository URL if you're pulling from GitHub
    GIT_TAG master                     # Optionally specify a tag or branch
    SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/src/sqlparser"   # Source directory
    CONFIGURE_COMMAND ""              # Disable default configure step
    BUILD_COMMAND make                # Build command 
    INSTALL_COMMAND make install      # Installation command
    BUILD_IN_SOURCE 1                 # Build in source
)


# Fetch Google Test and Google Mock
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)

#############################################################################

# Define variables for directories
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)


# Add include directories

# 


# Automatically include all source files from the src directory
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# Define library
add_library(AumDbLib ${SOURCES})
target_include_directories(AumDbLib PUBLIC ${INCLUDE_DIR})
target_include_directories(AumDbLib PUBLIC  /usr/local/include)
add_dependencies(AumDbLib sqlparser_project)
find_library(SQL_PARSER_LIB sqlparser HINTS /usr/local/lib)

target_link_libraries(AumDbLib PUBLIC spdlog::spdlog nlohmann_json::nlohmann_json CRCpp ${SQL_PARSER_LIB})

# Define executable
add_executable(AumDb src/main.cpp)

target_link_libraries(AumDb PRIVATE AumDbLib)

add_subdirectory(tests)

# Enable testing for the project
enable_testing()

# Copy config.json to the build directory
add_custom_command(TARGET AumDb POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/config.json
                   $<TARGET_FILE_DIR:AumDb>)

add_custom_target(buildWithoutTests 
                   COMMENT "Building project without running tests"
                   DEPENDS AumDb AumDbTests AumDbLib
)
 
                   
add_custom_target(runTests ALL
                   COMMAND ctest -C $<CONFIGURATION> --output-on-failure
                   DEPENDS AumDbTests
               )
               
