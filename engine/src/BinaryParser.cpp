#include "DataTypes.h" // We need the PanoptesMessage definition.
#include <cstring>     // For std::memcpy
#include "BinaryParser.h"

// This is the core parser function.
// It takes a pointer to a raw character buffer from the network
// and returns a PanoptesMessage struct.
PanoptesMessage parseMessage(const char* buffer) {
    // Here, we use reinterpret_cast to tell the compiler:
    // "Treat this block of memory at 'buffer' as if it were a PanoptesMessage struct."
    // This is extremely fast because it involves no copying or processing of individual fields.
    // It simply re-interprets the raw bytes.
    //
    // WARNING: It only works because:
    // 1. We have guaranteed the memory layout with #pragma pack(push, 1).
    // 2. The sender (Thrasher) and receiver (Engine) are compiled on the same
    //    architecture (ie. "endianness").
    const PanoptesMessage* msg = reinterpret_cast<const PanoptesMessage*>(buffer);
    return *msg;
}
