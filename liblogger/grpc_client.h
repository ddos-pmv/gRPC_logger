#pragma once

#include <logs.grpc.pb.h>
#include <logs.pb.h>

#include <memory>

namespace logger {
class GrpcClient {
  GrpcClient(const std::string& adress) : stub_(logger::LogServer::NewStub) {}

 private:
  std::unique_ptr<logger::LogServer::Stub> stub_;
};
}  // namespace logger