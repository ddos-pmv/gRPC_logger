#include <gtest/gtest.h>
#include <queue_internal.h>
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
  RingBuffer<Dummy, 32768> buffer;
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
  constexpr int kCount = 5'000'000;
  ;
  RingBuffer<Dummy, 20000> buffer;

  std::vector<Dummy> source(kCount);
  for (int i = 0; i < kCount; ++i) {
    source[i].number = i;
    std::snprintf(source[i].msg, sizeof(source[i].msg), "Item %d", i);
  }

  std::vector<Dummy> result;
  result.reserve(kCount);

  std::atomic<bool> producer_done = false;

  std::thread producer([&]() {
    size_t waits_to_write = 0;
    for (const auto& item : source) {
      while (!buffer.write(item)) {
        waits_to_write += 1;
        std::this_thread::yield();  // не нагружаем CPU, ждём
      }
    }
    producer_done = true;
    std::cout << "poducer waited " << waits_to_write << " times\n";
  });

  std::thread consumer([&]() {
    size_t waits_to_read = 0;
    Dummy entry;
    while (result.size() < kCount) {
      if (buffer.read(entry)) {
        int i = entry.number;
        EXPECT_EQ(i, source[i].number) << "at index " << i;
        EXPECT_STREQ(entry.msg, source[i].msg) << "at index " << i;
        result.emplace_back(entry);

      } else if (producer_done.load()) {
        if (buffer.read(entry)) {
          result.emplace_back(entry);
        } else {
          break;
        }

      } else {
        waits_to_read += 1;
        std::this_thread::yield();
      }
    }

    std::cout << "Counsumer waited " << waits_to_read << " times\n";
  });

  producer.join();
  consumer.join();

  ASSERT_EQ(result.size(), source.size());
  for (size_t i = 0; i < source.size(); ++i) {
    EXPECT_EQ(result[i], source[i]) << "Mismatch at index " << i;
  }
}

static constexpr size_t BATCH_SZ = 128;

TEST(RingBufferTest, SingleProducerSingleConsumerBatch) {
  constexpr int kCount = 5'000'000;
  RingBuffer<Dummy, 20000> buffer;
  std::vector<Dummy> source(kCount);

  // fill source data
  for (int i = 0; i < kCount; ++i) {
    source[i].number = i;
    std::snprintf(source[i].msg, sizeof(source[i].msg), "Item %d", i);
  }

  std::vector<Dummy> result;
  result.reserve(kCount);

  std::atomic<bool> producer_done{false};

  std::thread producer([&]() {
    size_t waits_to_write = 0;
    for (const auto& item : source) {
      while (!buffer.write(item)) {
        waits_to_write += 1;
        std::this_thread::yield();  // не нагружаем CPU, ждём
      }
    }
    producer_done = true;
    std::cout << "poducer waited " << waits_to_write << " times\n";
  });

  std::thread consumer([&]() {
    Dummy batch[BATCH_SZ];
    size_t batch_size;
    size_t waits_to_read = 0;
    while (result.size() < kCount) {
      batch_size = BATCH_SZ;
      if (buffer.read(batch, batch_size)) {
        for (size_t i = 0; i < batch_size; ++i) {
          result.push_back(batch[i]);
        }

      } else if (producer_done.load(std::memory_order_acquire)) {
        batch_size = BATCH_SZ;
        if (buffer.read(batch, batch_size)) {
          for (size_t i = 0; i < batch_size; ++i) {
            result.push_back(batch[i]);
          }
        } else {
          break;
        }

      } else {
        waits_to_read += 1;
        std::this_thread::yield();
      }
    }
    std::cout << "counsumer waited " << waits_to_read << " times\n";
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