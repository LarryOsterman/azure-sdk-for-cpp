# CMakeList.txt : CMake project for RustHttp, include source and define
# project specific logic here.
#

project (RustHttpTransport LANGUAGES CXX)

include (${CMAKE_SOURCE_DIR}/cmake-modules/acquire_rust.cmake)

install_rustup()

include(FetchContent)

FetchContent_Declare(
    Corrosion
    GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
    GIT_TAG v0.5 # Optionally specify a commit hash, version tag or branch here
)
FetchContent_MakeAvailable(Corrosion)

message(STATUS "Project name: ${PROJECT_NAME}")

# Import targets defined in a package or workspace manifest `Cargo.toml` file
corrosion_import_crate(MANIFEST_PATH ${REQWEST_TRANSPORT_ROOT}/ReqwestRustClient/Cargo.toml)

# Add source to this project's executable.
set(RUST_TRANSPORT_SOURCES 
  RustHttpTransport.cpp
  )

set(RUST_HTTP_DIR ${REQWEST_TRANSPORT_ROOT}/ReqwestRustClient)

add_library(RustHttpTransport ${RUST_TRANSPORT_SOURCES})

target_include_directories(RustHttpTransport PRIVATE ${CMAKE_SOURCE_DIR}/include ${RUST_HTTP_DIR})
target_include_directories(RustHttpTransport INTERFACE ${RUST_HTTP_DIR})
target_include_directories(RustHttpTransport INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries( RustHttpTransport PUBLIC rust_http Azure::azure-core )


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RustHttpTransport PROPERTY CXX_STANDARD 20)
endif()

