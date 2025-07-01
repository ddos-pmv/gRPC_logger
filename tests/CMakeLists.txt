find_package(GTest REQUIRED)

enable_testing()

if (NOT GTest_FOUND)
        message(FATAL_ERROR "GTest not found!")
endif()


add_library(toy-queue STATIC
  toy-queue/queue.c
)

set_target_properties(toy-queue PROPERTIES
    LINKER_LANGUAGE C
)

target_include_directories(toy-queue
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/toy-queue
)

target_link_libraries(toy-queue
  PUBLIC
  pthread
)

file(GLOB TEST_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(logger_tests ${TEST_SRCS})


target_link_libraries(logger_tests
  PRIVATE liblogger
  PRIVATE GTest::GTest
  PRIVATE toy-queue
)

gtest_discover_tests(logger_tests)

add_test(NAME logger_tests
  COMMAND logger_tests
)