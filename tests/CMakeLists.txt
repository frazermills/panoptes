# Include the FetchContent module to manage dependencies.
include(FetchContent)

# --- Dependency Management ---

# Declare the Google Test dependency.
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Declare the Google Benchmark dependency.
FetchContent_Declare(
  benchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.8.3.zip
)

# Make the dependencies available. This will download and configure them.
FetchContent_MakeAvailable(googletest benchmark)


# --- Unit Test Target ---
add_executable(run_unit_tests
    test_L1CacheBook.cpp
    ../engine/src/L1CacheBook.cpp
)
target_include_directories(run_unit_tests PRIVATE ../engine/include)

# Link against the gtest_main target provided by FetchContent.
target_link_libraries(run_unit_tests PRIVATE gtest_main)

# Add the unit test to CTest.
include(GoogleTest)
gtest_discover_tests(run_unit_tests)


# --- Benchmark Target ---
add_executable(run_benchmarks
    bench_L1CacheBook.cpp
    ../engine/src/L1CacheBook.cpp
)
target_include_directories(run_benchmarks PRIVATE ../engine/include)

# Link against the benchmark and benchmark_main targets provided by FetchContent.
target_link_libraries(run_benchmarks PRIVATE benchmark benchmark_main)
