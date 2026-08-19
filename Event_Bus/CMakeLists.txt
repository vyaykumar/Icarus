# Event Bus: Phase 1

add_library(event_bus INTERFACE)

target_include_directories(event_bus INTERFACE src)
target_compile_features(event_bus INTERFACE cxx_std_26)