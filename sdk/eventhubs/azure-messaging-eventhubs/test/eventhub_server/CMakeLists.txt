# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.13)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(azure-messaging-eventhubs-test-server LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(AzureBuildTargetForCI)
#include(GoogleTest)

find_package(uamqp CONFIG REQUIRED)
find_package(umock_c)
find_package(azure_macro_utils_c CONFIG REQUIRED)
find_package(azure_c_shared_utility CONFIG REQUIRED)

# Unit Tests
add_executable(azure-messaging-eventhubs-test-server
  fake_eventhub_server.cpp
)
add_dependencies(azure-messaging-eventhubs-test-server  Azure::azure-core-amqp uamqp)

if (MSVC)
  # Disable warnings:
  # - C26495: Variable
  #             - 'testing::internal::Mutex::critical_section_'
  #             - 'testing::internal::Mutex::critical_section_init_phase_'
  #             - 'testing::internal::Mutex::owner_thread_id_'
  #             - 'testing::internal::Mutex::type_'
  #           is uninitialized. Always initialize member variables (type.6).
  # - C26812: The enum type
  #             - 'testing::internal::Mutex::StaticConstructorSelector'
  #             - 'testing::TestPartResult::Type'
  #           is unscoped. Prefer 'enum class' over 'enum' (Enum.3)
  # - C6326: Google comparisons
  target_compile_options(azure-messaging-eventhubs-test-server PUBLIC /wd26495 /wd26812 /wd6326 /wd28204 /wd28020 /wd6330 /wd4389)
endif()

target_include_directories(azure-messaging-eventhubs-test-server PRIVATE Azure::azure-core-amqp)

# Link test executable against gtest & gtest_main
target_link_libraries(azure-messaging-eventhubs-test-server PRIVATE Azure::azure-core-amqp uamqp umock_c azure_macro_utils_c)

create_per_service_target_build(core azure-messaging-eventhubs-test-server)
create_map_file(azure-messaging-eventhubs-test-server azure-messaging-eventhubs-test-server.map)

# gtest_discover_tests will scan the test from azure-core-amqp-tests and call add_test
# for each test to ctest. This enables `ctest -r` to run specific tests directly.
#gtest_discover_tests(azure-core-amqp-tests-uamqp
#     TEST_PREFIX azure-core-amqp.
#     NO_PRETTY_TYPES
#     NO_PRETTY_VALUES
#     DISCOVERY_TIMEOUT 600)
