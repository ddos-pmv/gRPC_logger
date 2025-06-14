file(GLOB LIBLOGGER_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB LIBLOGGER_HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(liblogger STATIC
  ${LIBLOGGER_SRCS}
  ${LIBLOGGER_HDRS}
)

# Включаем директорию с заголовочными файлами
target_include_directories(liblogger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(liblogger PRIVATE libproto)
