find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_library(event_bus INTERFACE)
target_include_directories(event_bus INTERFACE src)
target_compile_features(event_bus INTERFACE cxx_std_26)

add_library(affinity INTERFACE)
target_include_directories(affinity INTERFACE src/core)
target_compile_features(affinity INTERFACE cxx_std_26)
target_link_libraries(affinity INTERFACE Threads::Threads)

## [P1]: Event bus performance tests
#add_executable(test_event_bus tests/test_performance.cpp)
#target_link_libraries(test_event_bus event_bus GTest::gtest GTest::gtest_main)
#target_compile_features(test_event_bus PRIVATE cxx_std_26)

# [P2]: Affinity binding tests
#add_executable(test_affinity tests/test_affinity.cpp)
#target_link_libraries(test_affinity affinity Threads::Threads GTest::gtest GTest::gtest_main)
#target_compile_features(test_affinity PRIVATE cxx_std_26)

# [P2]: Publisher+Subscriber integration tests
add_executable(test_publisher_subscriber tests/test_publisher_subscriber.cpp)
target_link_libraries(test_publisher_subscriber event_bus affinity GTest::gtest GTest::gtest_main Threads::Threads)
target_compile_features(test_publisher_subscriber PRIVATE cxx_std_26)

# [P3]: EventDirector integration tests
add_executable(test_event_director tests/test_event_director.cpp)
target_link_libraries(test_event_director event_bus affinity GTest::gtest GTest::gtest_main Threads::Threads)
target_compile_features(test_event_director PRIVATE cxx_std_26)

# [P3]: GuardedMemoryPool integration tests
add_executable(test_guarded_pool tests/test_guarded_pool.cpp)
target_include_directories(test_guarded_pool PRIVATE src)
target_link_libraries(test_guarded_pool GTest::gtest GTest::gtest_main Threads::Threads)
target_compile_features(test_guarded_pool PRIVATE cxx_std_26)

# [P3]: Concurrent EventDirector + Subscriber test
add_executable(test_concurrent_ingestion tests/test_concurrent_ingestion.cpp)
target_include_directories(test_concurrent_ingestion PRIVATE src)
target_link_libraries(test_concurrent_ingestion event_bus affinity GTest::gtest GTest::gtest_main Threads::Threads)
target_compile_features(test_concurrent_ingestion PRIVATE cxx_std_26)

# [P3]: Ingestion latency and throughput benchmark
add_executable(test_ingestion_benchmark tests/test_ingestion_benchmark.cpp)
target_include_directories(test_ingestion_benchmark PRIVATE src tests)
target_link_libraries(test_ingestion_benchmark event_bus affinity GTest::gtest GTest::gtest_main Threads::Threads)
target_compile_features(test_ingestion_benchmark PRIVATE cxx_std_26)

enable_testing()
#add_test(NAME event_bus_tests COMMAND test_event_bus)
#add_test(NAME affinity_tests COMMAND test_affinity)
add_test(NAME publisher_subscriber_tests COMMAND test_publisher_subscriber)
add_test(NAME event_director_tests COMMAND test_event_director)
add_test(NAME guarded_pool_tests COMMAND test_guarded_pool)
add_test(NAME concurrent_ingestion_tests COMMAND test_concurrent_ingestion)
add_test(NAME ingestion_benchmark_tests COMMAND test_ingestion_benchmark)