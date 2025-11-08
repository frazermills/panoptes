#include "L1CacheBook.h" // Include the header file that defines the L1CacheBook class.

// Constructor implementation.
L1CacheBook::L1CacheBook() : next_order_pool_index_(0) {
    // Pre-allocate memory for a large number of orders to avoid allocation at runtime.
    // A real system would have a more sophisticated memory management strategy.
    order_pool_.resize(10000000); // Reserve space for 10 million orders.
}

// Implementation of the addOrder method.
void L1CacheBook::addOrder(const PanoptesMessage& msg) {
    // 1. Get a new order object from our pre-allocated memory pool.
    // This avoids a slow call to 'new'.
    Order* new_order = &order_pool_[next_order_pool_index_++];
    new_order->id = msg.order_id;
    new_order->price = msg.price;
    new_order->size = msg.size;
    new_order->side = msg.side;
    new_order->next = nullptr;
    new_order->prev = nullptr;

    // 2. Add the new order to the order map for fast O(1) lookups later.
    order_map_[new_order->id] = new_order;

    // 3. Find the correct price level in the correct array (bids or asks).
    size_t index = priceToIndex(new_order->price);
    PriceLevel* level;
    if (new_order->side == 'B') {same
        level = &bids_[index];
    } else {
        level = &asks_[index];
    }

    // 4. Add the order to the doubly-linked list at that price level.
    // We add new orders to the back of the list (tail). This represents FIFO (First-In, First-Out) priority.
    if (level->head == nullptr) {
        // If the list is empty, this order is both the head and the tail.
        level->head = new_order;
        level->tail = new_order;
    } else {
        // If the list is not empty, add the new order after the current tail.
        level->tail->next = new_order;
        new_order->prev = level->tail;
        level->tail = new_order;
    }
    level->total_volume += new_order->size;
}

// Implementation of the cancelOrder method.
void L1CacheBook::cancelOrder(const PanoptesMessage& msg) {
    // 1. Find the order to cancel using the order map. This is an O(1) operation.
    auto it = order_map_.find(msg.order_id);
    if (it == order_map_.end()) {
        // The order ID was not found. This can happen in real-world scenarios.
        // We can log this event, but for now, we just ignore it.
        return;
    }
    Order* order_to_cancel = it->second;

    // 2. Remove the order from the order map.
    order_map_.erase(it);

    // 3. Find the price level where the order resides.
    size_t index = priceToIndex(order_to_cancel->price);
    PriceLevel* level;
    if (order_to_cancel->side == 'B') {
        level = &bids_[index];
    } else {
        level = &asks_[index];
    }

    // 4. Unlink the order from the doubly-linked list.
    // This is where the prev/next pointers are crucial for an O(1) removal.
    if (order_to_cancel->prev != nullptr) {
        order_to_cancel->prev->next = order_to_cancel->next;
    }
    if (order_to_cancel->next != nullptr) {
        order_to_cancel->next->prev = order_to_cancel->prev;
    }

    // 5. Update the head and tail pointers of the price level if necessary.
    if (level->head == order_to_cancel) {
        level->head = order_to_cancel->next;
    }
    if (level->tail == order_to_cancel) {
        level->tail = order_to_cancel->prev;
    }
    level->total_volume -= order_to_cancel->size;

    // Note: In this simple implementation, the order object remains in the memory pool
    // but is now "orphaned" (not pointed to by the map or any list). A more advanced
    // memory pool would mark this slot as reusable.
}

void L1CacheBook::executeOrder(const PanoptesMessage& msg) {
    // An execution message means a trade occurred. This typically reduces the size
    // of an existing order at the top of the book.
    // For Phase 1, we will treat it like a cancel to simplify the logic.
    // A full implementation would find the order and decrease its size.
    cancelOrder(msg);
}

// A simple helper function to see the state of the book.
void L1CacheBook::printTopOfBook() const {
    Price best_ask_price = -1;
    int32_t best_ask_volume = 0;
    for(size_t i = 0; i < MAX_PRICE_LEVELS; ++i) {
        if(asks_[i].head != nullptr) {
            best_ask_price = asks_[i].head->price;
            best_ask_volume = asks_[i].total_volume;
            break;
        }
    }

    Price best_bid_price = -1;
    int32_t best_bid_volume = 0;
    for(int i = MAX_PRICE_LEVELS - 1; i >= 0; --i) {
        if(bids_[i].head != nullptr) {
            best_bid_price = bids_[i].head->price;
            best_bid_volume = bids_[i].total_volume;
            break;
        }
    }
    std::cout << "BBO: " << best_bid_volume << " @ " << (double)best_bid_price/10000.0
              << " -- " << best_ask_volume << " @ " << (double)best_ask_price/10000.0 << std::endl;
}
