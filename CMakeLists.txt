cmake_minimum_required(VERSION 3.10)
project(swimmerProject)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(glfw3 REQUIRED)

find_library(COREAUDIO_LIBRARY CoreAudio)
find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)

include_directories(${CMAKE_SOURCE_DIR}/dependencies)
# NEW: Tell CMake where to find the ImGui headers
include_directories(${CMAKE_SOURCE_DIR}/dependencies/imgui) 

# NEW: Compile the game WITH the ImGui source files
add_executable(swimmerProject 
    main.cpp
    dependencies/imgui/imgui.cpp
    dependencies/imgui/imgui_draw.cpp
    dependencies/imgui/imgui_widgets.cpp
    dependencies/imgui/imgui_tables.cpp
    dependencies/imgui/imgui_impl_glfw.cpp
    dependencies/imgui/imgui_impl_opengl3.cpp
)

target_link_libraries(swimmerProject 
    OpenGL::GL 
    glfw
    ${OpenCV_LIBS}
    ${COREAUDIO_LIBRARY}
    ${AUDIOTOOLBOX_LIBRARY}
)