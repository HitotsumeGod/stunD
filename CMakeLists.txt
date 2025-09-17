cmake_minimum_required(VERSION 3.15...4.00)
project(stunD LANGUAGES C)
set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/main)
set(PHDRS ${CMAKE_CURRENT_SOURCE_DIR}/src/include)
set(DEPS ${CMAKE_CURRENT_SOURCE_DIR}/vendors)
add_subdirectory(${DEPS}/ptools ${DEPS}/ptools/build)
add_executable(stunD
                ${SRC}/pilot.c
                ${SRC}/polling.c
                ${SRC}/queries.c
                ${SRC}/sio.c)
target_include_directories(stunD PRIVATE ${PHDRS})
target_link_libraries(stunD ptools)