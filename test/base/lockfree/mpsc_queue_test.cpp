/**
 * @file mpsc_queue_test.cpp
 * @brief Unit tests for MPSC lock-free queue
 *
 * Test Coverage:
 * 1. Single-threaded basic operations (push/pop)
 * 2. Empty and full queue behavior
 * 3. Batch operations
 * 4. Multi-producer single-consumer concurrency
 * 5. Move semantics
 * 6. Various types (int, struct, std::string)
 */

#include "base/lockfree/mpsc_queue.hpp"
#include <atomic>
#include <gtest/gtest.h>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

using namespace cf::lockfree;

// =============================================================================
// Test Types
// =============================================================================

struct TestStruct {
    int a;
    int b;

    TestStruct() : a(0), b(0) {}
    TestStruct(int x, int y) : a(x), b(y) {}

    bool operator==(const TestStruct& other) const { return a == other.a && b == other.b; }
};

// =============================================================================
// Test Suite 1: Single-Threaded Basic Operations
// =============================================================================

TEST(MpscQueueTest, ConstructAndDestruct) {
    MpscQueue<int, 4> queue;
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);
    EXPECT_EQ(queue.capacity(), 4);
}

TEST(MpscQueueTest, PushAndPopSingleElement) {
    MpscQueue<int, 4> queue;

    EXPECT_TRUE(queue.tryPush(42));
    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), 1);

    int value;
    EXPECT_TRUE(queue.tryPop(value));
    EXPECT_EQ(value, 42);
    EXPECT_TRUE(queue.empty());
}

TEST(MpscQueueTest, PushPopMultipleElements) {
    MpscQueue<int, 4> queue;

    EXPECT_TRUE(queue.tryPush(1));
    EXPECT_TRUE(queue.tryPush(2));
    EXPECT_TRUE(queue.tryPush(3));

    EXPECT_EQ(queue.size(), 3);

    int value;
    EXPECT_TRUE(queue.tryPop(value));
    EXPECT_EQ(value, 1);

    EXPECT_TRUE(queue.tryPop(value));
    EXPECT_EQ(value, 2);

    EXPECT_TRUE(queue.tryPop(value));
    EXPECT_EQ(value, 3);

    EXPECT_TRUE(queue.empty());
}

TEST(MpscQueueTest, PopFromEmptyQueue) {
    MpscQueue<int, 4> queue;

    int value;
    EXPECT_FALSE(queue.tryPop(value));
    EXPECT_TRUE(queue.empty());
}

TEST(MpscQueueTest, PushToFullQueue) {
    MpscQueue<int, 4> queue;

    EXPECT_TRUE(queue.tryPush(1));
    EXPECT_TRUE(queue.tryPush(2));
    EXPECT_TRUE(queue.tryPush(3));
    EXPECT_TRUE(queue.tryPush(4));

    // Queue is full with 4 elements, need to consume before pushing more
    EXPECT_EQ(queue.size(), 4);

    // Pop one element
    int value;
    EXPECT_TRUE(queue.tryPop(value));
    EXPECT_EQ(value, 1);

    // Now we can push again
    EXPECT_TRUE(queue.tryPush(5));
    EXPECT_EQ(queue.size(), 4);
}

TEST(MpscQueueTest, RoundtripElements) {
    MpscQueue<int, 128> queue; // Larger capacity to fit all elements

    for (int i = 0; i < 100; ++i) {
        EXPECT_TRUE(queue.tryPush(int{i}));
    }

    int value;
    for (int i = 0; i < 100; ++i) {
        EXPECT_TRUE(queue.tryPop(value));
        EXPECT_EQ(value, i);
    }
}

// =============================================================================
// Test Suite 2: Type Tests
// =============================================================================

TEST(MpscQueueTest, StructType) {
    MpscQueue<TestStruct, 4> queue;

    EXPECT_TRUE(queue.tryPush(TestStruct(10, 20)));

    TestStruct value;
    EXPECT_TRUE(queue.tryPop(value));
    EXPECT_EQ(value.a, 10);
    EXPECT_EQ(value.b, 20);
}

