#pragma once // Standard header guard.

#include "DataTypes.h" // Include our core data structure definitions.
#include <vector>
#include <array>
#include <unordered_map>
#include <iostream>

class L1CacheBook {
public:
    // Constructor: Initializes the order book.
    L1CacheBook();

    // Public methods to modify the order book state.
    // These are the primary entry points for the event loop.
    void addOrder(const PanoptesMessage& msg);
    void cancelOrder(const PanoptesMessage& msg);
    // Execute is not fully implemented in Phase 1, but the stub is here.
    void executeOrder(const PanoptesMessage& msg);

    // A simple method to print the top of the book for debugging.
    void printTopOfBook() const;

private:
    // Converts a fixed-point price to an array index.
    inline size_t priceToIndex(Price price) const {
        return static_cast<size_t>(price - PRICE_NORMALIZATION_BASE);
    }

    // --- Core Data Structures ---

    // Two large, fixed-size arrays to hold all possible price levels for bids and asks.
    // This is the core of the O(1) price level lookup. Accessing an element in an
    // array is a single memory operation.
    std::array<PriceLevel, MAX_PRICE_LEVELS> bids_;
    std::array<PriceLevel, MAX_PRICE_LEVELS> asks_;

    // A hash map to provide O(1) lookup of any order by its ID.
    // This is essential for fast cancel/modify operations. The map stores a
    // pointer to the order object, not the object itself.
    std::unordered_map<OrderID, Order*> order_map_;

    // A simple, contiguous memory pool for all Order objects.
    // We pre-allocate a large number of orders to avoid memory allocation
    // during runtime. This is a critical performance optimization.
    std::vector<Order> order_pool_;
    size_t next_order_pool_index_; // Tracks the next available slot in the pool.
};
