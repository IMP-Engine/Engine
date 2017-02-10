cmake_minimum_required ( VERSION 3.0.2 )

project ( Engine )

# Fetch all source and header files
file(GLOB_RECURSE source_list RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp" "*.hpp" "*.h" "*.c")

# Remove specific files
#list(FILTER source_list EXCLUDE REGEX "binary_to_compressed_c.cpp") 

add_executable ( IMPEngine ${source_list})

# Make the IMPEngine project the start project in the solution
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT IMPEngine)

#assign_source_group(${SOURCE_CODE})
assign_source_group(${source_list})

target_link_libraries(IMPEngine glfw)
