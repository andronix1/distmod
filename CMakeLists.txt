cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(distrand LANGUAGES C)

option(COMPILE_AS_STATIC "Compile as static" ON)

file(GLOB_RECURSE LIB_SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.c)
add_library(distrand-lib-objects OBJECT ${LIB_SRC_FILES})
target_compile_options(distrand-lib-objects
    PRIVATE
        -Wall -fPIC -O3 
)
include_directories(include)

if (COMPILE_AS_STATIC)
    add_library(distrand-lib
        STATIC
            $<TARGET_OBJECTS:distrand-lib-objects>
    )
else()
    add_library(distrand-lib
        SHARED
            $<TARGET_OBJECTS:distrand-lib-objects>
    )
endif()

if (DEMO)
    file(GLOB_RECURSE DEMO_SRC_FILES ${PROJECT_SOURCE_DIR}/demo/*.c)
    add_executable(distrand-demo ${DEMO_SRC_FILES})
    target_link_libraries(distrand-demo m)
    target_link_libraries(distrand-demo distrand-lib)
endif()
