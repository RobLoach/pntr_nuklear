cmake_minimum_required(VERSION 3.11)
project(pntr-nuklear
    DESCRIPTION "pntr-nuklear: Nuklear for pntr"
    HOMEPAGE_URL "https://github.com/robloach/pntr-nuklear"
    VERSION 0.0.1
    LANGUAGES C
)

# pntr-nuklear
add_library(pntr-nuklear INTERFACE)
target_include_directories(pntr-nuklear INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Options
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    set(PNTR_NUKLEAR_IS_MAIN TRUE)
else()
    set(PNTR_NUKLEAR_IS_MAIN FALSE)
endif()
option(PNTR_NUKLEAR_BUILD_TESTS "Examples" ${PNTR_NUKLEAR_IS_MAIN})

# Testing
if (PNTR_NUKLEAR_BUILD_TESTS)
    include(CTest)
    enable_testing()
    if (BUILD_TESTING)
        set(CTEST_CUSTOM_TESTS_IGNORE
            pkg-config--static
        )
        # Always print verbose output when tests fail if run using `make test`.
        list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
        add_subdirectory(test)
    endif()
endif()
