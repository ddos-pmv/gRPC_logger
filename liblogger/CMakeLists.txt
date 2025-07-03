find_package(Boost REQUIRED)

if (NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found. Please install Boost or set BOOST_ROOT.")
endif()

file(GLOB LIBLOGGER_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB LIBLOGGER_TPLS "${CMAKE_CURRENT_SOURCE_DIR}/*.tpp")
file(GLOB LIBLOGGER_HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

execute_process(
    COMMAND getconf PAGESIZE
    OUTPUT_VARIABLE PAGE_SIZE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_library(liblogger INTERFACE
  ${LIBLOGGER_SRCS}
  ${LIBLOGGER_HDRS}
  ${LIBLOGGER_TPLS}
)
target_compile_definitions(liblogger INTERFACE PAGE_SIZE=${PAGE_SIZE})


target_link_libraries(liblogger INTERFACE Boost::boost libproto)

# Включаем директорию с заголовочными файлами
target_include_directories(liblogger INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

