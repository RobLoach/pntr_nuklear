# CMAKE Modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

# Dependencies
find_package(pntr REQUIRED)
find_package(pntr_app REQUIRED)
find_package(Nuklear REQUIRED)

# Iterate through each .c file
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(APPEND example_sources ${sources})

# Do each example
foreach (example_source ${example_sources})
    # Create the basename for the example
    get_filename_component(example_name ${example_source} NAME)

    set(example_source_source ${example_source})
    string(REPLACE ".c" "" example_name ${example_name})

    # SDL
    if (PNTR_NUKLEAR_BUILD_EXAMPLES_SDL)
        find_package(SDL2 QUIET)

        set(example_name_sdl ${example_name}_sdl)

        add_executable(${example_name_sdl}
            ${example_source_source}
        )
        target_link_libraries(${example_name_sdl} PRIVATE
            SDL2::SDL2
            SDL2::SDL2main
            pntr
            pntr_app
            pntr_nuklear
            m
            Nuklear
        )
        target_compile_definitions(${example_name_sdl} PUBLIC PNTR_APP_SDL)
    endif()

    # raylib
    if (PNTR_NUKLEAR_BUILD_EXAMPLES_RAYLIB)
        find_package(raylib QUIET)
        set(example_name_raylib ${example_name}_raylib)

        add_executable(${example_name_raylib}
            ${example_source_source}
        )
        target_link_libraries(${example_name_raylib} PUBLIC
            raylib
            pntr
            pntr_app
            pntr_nuklear
            m
            Nuklear
        )
        target_compile_definitions(${example_name_raylib} PUBLIC PNTR_APP_RAYLIB)
    endif()

    # libretro
    if (PNTR_NUKLEAR_BUILD_EXAMPLES_LIBRETRO)
        find_package(libretro QUIET)
        set(example_name_libretro ${example_name}_libretro)

        add_library(${example_name_libretro} SHARED
            ${example_source_source}
        )
        target_link_libraries(${example_name_libretro} PUBLIC
            pntr
            pntr_app
            pntr_nuklear
            libretro
            m
            Nuklear
        )
        set_target_properties(${example_name_libretro} PROPERTIES PREFIX "")
        target_compile_definitions(${example_name_libretro} PUBLIC PNTR_APP_LIBRETRO)
    endif()
endforeach ()

# Resources
#file(GLOB resources resources/*)
#set(examples_resources)
#list(APPEND examples_resources ${resources})
#file(COPY ${examples_resources} DESTINATION "resources/")
