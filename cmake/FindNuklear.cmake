# Nuklear
include(FetchContent)
FetchContent_Declare(
    Nuklear
    GIT_REPOSITORY https://github.com/Immediate-Mode-UI/Nuklear.git
    GIT_TAG d052dd68de46c88e7534e841d4088d0e967ce01e
    GIT_SHALLOW 1
)
FetchContent_GetProperties(Nuklear)
if (NOT Nuklear_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(Nuklear)

    add_library(Nuklear INTERFACE)
    target_include_directories(Nuklear INTERFACE ${Nuklear_SOURCE_DIR})
endif()
