cmake_minimum_required(VERSION 3.20)
project(TrollCrypter VERSION 3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.1
)
FetchContent_MakeAvailable(imgui)

add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui_lib PUBLIC glfw)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

file(GLOB_RECURSE SOURCES
    *.cpp
    *.h
)
list(FILTER SOURCES EXCLUDE REGEX "build/|vcpkg/")

if(WIN32)
    list(APPEND SOURCES app.rc)
    set_source_files_properties(app.rc PROPERTIES
        COMPILE_FLAGS "/I\"${CMAKE_SOURCE_DIR}\""
    )
    add_executable(TrollCrypter WIN32 ${SOURCES})
    target_link_libraries(TrollCrypter PRIVATE opengl32 Comctl32 dwmapi)
    target_compile_definitions(TrollCrypter PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN)
else()
    add_executable(TrollCrypter ${SOURCES})
    find_package(OpenGL REQUIRED)
    target_link_libraries(TrollCrypter PRIVATE OpenGL::GL)
endif()

target_include_directories(TrollCrypter PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(TrollCrypter PRIVATE
    imgui_lib
    glfw
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
)

if(MSVC)
    target_compile_options(TrollCrypter PRIVATE /W3 /utf-8)
    set_target_properties(TrollCrypter PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
else()
    target_compile_options(TrollCrypter PRIVATE -Wall -Wextra)
endif()

set_target_properties(TrollCrypter PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
