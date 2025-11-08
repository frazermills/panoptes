#include <iostream>
#include <fstream>
#include <vector>
#include <sys/stat.h> // For mkdir

// We need the definition of PanoptesMessage from the main project.
#include "engine/include/DataTypes.h"

// This utility creates a small binary file for testing.
int main() {
    // Create the data directory. On Linux, this will do nothing if it already exists.
    mkdir("data", 0755);

    std::ofstream output_file("data/messages.bin", std::ios::binary);
    if (!output_file.is_open()) {
        std::cerr << "Failed to open data/messages.bin for writing." << std::endl;
        return 1;
    }

    std::vector<PanoptesMessage> messages;
    // Example start time: 9:30:00.000000000 AM in nanoseconds since midnight.
    int64_t start_time = 34200000000000;

    // Generate 1,000 sample 'Add Order' messages.
    for (int i = 0; i < 1000; ++i) {
        messages.push_back({
            start_time + (i * 10000), // timestamp (advancing by 10 microseconds)
            (uint64_t)(1000 + i),     // order_id (unique)
            (int64_t)(1500000 + i),   // price (fixed-point, e.g., 150.0000 + 0.0001i)
            100,                     // size
            'A',                     // event_type ('A' for Add)
            'B',                     // side ('B' for Bid)
            {0}                      // padding
        });
    }

    // Write the raw bytes of the vector directly to the file.
    output_file.write(reinterpret_cast<const char*>(messages.data()), messages.size() * sizeof(PanoptesMessage));
    output_file.close();

    std::cout << "Successfully created dummy data file: data/messages.bin" << std::endl;

    return 0;
}
