cmake_minimum_required ( VERSION 3.0.2 )

project ( IMPEngine )


# Function to assign the path of a file as it source group
# This will produce filter ("folders") in a VS project.
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin )

set ( DEBUG )

# GLFW
add_subdirectory(external/glfw-3.2.1)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# GLM
include_directories(external/glm)

# dirent
if (WIN32)
	include_directories(external/dirent)
endif(WIN32)

#TBB
add_subdirectory(external/tbb)

# Main code
add_subdirectory ( src )