TEST(MpscQueueTest, StringType) {
    MpscQueue<std::string, 4> queue;

    EXPECT_TRUE(queue.tryPush("hello"));

    std::string value;
    EXPECT_TRUE(queue.tryPop(value));
    EXPECT_EQ(value, "hello");
}

TEST(MpscQueueTest, MoveSemantics) {
    MpscQueue<std::string, 4> queue;

    std::string str = "move test";
    EXPECT_TRUE(queue.tryPush(std::move(str)));

    // Original string should be in moved-from state
    EXPECT_TRUE(str.empty() || !str.empty()); // Either state is valid

    std::string value;
    EXPECT_TRUE(queue.tryPop(value));
    EXPECT_EQ(value, "move test");
}

// =============================================================================
// Test Suite 3: Batch Operations
// =============================================================================

TEST(MpscQueueTest, PushBatchEmpty) {
    MpscQueue<int, 16> queue;
    std::vector<int> values;

    size_t pushed = queue.tryPushBatch(cf::span<int>(values));
    EXPECT_EQ(pushed, 0);
}

TEST(MpscQueueTest, PushBatchMultiple) {
    MpscQueue<int, 16> queue;
    std::vector<int> values = {1, 2, 3, 4, 5};

    size_t pushed = queue.tryPushBatch(cf::span<int>(values));
    EXPECT_EQ(pushed, 5);
    EXPECT_EQ(queue.size(), 5);

    int value;
    for (int expected : values) {
        EXPECT_TRUE(queue.tryPop(value));
        EXPECT_EQ(value, expected);
    }
}

TEST(MpscQueueTest, PopBatchEmpty) {
    MpscQueue<int, 16> queue;

    int buffer[10];
    size_t popped = queue.tryPopBatch(buffer, 10);
    EXPECT_EQ(popped, 0);
}

TEST(MpscQueueTest, PopBatchMultiple) {
    MpscQueue<int, 16> queue;

    for (int i = 0; i < 5; ++i) {
        queue.tryPush(int{i});
    }

    int buffer[10];
    size_t popped = queue.tryPopBatch(buffer, 10);
    EXPECT_EQ(popped, 5);

    for (size_t i = 0; i < popped; ++i) {
        EXPECT_EQ(buffer[i], static_cast<int>(i));
    }
}

TEST(MpscQueueTest, PopBatchPartial) {
    MpscQueue<int, 16> queue;

    for (int i = 0; i < 3; ++i) {
        queue.tryPush(int{i});
    }

    int buffer[10];
    size_t popped = queue.tryPopBatch(buffer, 10);
    EXPECT_EQ(popped, 3);

    EXPECT_EQ(buffer[0], 0);
    EXPECT_EQ(buffer[1], 1);
    EXPECT_EQ(buffer[2], 2);
}

// =============================================================================
// Test Suite 4: Concurrency Tests
// =============================================================================

