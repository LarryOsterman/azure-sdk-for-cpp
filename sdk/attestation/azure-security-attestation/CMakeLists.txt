# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.13)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake-modules")

include(AzureVcpkg)

az_vcpkg_integrate()
project(azure-security-attestation LANGUAGES CXX)

include(AzureTransportAdapters)
include(AzureBuildTargetForCI)
include(AzureVersion)
include(AzureCodeCoverage)
include(AzureDoxygen)
include(AzureGlobalCompileOptions)
include(AzureConfigRTTI)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(NOT AZ_ALL_LIBRARIES)
  find_package(azure-core-cpp CONFIG QUIET)
  if(NOT azure-core-cpp_FOUND)
    find_package(azure-core-cpp REQUIRED)
  endif()
endif()
find_package(OpenSSL REQUIRED)

set(
  AZURE_ATTESTATION_HEADER
    inc/azure/attestation.hpp
    inc/azure/attestation/attestation_administration_client.hpp
    inc/azure/attestation/attestation_client.hpp
    inc/azure/attestation/attestation_client_models.hpp
    inc/azure/attestation/attestation_client_options.hpp
    inc/azure/attestation/dll_import_export.hpp
    inc/azure/attestation/rtti.hpp
)

set(
  AZURE_ATTESTATION_SOURCE
    src/attestation_administration_client.cpp
    src/attestation_client.cpp
    src/attestation_client_options.cpp
    src/private/attestation_client_models_private.hpp
    src/private/attestation_client_private.cpp
    src/private/attestation_client_private.hpp
    src/private/attestation_common_request.cpp
    src/private/attestation_common_request.hpp
    src/private/attestation_deserializers_private.cpp
    src/private/attestation_deserializers_private.hpp
    src/private/crypto/inc/crypto.hpp
    src/private/crypto/openssl/openssl_helpers.hpp
    src/private/crypto/openssl/opensslcert.cpp
    src/private/crypto/openssl/opensslcert.hpp
    src/private/crypto/openssl/opensslcrypto.cpp
    src/private/crypto/openssl/opensslkeys.cpp
    src/private/crypto/openssl/opensslkeys.hpp
    src/private/jsonhelpers.cpp
    src/private/jsonhelpers_private.hpp
    src/private/package_version.hpp
)

add_library(azure-security-attestation ${AZURE_ATTESTATION_HEADER} ${AZURE_ATTESTATION_SOURCE})

create_per_service_target_build(attestation azure-security-attestation)

target_include_directories(
  azure-security-attestation
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
      $<INSTALL_INTERFACE:include>
)

target_link_libraries(azure-security-attestation PUBLIC Azure::azure-core OpenSSL::Crypto OpenSSL::SSL)
target_compile_definitions(azure-security-attestation PRIVATE _azure_BUILDING_SDK)

# make sure that users can consume the project as a library.
add_library(Azure::azure-security-attestation ALIAS azure-security-attestation)

# coverage. Has no effect if BUILD_CODE_COVERAGE is OFF
create_code_coverage(attestation azure-security-attestation azure-security-attestation-test "tests?/*;samples?/*")


get_az_version("${CMAKE_CURRENT_SOURCE_DIR}/src/private/package_version.hpp")
set_target_properties(azure-security-attestation PROPERTIES VERSION ${AZ_LIBRARY_VERSION})
generate_documentation(azure-security-attestation ${AZ_LIBRARY_VERSION})

az_vcpkg_export(
    azure-security-attestation
    ATTESTATION
    "azure/attestation/dll_import_export.hpp"
  )

az_rtti_setup(
  azure-security-attestation
  ATTESTATION
  "azure/attestation/rtti.hpp"
)

if(BUILD_TESTING)
    # tests
    add_subdirectory(test/ut)
endif()

if(BUILD_SAMPLES)
   add_subdirectory(samples)
endif()

