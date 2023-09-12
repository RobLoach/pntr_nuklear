cmake_minimum_required(VERSION 3.11)
project(pntr_nuklear
    DESCRIPTION "pntr_nuklear: Nuklear for pntr"
    HOMEPAGE_URL "https://github.com/robloach/pntr-nuklear"
    VERSION 0.1.0
    LANGUAGES C
)

# pntr_nuklear
add_library(pntr_nuklear INTERFACE)
target_include_directories(pntr_nuklear INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Options
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    set(PNTR_NUKLEAR_IS_MAIN TRUE)
else()
    set(PNTR_NUKLEAR_IS_MAIN FALSE)
endif()

option(PNTR_NUKLEAR_BUILD_EXAMPLES "pntr_nuklear: Examples" ${PNTR_NUKLEAR_IS_MAIN})
option(PNTR_NUKLEAR_BUILD_EXAMPLES_RAYLIB "pntr_nuklear: raylib Examples" ${PNTR_NUKLEAR_IS_MAIN})
option(PNTR_NUKLEAR_BUILD_EXAMPLES_CLI "pntr_nuklear: CLI Examples" ${PNTR_NUKLEAR_IS_MAIN})
option(PNTR_NUKLEAR_BUILD_TESTS "pntr_nuklear: Test" ${PNTR_NUKLEAR_IS_MAIN})

if (PNTR_NUKLEAR_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

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
