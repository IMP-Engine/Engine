cmake_minimum_required ( VERSION 3.0.2 )

project ( IMPEngine )

set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin )

set ( DEBUG )

# GLFW
add_subdirectory(external/glfw-3.2.1)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Main code
add_subdirectory ( src )
