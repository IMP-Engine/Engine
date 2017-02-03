cmake_minimum_required ( VERSION 3.0.2 )
add_library(
        imgui STATIC
        imconfig.h
        imgui.cpp
        imgui.h
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_internal.h
        stb_rect_pack.h
        stb_textedit.h
        stb_truetype.h
)