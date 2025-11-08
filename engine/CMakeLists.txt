# Define the executable target name and list its source files.
add_executable(panoptes_engine
    src/main.cpp
    src/L1CacheBook.cpp
    src/BinaryParser.cpp
)

# Tell the compiler to look for header files in the "include" directory.
# This is how src/main.cpp can find L1CacheBook.h.
target_include_directories(panoptes_engine
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
