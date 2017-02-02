cmake_minimum_required ( VERSION 3.0.2 )

project ( Engine )

# External libraries.
include_directories ( ${CMAKE_SOURCE_DIR}/external/imgui)
add_subdirectory ( ${CMAKE_SOURCE_DIR}/external/imgui ${CMAKE_CURRENT_BINARY_DIR}/imgui )
include_directories ( ${CMAKE_SOURCE_DIR}/external/glad)
add_subdirectory ( ${CMAKE_SOURCE_DIR}/external/glad ${CMAKE_CURRENT_BINARY_DIR}/glad )


# Build and link executable.
set(SOURCE_CODE main.cpp)
add_executable ( IMPEngine ${SOURCE_CODE})

target_link_libraries(IMPEngine glfw imgui glad)