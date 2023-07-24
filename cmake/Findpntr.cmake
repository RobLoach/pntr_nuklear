# pntr
include(FetchContent)
FetchContent_Declare(
    pntr
    GIT_REPOSITORY https://github.com/robloach/pntr.git
    GIT_TAG 266fc1ee5825c9ed25961795924a9dabbf109654
)
FetchContent_MakeAvailable(pntr)
#include_directories(${pntr_SOURCE_DIR}/src)
