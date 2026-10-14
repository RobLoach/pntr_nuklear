# raylib
include(FetchContent)

# raylib reads these when its own CMakeLists.txt is processed, so they have to be
# in the cache before FetchContent_MakeAvailable() adds it.
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
set(FETCHCONTENT_QUIET NO)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
    GIT_SHALLOW 1
)
FetchContent_MakeAvailable(raylib)
