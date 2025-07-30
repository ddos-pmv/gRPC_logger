# Пути
set(PROTO_DIR ${CMAKE_SOURCE_DIR}/proto)
set(GENERATED_DIR ${CMAKE_SOURCE_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

# Генерация gRPC и protobuf файлов
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# message(STATUS "Protobuf_VERSION: ${Protobuf_VERSION}")
# message(STATUS "Protobuf_INCLUDE_DIR: ${Protobuf_INCLUDE_DIR}")
# message(STATUS "Protobuf_LIBRARIES: ${Protobuf_LIBRARIES}")
# message(STATUS "Protobuf_PROTOC_EXECUTABLE: ${Protobuf_PROTOC_EXECUTABLE}")
# message(STATUS "Protobuf_PROTOC_VERSION: ${Protobuf_PROTOC_VERSION}")


# message(STATUS "gRPC_VERSION: ${gRPC_VERSION}")
# message(STATUS "gRPC_INCLUDE_DIR: ${gRPC_INCLUDE_DIR}")
# message(STATUS "gRPC_LIBRARIES: ${gRPC_LIBRARIES}")
# message(STATUS "gRPC_PROTOC_EXECUTABLE: ${gRPC_PROTOC_EXECUTABLE}")
# message(STATUS "gRPC_PROTOC_VERSION: ${gRPC_PROTOC_VERSION}")

# set(Protobuf_INCLUDE_DIR "$ENV{HOME}/.local/include")
# set(Protobuf_LIBRARY "$ENV{HOME}/.local/lib/libprotobuf.a")
# set(Protobuf_PROTOC_EXECUTABLE "$ENV{HOME}/.local/bin/protoc")

# Protobuf и gRPC файлы
file(GLOB PROTO_FILES "${PROTO_DIR}/*.proto")

add_library(libproto ${PROTO_FILES})

target_link_libraries(libproto PUBLIC protobuf::libprotobuf gRPC::grpc++)
target_include_directories(libproto PUBLIC ${GENERATED_DIR})

# Генерация исходных файлов из .proto
protobuf_generate(
    TARGET libproto 
    LANGUAGE cpp
    OUT_VAR CPP_GEN_SRCS
    IMPORT_DIRS ${PROTO_DIR}
    PROTOC_OUT_DIR ${GENERATED_DIR}
)
protobuf_generate(
    TARGET libproto
    LANGUAGE grpc
    PLUGIN protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    IMPORT_DIRS ${PROTO_DIR}
    PROTOC_OUT_DIR ${GENERATED_DIR}
)