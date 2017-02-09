cmake_minimum_required ( VERSION 3.0.2 )

project ( Engine )


# Build and link executable.
#set(SOURCE_CODE main.cpp)

# Fetch all source and header files
file(GLOB_RECURSE source_list RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp" "*.hpp" "*.h" "*.c")

# Remove specific files
list(FILTER source_list EXCLUDE REGEX "imgui*") 
list(FILTER source_list EXCLUDE REGEX "imconfig*") 
list(FILTER source_list EXCLUDE REGEX "glad*") 
list(FILTER source_list EXCLUDE REGEX "khrplatform*") 
list(FILTER source_list EXCLUDE REGEX "stb_*") 
list(FILTER source_list EXCLUDE REGEX "binary_to_compressed_c.cpp") 

set(IMGUI     
	imconfig.h
        imgui.cpp
        imgui.h
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_internal.h
        stb_rect_pack.h
        stb_textedit.h
        stb_truetype.h)
set(GLAD glad.h glad.c khrplatform.h)

add_executable ( IMPEngine ${source_list} ${IMGUI} ${GLAD})

# Make the IMPEngine project the start project in the solution
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT IMPEngine)

#assign_source_group(${SOURCE_CODE})
assign_source_group(${source_list})

target_link_libraries(IMPEngine glfw)