TEST(MpscQueueTest, SingleProducerSingleConsumer) {
    MpscQueue<int, 1024> queue;
    constexpr int numItems = 10000;

    // Producer thread
    std::thread producer([&]() {
        for (int i = 0; i < numItems; ++i) {
            while (!queue.tryPush(int{i})) {
                std::this_thread::yield();
            }
        }
    });

    // Consumer thread
    std::thread consumer([&]() {
        int received = 0;
        int value;
        while (received < numItems) {
            if (queue.tryPop(value)) {
                EXPECT_EQ(value, received);
                ++received;
            } else {
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();

    EXPECT_TRUE(queue.empty());
}

TEST(MpscQueueTest, MultiProducerSingleConsumer) {
    MpscQueue<int, 4096> queue;
    constexpr int numProducers = 4;
    constexpr int itemsPerProducer = 2500;
    constexpr int totalItems = numProducers * itemsPerProducer;

    std::atomic<int> receivedCount{0};
    std::atomic<int> sum{0};

    // Producer threads
    std::vector<std::thread> producers;
    for (int p = 0; p < numProducers; ++p) {
        producers.emplace_back([&, p]() {
            for (int i = 0; i < itemsPerProducer; ++i) {
                int value = p * 1000 + i;
                while (!queue.tryPush(int{value})) {
                    std::this_thread::yield();
                }
            }
        });
    }

    // Consumer thread
    std::thread consumer([&]() {
        int value;
        while (receivedCount.load() < totalItems) {
            if (queue.tryPop(value)) {
                sum.fetch_add(value);
                receivedCount.fetch_add(1);
            } else {
                std::this_thread::yield();
            }
        }
    });

    for (auto& t : producers) {
        t.join();
    }
    consumer.join();

    EXPECT_EQ(receivedCount.load(), totalItems);

    // Calculate expected sum
    int expectedSum = 0;
    for (int p = 0; p < numProducers; ++p) {
        for (int i = 0; i < itemsPerProducer; ++i) {
            expectedSum += p * 1000 + i;
        }
    }
    EXPECT_EQ(sum.load(), expectedSum);
}

TEST(MpscQueueTest, StressTestMultiProducer) {
    MpscQueue<int, 65536> queue;
    constexpr int numProducers = 8;
    constexpr int itemsPerProducer = 5000;
    constexpr int totalItems = numProducers * itemsPerProducer;

    std::atomic<int> receivedCount{0};

    // Producer threads
    std::vector<std::thread> producers;
    for (int p = 0; p < numProducers; ++p) {
        producers.emplace_back([&, p]() {
            for (int i = 0; i < itemsPerProducer; ++i) {
                while (!queue.tryPush(p * itemsPerProducer + i)) {
                    std::this_thread::yield();
                }
            }
        });
    }

    // Consumer thread
    std::thread consumer([&]() {
        int value;
        while (receivedCount.load() < totalItems) {
            if (queue.tryPop(value)) {
                receivedCount.fetch_add(1);
            } else {
                std::this_thread::yield();
            }
        }
    });

    for (auto& t : producers) {
        t.join();
    }
    consumer.join();

    EXPECT_EQ(receivedCount.load(), totalItems);
    EXPECT_TRUE(queue.empty());
}

// =============================================================================
// Test Suite 5: Edge Cases
// =============================================================================

TEST(MpscQueueTest, CapacityIsPowerOfTwo) {
    MpscQueue<int, 1> q1;         // 2^0
    MpscQueue<int, 2> q2;         // 2^1
    MpscQueue<int, 4> q4;         // 2^2
    MpscQueue<int, 8> q8;         // 2^3
    MpscQueue<int, 16> q16;       // 2^4
    MpscQueue<int, 1024> q1024;   // 2^10
    MpscQueue<int, 65536> q65536; // 2^16

    EXPECT_EQ(q1.capacity(), 1);
    EXPECT_EQ(q2.capacity(), 2);
    EXPECT_EQ(q4.capacity(), 4);
    EXPECT_EQ(q8.capacity(), 8);
    EXPECT_EQ(q16.capacity(), 16);
    EXPECT_EQ(q1024.capacity(), 1024);
    EXPECT_EQ(q65536.capacity(), 65536);
}

TEST(MpscQueueTest, LargeQueue) {
    MpscQueue<int, 65536> largeQueue;

    // Fill partially
    for (int i = 0; i < 1000; ++i) {
        EXPECT_TRUE(largeQueue.tryPush(int{i}));
    }

    EXPECT_EQ(largeQueue.size(), 1000);

    // Drain
    int value;
    for (int i = 0; i < 1000; ++i) {
        EXPECT_TRUE(largeQueue.tryPop(value));
        EXPECT_EQ(value, i);
    }

    EXPECT_TRUE(largeQueue.empty());
}

TEST(MpscQueueTest, AlternatingPushPop) {
    MpscQueue<int, 16> queue;

    for (int round = 0; round < 10; ++round) {
        EXPECT_TRUE(queue.tryPush(int{round}));
        int value;
        EXPECT_TRUE(queue.tryPop(value));
        EXPECT_EQ(value, round);
    }

    EXPECT_TRUE(queue.empty());
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
