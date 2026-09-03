# Event Bus: Phase 2

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Core libraries
add_library(event_bus INTERFACE)
target_include_directories(event_bus INTERFACE src)
target_compile_features(event_bus INTERFACE cxx_std_26)

add_library(affinity INTERFACE)
target_include_directories(affinity INTERFACE src/core)
target_compile_features(affinity INTERFACE cxx_std_26)
target_link_libraries(affinity INTERFACE Threads::Threads)

# [P1]: Event bus performance tests
add_executable(test_event_bus tests/test_performance.cpp)
target_link_libraries(test_event_bus event_bus GTest::gtest GTest::gtest_main)
target_compile_features(test_event_bus PRIVATE cxx_std_26)

# [P2]: Affinity binding tests
add_executable(test_affinity tests/test_affinity.cpp)
target_link_libraries(test_affinity affinity Threads::Threads GTest::gtest GTest::gtest_main)
target_compile_features(test_affinity PRIVATE cxx_std_26)

## [P2]: Publisher integration tests
#add_executable(test_publisher tests/test_publisher.cpp)
#target_link_libraries(test_publisher event_bus affinity GTest::gtest GTest::gtest_main Threads::Threads)
#target_compile_features(test_publisher PRIVATE cxx_std_26)
#
## [P2]: Subscriber integration tests
#add_executable(test_subscriber tests/test_subscriber.cpp)
#target_link_libraries(test_subscriber event_bus affinity GTest::gtest GTest::gtest_main Threads::Threads)
#target_compile_features(test_subscriber PRIVATE cxx_std_26)

# [P2]: Publisher+Subscriber integration tests
add_executable(test_publisher_subscriber tests/test_publisher_subscriber.cpp)
target_link_libraries(test_publisher_subscriber event_bus affinity GTest::gtest GTest::gtest_main Threads::Threads)
target_compile_features(test_publisher_subscriber PRIVATE cxx_std_26)

# Test registration
enable_testing()
#add_test(NAME event_bus_tests COMMAND test_event_bus)
#add_test(NAME affinity_tests COMMAND test_affinity)
#add_test(NAME publisher_tests COMMAND test_publisher)
#add_test(NAME subscriber_tests COMMAND test_subscriber)
add_test(NAME publisher_subscriber_tests COMMAND test_subscriber)