cmake_minimum_required(VERSION 3.0)

project(col VERSION 0.0)

add_subdirectory(lib/lua)
include_directories(lib/lua)

if (MSVC)
    add_executable(col src/windows_main.c src/col.c src/libcol.c)
    target_link_libraries(col lua opengl32 winmm)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_executable(col src/macos_main.m src/col.c)
    target_compile_options(col PRIVATE "-fobjc-arc")

    find_library(cocoa Cocoa)
    find_library(opengl OpenGL)
    find_library(corevideo CoreVideo)
    target_link_libraries(col lua ${cocoa} ${opengl} ${corevideo})
endif()
