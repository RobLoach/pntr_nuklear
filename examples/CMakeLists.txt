# CMAKE Modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

# Dependencies
find_package(pntr REQUIRED)
find_package(pntr_app REQUIRED)

# Iterate through each .c file
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(APPEND example_sources ${sources})

# Do each example
foreach (example_source ${example_sources})
    # Create the basename for the example
    get_filename_component(example_name ${example_source} NAME)

    set(example_source_source ${example_source})
    string(REPLACE ".c" "" example_name ${example_name})

    # Targets built for this example, collected so the standard and warning
    # flags below are applied identically to every variant.
    set(example_targets)

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
        )
        target_compile_definitions(${example_name_raylib} PUBLIC PNTR_APP_RAYLIB)
        list(APPEND example_targets ${example_name_raylib})
    endif()

    # CLI
    if (PNTR_NUKLEAR_BUILD_EXAMPLES_CLI AND NOT EMSCRIPTEN)
        set(example_name_cli ${example_name}_cli)

        add_executable(${example_name_cli}
            ${example_source_source}
        )
        target_link_libraries(${example_name_cli} PUBLIC
            pntr
            pntr_app
            pntr_nuklear
            m
        )
        target_compile_definitions(${example_name_cli} PUBLIC PNTR_APP_CLI)
        list(APPEND example_targets ${example_name_cli})
    endif()

    # C99 Standard and warnings, matching the test target. Warnings are not
    # errors here: the vendored Nuklear demo code should never block a build.
    foreach (example_target ${example_targets})
        set_property(TARGET ${example_target} PROPERTY C_STANDARD 99)
        set_property(TARGET ${example_target} PROPERTY C_STANDARD_REQUIRED TRUE)

        if(MSVC)
            target_compile_options(${example_target} PRIVATE /W4)
        else()
            target_compile_options(${example_target} PRIVATE -Wall -Wextra)
        endif()
    endforeach ()

endforeach ()

# Resources
file(GLOB resources resources/*)
set(examples_resources)
list(APPEND examples_resources ${resources})
file(COPY ${examples_resources} DESTINATION "resources/")
