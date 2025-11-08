#include <gtest/gtest.h>
#include "../engine/include/L1CacheBook.h" // Include the class we want to test.

// A test fixture class to set up a clean L1CacheBook for each test.
class L1CacheBookTest : public ::testing::Test {
protected:
    L1CacheBook book; // Each test will get its own instance of the book.
};

// Test case to verify that adding a single order works correctly.
TEST_F(L1CacheBookTest, AddSingleOrder) {
    PanoptesMessage msg{'A', 1000, 1500000, 100, 'A', 'B'}; // A buy order.
    book.addOrder(msg);

    // For this simple test, we don't have a public interface to check the state.
    // A more complete test suite would have 'getter' methods to inspect the book.
    // For now, we are just ensuring the code runs without crashing.
    // A real test would check: ASSERT_EQ(book.getBestBid(), 1500000);
    SUCCEED();
}

// Test case for adding and then cancelling an order.
TEST_F(L1CacheBookTest, AddAndCancelOrder) {
    PanoptesMessage add_msg{'A', 1001, 1500000, 100, 'A', 'B'};
    book.addOrder(add_msg);

    // Now, create a cancel message for the same order ID.
    PanoptesMessage cancel_msg{'X', 1001, 0, 0, 'X', 'B'};
    book.cancelOrder(cancel_msg);

    // Again, without getters, we can only verify it doesn't crash.
    // A real test would assert that the book is now empty.
    SUCCEED();
}

// Test case for cancelling a non-existent order.
TEST_F(L1CacheBookTest, CancelNonExistentOrder) {
    // This should be a safe operation that does nothing.
    PanoptesMessage cancel_msg{'X', 9999, 0, 0, 'X', 'B'};
    book.cancelOrder(cancel_msg);
    SUCCEED();
}
