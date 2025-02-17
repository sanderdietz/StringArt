cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(stringArt)

set(SOURCES
    src/main.cpp
    src/InputImage.cpp
    src/OutputImage.cpp
    src/StringArt.cpp
    src/Image.cpp
    imgui/src/imgui.cpp 
    imgui/src/imgui_demo.cpp 
    imgui/src/imgui_draw.cpp 
    imgui/src/imgui_tables.cpp 
    imgui/src/imgui_widgets.cpp
    imgui/src/imgui_impl_glfw.cpp 
    imgui/src/imgui_impl_opengl3.cpp
)

add_executable(StringArt ${SOURCES})

target_link_libraries(StringArt GL glfw)

target_include_directories(StringArt PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/imgui/include
)