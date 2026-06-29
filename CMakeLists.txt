cmake_minimum_required(VERSION 3.16)

file(READ "${CMAKE_SOURCE_DIR}/VERSION" NEON_VERSION)
string(STRIP "${NEON_VERSION}" NEON_VERSION)

project(Neon
    VERSION ${NEON_VERSION}
    LANGUAGES C
)

configure_file(
    "${CMAKE_SOURCE_DIR}/version.h.in"
    "${CMAKE_BINARY_DIR}/generated/include/version.h"
    @ONLY
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(RELEASE_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/dist")

file(GLOB_RECURSE SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.c"
)

add_executable(neon ${SOURCES})
add_executable(debug ${SOURCES})
add_executable(profiler ${SOURCES})
add_executable(release ${SOURCES})

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")


function(setup_neon_target target)
    target_include_directories(${target} PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${CMAKE_BINARY_DIR}/generated/include"
    )
endfunction()

setup_neon_target(release)
setup_neon_target(neon)
setup_neon_target(debug)
setup_neon_target(profiler)

if(MSVC)
    target_compile_options(release PRIVATE /O2 /GL)
    target_compile_options(neon PRIVATE /O2 /GL)
    target_compile_options(debug PRIVATE /O2 /GL)

    target_link_options(release PRIVATE /LTCG)
    target_link_options(neon PRIVATE /LTCG)
    target_link_options(debug PRIVATE /LTCG)

    message(WARNING "gprof profiling is not supported on MSVC")
else()
    target_compile_options(release PRIVATE
        -O3
        -march=x86-64
        -mtune=generic
        -flto
        -falign-functions=32
        -falign-loops=32
        -fno-tree-vectorize
        -g0
    )

    target_compile_options(neon PRIVATE
        -O3
        -march=native
        -flto
        -falign-functions=32
        -falign-loops=32
        -fno-tree-vectorize
    )

    target_compile_options(debug PRIVATE
        -O3
        -march=native
        -flto
    )

    target_compile_options(profiler PRIVATE
        -pg
        -g
        -O0
    )

    target_link_options(release PRIVATE
        -flto
        -s
    )

    target_link_options(neon PRIVATE
        -flto
    )

    target_link_options(debug PRIVATE
        -flto
    )

    target_link_options(profiler PRIVATE
        -pg
    )
endif()

target_compile_definitions(debug PRIVATE
    NEON_DEBUG
)