file(GLOB LIBLOGGER_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB LIBLOGGER_HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB LIBLOGGER_HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.tpp")

execute_process(
    COMMAND getconf PAGESIZE
    OUTPUT_VARIABLE PAGE_SIZE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)


add_library(liblogger STATIC
  ${LIBLOGGER_SRCS}
  ${LIBLOGGER_HDRS}
)

target_compile_definitions(liblogger PUBLIC PAGE_SIZE=${PAGE_SIZE})
# Включаем директорию с заголовочными файлами
target_include_directories(liblogger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(liblogger PRIVATE libproto)
