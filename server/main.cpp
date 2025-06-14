#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <iostream>

#include "absl/log/initialize.h"
#include "logs.grpc.pb.h"
#include "logs.pb.h"

using grpc::Server;
// using grpc::ServerBuilder;
// using grpc::ServerContext;
using grpc::Status;

class LogServerImpl final : public logger::LogServer::Service {
  Status Log(::grpc::ServerContext* context, const ::logger::LogEntry* request,
             ::logger::LogAck* response) {
    std::cout << "Got message: " << request->message()
              << " Level:" << request->level() << '\n';
    response->set_success(true);
    return Status::OK;
  }
};

void runServer() {
  std::string address = "0.0.0.0:50051";
  LogServerImpl logServer;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(address, grpc::InsecureServerCredentials());
  builder.RegisterService(&logServer);
  std::unique_ptr<grpc::Server> server = builder.BuildAndStart();

  std::cout << "Server listening on:" << address << '\n';
  server->Wait();
}

int main() {
  LogServerImpl server;

  //   absl::InitializeLog();

  runServer();

  return 0;
}