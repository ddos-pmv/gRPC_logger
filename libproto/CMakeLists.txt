# Пути
set(PROTO_DIR ${CMAKE_SOURCE_DIR}/proto)
set(GENERATED_DIR ${CMAKE_SOURCE_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

# Генерация gRPC и protobuf файлов
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Protobuf и gRPC файлы
file(GLOB PROTO_FILES "${PROTO_DIR}/*.proto")

add_library(libproto ${PROTO_FILES})

target_link_libraries(libproto PUBLIC gRPC::grpc++)
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