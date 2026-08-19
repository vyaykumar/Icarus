# Event Bus: Phase 1

find_package(GTest REQUIRED)

add_library(event_bus INTERFACE)

target_include_directories(event_bus INTERFACE src)
target_compile_features(event_bus INTERFACE cxx_std_26)

add_executable(test_event_bus tests/test_queues.cpp)
target_link_libraries(test_event_bus event_bus GTest::gtest GTest::gtest_main)
target_compile_features(test_event_bus PRIVATE cxx_std_26)

enable_testing()
add_test(NAME event_bus_tests COMMAND test_event_bus)