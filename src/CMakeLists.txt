cmake_minimum_required ( VERSION 3.0.2 )

project ( Engine )


# Build and link executable.
set(SOURCE_CODE main.cpp)
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
set(GLHELPER glhelper.h glhelper.cpp)
#set(SHADERS simple.frag simple.vert)
add_executable ( IMPEngine ${SOURCE_CODE} ${IMGUI} ${GLAD} ${GLHELPER})

include_directories(linmath, shaders)

target_link_libraries(IMPEngine glfw)