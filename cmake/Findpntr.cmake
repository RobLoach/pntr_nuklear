# pntr
include(FetchContent)
FetchContent_Declare(
    pntr
    GIT_REPOSITORY https://github.com/robloach/pntr.git
    GIT_TAG de6abd7b48d37126edcd491cb4e0453c8ba146d7
)
FetchContent_MakeAvailable(pntr)
#include_directories(${pntr_SOURCE_DIR}/src)
