file(GLOB LIBLOGGER_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB LIBLOGGER_HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
# file(GLOB LIBLOGGER_TPLS "${CMAKE_CURRENT_SOURCE_DIR}/*.tpp")

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
# Включаем директорию с заголовочными файлами
target_include_directories(liblogger INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(liblogger INTERFACE libproto)
