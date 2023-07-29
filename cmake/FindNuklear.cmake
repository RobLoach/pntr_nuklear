# Nuklear
include(FetchContent)
FetchContent_Declare(
    Nuklear
    GIT_REPOSITORY https://github.com/Immediate-Mode-UI/Nuklear.git
    GIT_TAG b4b94b0486c0c43045d0176d03cce016190fe3ff
    GIT_SHALLOW 1
)
FetchContent_GetProperties(Nuklear)
if (NOT Nuklear_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(Nuklear)

    add_library(Nuklear INTERFACE)
    target_include_directories(Nuklear INTERFACE ${Nuklear_SOURCE_DIR})
endif()
