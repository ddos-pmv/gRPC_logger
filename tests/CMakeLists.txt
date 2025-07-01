find_package(GTest REQUIRED)

enable_testing()

if (NOT GTest_FOUND)
        message(FATAL_ERROR "GTest not found!")
endif()


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/toy-queue)

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