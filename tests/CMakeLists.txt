include(GoogleTest)

# Create a test executable target
add_executable(AumDbTests
    db_tests.cpp
)

# Link the test executable with necessary libraries
target_link_libraries(AumDbTests PRIVATE gtest gtest_main gmock gmock_main AumDbLib)

# Use gtest_discover_tests for test discovery
gtest_discover_tests(AumDbTests)

add_custom_command(TARGET AumDbTests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/config.json
                   $<TARGET_FILE_DIR:AumDbTests>)


add_custom_command(TARGET AumDbTests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/config.json
                   ${CMAKE_BINARY_DIR}) # ctest runs here by default


