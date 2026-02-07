set (SOURCES
    src/AppInfo.h
    src/Application.h
    src/ApplicationHarness.cpp
    src/Colour.cpp
    src/LineRenderer.cpp
    src/LineRenderer.h
    src/Maths.cpp
    src/ShaderProgram.cpp
    src/TextStream.cpp
    src/Utilities.cpp
    src/Vec2.cpp
    src/Vec2.h
    src/glad.c
)

# SDL Shit
set(SDL3_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SDL3/include")

if(UNIX AND NOT APPLE)
    set(SDL3_STATIC_LIB "${CMAKE_CURRENT_SOURCE_DIR}/SDL3/libSDL3.a")
endif()

if(WIN32)
    set(SDL3_STATIC_LIB "${CMAKE_CURRENT_SOURCE_DIR}/SDL3/lib_debug/SDL3.lib")
endif()

# Create an imported SDL3 target
add_library(SDL3 STATIC IMPORTED)

set_target_properties(SDL3 PROPERTIES
    IMPORTED_LOCATION "${SDL3_STATIC_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL3_INCLUDE_DIR}"
)

# Make ImGUI Lib
set(IMGUI_SOURCES 
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_impl_sdl3.cpp
    imgui/imgui_impl_opengl3.cpp
    imgui/imgui_demo.cpp
)

add_library(imgui STATIC)
target_sources(imgui PUBLIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC imgui)

add_library(Engine STATIC)
target_sources(Engine PUBLIC ${SOURCES})
target_include_directories(Engine PUBLIC src)

target_link_libraries(imgui PUBLIC SDL3)
target_link_libraries(Engine PUBLIC SDL3 imgui)

