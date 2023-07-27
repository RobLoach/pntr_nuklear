# pntr
include(FetchContent)
FetchContent_Declare(
    pntr
    GIT_REPOSITORY https://github.com/robloach/pntr.git
    GIT_TAG 5698322dcfd513b06372f3b3e754cfb582df6966
)
FetchContent_MakeAvailable(pntr)
#include_directories(${pntr_SOURCE_DIR}/src)
