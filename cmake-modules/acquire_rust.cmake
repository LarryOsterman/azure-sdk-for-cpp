# CMake function to download the "rustup-init" script and run it to install Rust

# This function will download the "rustup-init" script from the Rust website and run it to install Rust.
function(install_rustup)
if(WIN32)
    set(_USER_HOME $ENV{USERPROFILE})
    set(_CARGO_NAME "cargo.exe")
else()
    set(_USER_HOME $ENV{HOME})
    set(_CARGO_NAME "cargo")
endif()

if (EXISTS "${_USER_HOME}/.cargo/bin/${_CARGO_NAME}")
set(RUST_EXISTS TRUE)
else()
set(RUST_EXISTS FALSE)
endif()

if (NOT ${RUST_EXISTS})
	message(STATUS "Rust is not installed. Downloading and installing Rust using rustup...")
    set(RUSTUP_URL "https://sh.rustup.rs")
    set(RUSTUP_INIT_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/rustup-init.sh")

    if(WIN32)
    set(RUSTUP_URL "https://static.rust-lang.org/rustup/dist/i686-pc-windows-gnu/rustup-init.exe")
    # On Windows, the "rustup-init" script is a .exe file
        set(RUSTUP_INIT_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/rustup-init.exe")
    endif()

    message(STATUS "Downloading and installing Rust using rustup to ${RUSTUP_INIT_SCRIPT}")
    message(STATUS "Download URL: ${RUSTUP_URL}")
    # Download the "rustup-init" script
    file(DOWNLOAD "${RUSTUP_URL}" "${RUSTUP_INIT_SCRIPT}"
         EXPECTED_HASH SHA256=92535fbde0c7ce45dce7d58f853d89ab1f873d29f78e6d80382f76ca2d1984cf
         SHOW_PROGRESS)

    # Make the script executable
    file(COPY "${RUSTUP_INIT_SCRIPT}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

    if(WIN32)
        # Run the rustup-init script on Windows
        message(STATUS "Running rustup-init.exe")
        execute_process(COMMAND "${CMAKE_CURRENT_BINARY_DIR}/rustup-init.exe" -y RESULTS_VARIABLE _RESULTS OUTPUT_VARIABLE _OUTPUT ERROR_VARIABLE _ERROR)
        message(STATUS "Running rustup-init.exe completed: ${_RESULTS}")
        if (NOT ${_RESULTS} EQUAL 0)
			message(FATAL "rustup-init failed: ${_OUTPUT}")
			message(FATAL "Error: ${_ERROR}")
        endif()
        message(STATUS "Output: ${_OUTPUT}")
        message(STATUS "Error: ${_ERROR}")
    else()
        # Run the rustup-init script on Linux
        message(STATUS "Running rustup-init.sh")
        execute_process(COMMAND chmod +x "${CMAKE_CURRENT_BINARY_DIR}/rustup-init.sh")
        execute_process(COMMAND "${CMAKE_CURRENT_BINARY_DIR}/rustup-init.sh" -y)

        set(ENV{PATH} "$ENV{_USER_HOME}/.cargo/bin:$ENV{PATH}")
    endif()
else()
	message(STATUS "Rust is already installed, skipping.")
endif()
endfunction()
