#include <gtest/gtest.h>
#include <queue.h>

#include <iostream>
#include <thread>

#define BUFFER_SIZE (32768)

// void *consumer_loop(void *arg) {
//   queue_t *q = (queue_t *)arg;
//   size_t count = 0;
//   size_t i;
//   for (i = 0; i < MESSAGES_PER_THREAD; i++) {
//     uint8_t x[72] =
//     queue_get(q, (uint8_t *)&x, sizeof(size_t));
//     count++;
//   }
//   return (void *)count;
// }

// void *publisher_loop(void *arg) {
//   queue_t *q = (queue_t *)arg;
//   size_t i;
//   for (i = 0; i < NUM_THREADS * MESSAGES_PER_THREAD; i++) {
//     queue_put(q, (uint8_t *)&i, sizeof(size_t));
//   }
//   return (void *)i;
// }

TEST(QueueTest, SingleProducerSingleConsumer) {
  queue_t q;
  queue_init(&q, BUFFER_SIZE);

  constexpr size_t kCount = 5'000'000;
  constexpr size_t kMsgLen = 72;

  std::atomic<bool> producerDone = false;

  std::thread producer([&]() {
    for (int i = 0; i < kCount; i++) {
      char x[kMsgLen];
      std::snprintf(x, sizeof(x), "Item %d", i);
      queue_put(&q, (uint8_t *)x, sizeof(x));
    }

    producerDone = true;
  });

  std::thread consumer([&]() {
    size_t recieved = 0;
    uint8_t x[kMsgLen];
    size_t toGet = sizeof(x);
    while (recieved < kCount) {
      size_t got = queue_get(&q, x, toGet);
      toGet -= got;
      if (toGet == 0) {
        toGet = sizeof(x);
        uint8_t tmp[kMsgLen];
        std::snprintf((char *)tmp, kMsgLen, "Item %d", (int)recieved);
        ASSERT_STREQ((char *)x, (char *)tmp);
        recieved += 1;
      }
    }
    EXPECT_EQ(recieved, kCount);
  });

  consumer.join();
  producer.join();

  queue_destroy(&q);
}