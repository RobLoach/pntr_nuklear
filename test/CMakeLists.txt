
# Add the cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

find_package(pntr)

# Resources
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)

# pntr-nuklear-test
add_executable(pntr-nuklear-test pntr-nuklear-test.c)
target_link_libraries(pntr-nuklear-test PUBLIC
    pntr
    pntr-nuklear
)

# Set up the test
add_test(NAME pntr-nuklear-test COMMAND pntr-nuklear-test)
