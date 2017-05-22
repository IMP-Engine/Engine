cmake_minimum_required ( VERSION 3.0.2 )

project ( Engine )

# Fetch all source and header files
file(GLOB_RECURSE source_list RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp" "*.hpp" "*.h" "*.c" "*.vert" "*.frag" "*.glsl" "*.cl")

# Remove specific files
#list(FILTER source_list EXCLUDE REGEX "binary_to_compressed_c.cpp") 

add_executable ( IMPEngine ${source_list})

set_property(TARGET IMPEngine PROPERTY CXX_STANDARD 11)
set_property(TARGET IMPEngine PROPERTY CXX_STANDARD_REQUIRED ON)

# Make the IMPEngine project the start project in the solution
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT IMPEngine)

#assign_source_group(${SOURCE_CODE})
assign_source_group(${source_list})

target_link_libraries(IMPEngine glfw)
target_link_libraries(IMPEngine tbb)
target_link_libraries(IMPEngine ${OPENCL_LIBRARIES})

# Set output directory to bin
if( MSVC )
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin/${BITNESS_SUFFIX})
else()
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin/${BITNESS_SUFFIX}/${CMAKE_BUILD_TYPE})
endif()
