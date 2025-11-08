#include <benchmark/benchmark.h>
#include "../engine/include/L1CacheBook.h"
#include <memory>

// A benchmark fixture sets up a consistent environment for our tests.
class L1CacheBookFixture : public benchmark::Fixture {
public:
    // This code runs before each benchmark timing loop.
    // By creating a new book here, we ensure the memory pool is fresh.
    void SetUp(const benchmark::State& state) override {
        book = std::make_unique<L1CacheBook>();
        order_id_counter = 0;
    }

    // This code runs after the loop.
    void TearDown(const benchmark::State& state) override {
        // Clean up if necessary
    }

    // We use a unique_ptr to manage the book's memory automatically.
    std::unique_ptr<L1CacheBook> book;
    uint64_t order_id_counter;
};

// Use BENCHMARK_F to register a benchmark that uses our fixture.
BENCHMARK_F(L1CacheBookFixture, BM_AddOrder)(benchmark::State& state) {
    PanoptesMessage msg{0, 0, 1500000, 100, 'A', 'B'};

    // This is the main timing loop.
    for (auto _ : state) {
        // Give each order a unique ID.
        msg.order_id = ++order_id_counter;
        // Use the 'book' object created in our fixture's SetUp method.
        book->addOrder(msg);
    }
}

// Another benchmark using the same fixture.
BENCHMARK_F(L1CacheBookFixture, BM_AddCancelCycle)(benchmark::State& state) {
    PanoptesMessage add_msg{0, 0, 1500000, 100, 'A', 'B'};
    PanoptesMessage cancel_msg{0, 0, 0, 0, 'X', 'B'};

    for (auto _ : state) {
        add_msg.order_id = ++order_id_counter;
        cancel_msg.order_id = order_id_counter;

        book->addOrder(add_msg);
        book->cancelOrder(cancel_msg);
    }
}

// The main entry point for the benchmark executable.
BENCHMARK_MAIN();
