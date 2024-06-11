# Nuklear
include(FetchContent)
FetchContent_Declare(
    Nuklear
    GIT_REPOSITORY https://github.com/Immediate-Mode-UI/Nuklear.git
    GIT_TAG 47f570c
)
FetchContent_GetProperties(Nuklear)
if (NOT Nuklear_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(Nuklear)

    add_library(Nuklear INTERFACE)
    target_include_directories(Nuklear INTERFACE ${Nuklear_SOURCE_DIR})
endif()
