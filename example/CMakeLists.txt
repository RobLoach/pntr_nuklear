# CMAKE Modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

# Resources
#file(GLOB resources resources/*)
#set(examples_resources)
#list(APPEND examples_resources ${resources})
#file(COPY ${examples_resources} DESTINATION "resources/")

find_package(pntr REQUIRED)
find_package(pntr_app REQUIRED)

# SDL
if (PNTR_NUKLEAR_BUILD_EXAMPLE_SDL)
    find_package(SDL2 QUIET)

    add_executable(pntr_nuklear_example_sdl
        pntr_nuklear_example.c
    )
    target_link_libraries(pntr_nuklear_example_sdl PRIVATE
        SDL2::SDL2
        SDL2::SDL2main
        pntr
        pntr_app
        pntr_nuklear
        m
    )
    target_compile_definitions(pntr_nuklear_example_sdl PUBLIC PNTR_APP_SDL)
endif()

# raylib
if (PNTR_NUKLEAR_BUILD_EXAMPLE_RAYLIB)
    find_package(raylib QUIET)

    add_executable(pntr_nuklear_example_raylib
        pntr_nuklear_example.c
    )
    target_link_libraries(pntr_nuklear_example_raylib PUBLIC
        raylib
        pntr
        pntr_app
        pntr_nuklear
        m
    )
    target_compile_definitions(pntr_nuklear_example_raylib PUBLIC PNTR_APP_RAYLIB)
endif()

# libretro
if (PNTR_NUKLEAR_BUILD_EXAMPLE_LIBRETRO)
    find_package(libretro QUIET)

    add_library(pntr_nuklear_example_libretro SHARED
        pntr_nuklear_example.c
    )
    target_link_libraries(pntr_nuklear_example_libretro PUBLIC
        pntr
        pntr_app
        pntr_nuklear
        libretro
        m
    )
    set_target_properties(pntr_nuklear_example_libretro PROPERTIES PREFIX "")
    target_compile_definitions(pntr_nuklear_example_libretro PUBLIC PNTR_APP_LIBRETRO)
endif()
