#include <arm_neon.h>
#include <grpcpp/create_channel.h>
#include <logger.h>

#include <memory>

#include "logs.grpc.pb.h"
#include "logs.pb.h"

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
#include <log.h>
#include <time.h>

#include <string>
#include <thread>
#include <vector>

// void* operator new(std::size_t n) {
//   static std::size_t counter = 0;
//   counter++;
//   std::cout << "new alloc(" << counter << ")\n";
//   return malloc(n);
// }

int main() {
  LOG(LEVEL, "hello");
  LOG(LEVEL, "hello");
  LOG(LEVEL, "hello");

  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);

  std::cout << ts.tv_sec << std::endl;

  std::cout << static_cast<uint32_t>(ts.tv_sec) << std::endl;

  // std::thread worker([]() { LOG(LEVEL, "hello"); });
  // worker.join();
  // logger::LogEntry ent;
  // ent.set_timestamp("today");
  // ent.set_message("Long long long long long");
  // ent.set_line("long long longlonglonglonglong line");
  // ent.set_line(
  //     "long long "
  //     "longlonglonglonglonglonglonglonglonglonglonglonglonglonglonglonglonglong"
  //     "longlonglonglonglonglonglonglonglonglonglonglonglonglonglonglonglonglong"
  //     "longlonglonglonglonglonglonglonglonglonglonglonglonglonglonglonglonglong"
  //     "long file");
  // std::cout << sizeof(ent) << std::endl;
  // std::cout << sizeof(logger::LogEntry) << std::endl;

  // constexpr int NUM_THREADS = 5;
  // constexpr int NUM_ITERATIONS = 1000;
  // std::vector<std::thread> threads;

  // st a;
  // a.arr = new char[20];

  // for (int i = 0; i < NUM_THREADS; i++) {
  //   threads.emplace_back([]() {
  //     for (int i = 0; i < NUM_ITERATIONS; i++) {
  //       Logger::getInstance().log(logger::LogLevel::INFO, __FILE__,
  //       __LINE__,
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