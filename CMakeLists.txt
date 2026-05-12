cmake_minimum_required(VERSION 4.2)
project(yuv420_bmp_overlay)

set(CMAKE_CXX_STANDARD 11)

add_executable(yuv420_bmp_overlay src/main.cpp
        src/config.h
        src/config.cpp
        src/bmp_reader.h
        src/bmp_reader.cpp
        )

if (WIN32)
    add_custom_command(TARGET yuv420_bmp_overlay POST_BUILD
            # Runtime MinGW
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/Program Files/JetBrains/CLion 2026.1.1/bin/mingw/bin/libgcc_s_seh-1.dll"
            "$<TARGET_FILE_DIR:yuv420_bmp_overlay>"

            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/Program Files/JetBrains/CLion 2026.1.1/bin/mingw/bin/libstdc++-6.dll"
            "$<TARGET_FILE_DIR:yuv420_bmp_overlay>"

            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/Program Files/JetBrains/CLion 2026.1.1/bin/mingw/bin/libwinpthread-1.dll"
            "$<TARGET_FILE_DIR:yuv420_bmp_overlay>"
    )
endif()