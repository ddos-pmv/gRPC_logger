#include <gtest/gtest.h>
#include <ring_buffer.h>

using namespace logger;

struct Dummy {
  size_t number;
  char msg[64];

  bool operator==(const Dummy& other) const {
    return number == other.number && std::strcmp(msg, other.msg) == 0;
  }
};

TEST(RingBufferTest, WriteReadBasic) {
  RingBuffer<Dummy, 8192> buffer;
  Dummy data{1, "data1"};

  ASSERT_TRUE(buffer.write(data));

  Dummy out;
  ASSERT_TRUE(buffer.read(out));

  // std::cout << "DATA: " << data.number << " " << data.msg << '\n';
  // std::cout << "OUT:  " << out.number << " " << out.msg << '\n';

  ASSERT_EQ(data, out);

  ASSERT_FALSE(buffer.read(out));
}
TEST(RingBufferTest, SingleProducerSingleConsumer) {
  constexpr int kCount = 20000;
  RingBuffer<Dummy, 8192> buffer;

  std::vector<Dummy> source(kCount);
  for (int i = 0; i < kCount; ++i) {
    source[i].number = i;
    std::snprintf(source[i].msg, sizeof(source[i].msg), "Item %d", i);
  }

  std::vector<Dummy> result;
  result.reserve(kCount);

  std::atomic<bool> producer_done = false;

  std::thread producer([&]() {
    for (const auto& item : source) {
      while (!buffer.write(item)) {
        std::cout << "waiting to write " << item.number << "\n";
        // std::this_thread::yield();  // не нагружаем CPU, ждём
      }
    }
    producer_done = true;
  });

  std::thread consumer([&]() {
    Dummy entry;
    while (result.size() < kCount) {
      if (buffer.read(entry)) {
        int i = entry.number;
        EXPECT_EQ(i, source[i].number) << "at index " << i;
        EXPECT_STREQ(entry.msg, source[i].msg) << "at index " << i;
        result.push_back(entry);

      } else if (producer_done.load()) {
        // Нет больше записей и producer завершён
        break;
      } else {
        std::cout << "waiting to read ";
        if (!result.empty()) std::cout << result.back().number;
        std::cout << '\n';
      }
    }
  });

  producer.join();
  consumer.join();

  ASSERT_EQ(result.size(), source.size());
  for (size_t i = 0; i < source.size(); ++i) {
    EXPECT_EQ(result[i], source[i]) << "Mismatch at index " << i;
  }
}

int main() {
  ::testing::InitGoogleTest();

  return RUN_ALL_TESTS();
}