#include <arm_neon.h>
#include <grpcpp/create_channel.h>
#include <logger.h>

#include <memory>
// #include "logs.grpc.pb.h"
// #include "logs.pb.h"

// using logger::LogEntry;
// using logger::LogServer;

// class LogClient {
//   std::unique_ptr<LogServer::Stub> stub;

//  public:
//   LogClient(std::shared_ptr<grpc::Channel> channel)
//       : stub(LogServer::NewStub(channel)) {}
//   void SendLog(const std::string& msg) {
//     LogEntry logRequest;
//     logRequest.set_message(msg);
//     logRequest.set_level("INFO");
//     logRequest.set_timestamp(std::to_string(std::time(nullptr)));

//     grpc::ClientContext context;

//     logger::LogAck ack;
//     ack.set_success(false);
//     grpc::Status replyStatus = stub->Log(&context, logRequest, &ack);

//     if (replyStatus.ok()) {
//       std::cout << "status: ok, ack: " << ack.success() << '\n';
//     } else {
//       std::cout << "status: !ok, ack: " << ack.success() << '\n';
//     }
//   }
// };
#include <string>
#include <thread>
#include <vector>

using logger::Logger;

struct st {
  int a;
  char* arr;
};

int main() {
  // constexpr int NUM_THREADS = 5;
  // constexpr int NUM_ITERATIONS = 1000;
  // std::vector<std::thread> threads;

  // st a;
  // a.arr = new char[20];

  // for (int i = 0; i < NUM_THREADS; i++) {
  //   threads.emplace_back([]() {
  //     for (int i = 0; i < NUM_ITERATIONS; i++) {
  //       Logger::getInstance().log(logger::LogLevel::INFO, __FILE__, __LINE__,
  //                                 "msg");
  //     }

  //     std::cout << "Current thead: " << std::this_thread::get_id()
  //               << " buf.counter: " << Logger::getInstance().buf.counter
  //               << std::endl;
  //   });
  // }

  // for (auto& t : threads) {
  //   t.join();
  // }
  return 0;
}