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
	imgui_impl_glfw_gl3.h
	imgui_impl_glfw_gl3.cpp
        stb_rect_pack.h
        stb_textedit.h
        stb_truetype.h)
set(GLAD glad.h glad.c khrplatform.h)
add_executable ( IMPEngine ${SOURCE_CODE} ${IMGUI} ${GLAD})

target_link_libraries(IMPEngine glfw)