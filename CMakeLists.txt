cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

set(FRAMEWORK_NAME "xalwart")
set(LIBRARY_NAME "${FRAMEWORK_NAME}")
set(LIBRARY_VERSION "0.0.0")

project(${LIBRARY_NAME})

# Search for OpenSSL
find_package(OpenSSL 1.1 REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

set(
    DEFAULT_INCLUDE_PATHS
        "/usr/local"
        "/usr"
)

foreach(ENTRY ${DEFAULT_INCLUDE_PATHS})
    include_directories(${ENTRY}/include)
    link_directories(${ENTRY}/lib)
endforeach()

# Search for xalwart.base
find_library(
    XALWART_BASE xalwart.base REQUIRED
    PATHS ${DEFAULT_INCLUDE_PATHS}
)

# Search for xalwart.crypto
find_library(
    XALWART_CRYPTO xalwart.crypto REQUIRED
    PATHS ${DEFAULT_INCLUDE_PATHS}
)

# Search for xalwart.orm
find_library(
    XALWART_ORM xalwart.orm REQUIRED
    PATHS ${DEFAULT_INCLUDE_PATHS}
)

option(XW_CONFIGURE_LIB "Configure the library." ON)
if (${XW_CONFIGURE_LIB})
    add_subdirectory(src)
endif()

option(XW_CONFIGURE_TESTS "Configure tests." OFF)
if (${XW_CONFIGURE_TESTS})
    enable_testing()
    add_subdirectory(tests)
endif()
