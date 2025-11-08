#pragma once // This is a standard header guard. It prevents the file from being included more than once.

#include <cstdint> // Include this header for fixed-width integer types like uint64_t.
#include <cstddef>

// --- Core Type Definitions ---
// Using type aliases makes the code more readable. We know exactly what an OrderID is.
using OrderID = uint64_t;
using Price = int64_t;
using Timestamp = int64_t;

// --- Exchange Message Format ---
// This is the custom binary format we defined. It's what travels over the network.
// It is 32 bytes, which is half of a typical 64-byte cache line, making it memory-friendly.
// We use '#pragma pack(push, 1)' to ensure the compiler doesn't add extra padding,
// so we can control the exact memory layout. This is critical for network serialization.
#pragma pack(push, 1)
struct PanoptesMessage {
    Timestamp timestamp;      // 8 bytes (nanoseconds since midnight)
    OrderID order_id;         // 8 bytes
    Price price;              // 8 bytes (fixed-point: price * 10000)
    int32_t size;             // 4 bytes
    char event_type;          // 1 byte ('A'dd, 'X'cancel, 'E'xecute)
    char side;                // 1 byte ('B'id, 'A'sk)
    char padding[2];          // 2 bytes to make the struct 32 bytes
};
#pragma pack(pop)


// --- Internal Order Book Data Structures ---

// Represents a single order in the order book's linked list.
struct Order {
    OrderID id;
    Price price;
    int32_t size;
    char side;

    // Pointers for the doubly-linked list at a specific price level.
    // This allows for O(1) insertion and removal.
    Order* next = nullptr;
    Order* prev = nullptr;
};

// Represents a single price level in the book. It contains the head and tail
// of a doubly-linked list of all orders at this price.
struct PriceLevel {
    Order* head = nullptr;
    Order* tail = nullptr;
    int32_t total_volume = 0; // The sum of sizes of all orders at this level.
};


// --- Configuration Constants ---

// The maximum number of price levels the order book can hold.
// This must be large enough to accommodate all possible price ticks for the instrument.
constexpr size_t MAX_PRICE_LEVELS = 1000000;

// The base price used for normalizing prices to array indices.
// This should be set to a value lower than any expected price for the instrument.
// For a stock trading at ~$150, a base of $100 (1000000 in fixed-point) is safe.
constexpr Price PRICE_NORMALIZATION_BASE = 1000000;
