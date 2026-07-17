cmake_minimum_required(VERSION 3.20)

project(chaos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Assembler
add_executable(chaos-assembler
    src/assembler/assembler.cpp
    src/assembler/translator.cpp
)

# VM
add_executable(chaos-vm
    src/vm/bus.cpp
    src/vm/chip.cpp
    src/vm/cpu.cpp
    src/vm/disk.cpp
    src/vm/img.cpp
    src/vm/memory.cpp
    src/vm/vm.cpp
    src/vm/display.cpp
    src/vm/machine.cpp
)

# Formatter
add_executable(chaos-formatter
    src/formatter/formatter.cpp
    src/formatter/writer.cpp
)

# Font
add_executable(chaos-font
    src/font/font.cpp
    src/font/mapping.cpp
)

find_package(raylib REQUIRED)

target_link_libraries(chaos-vm PRIVATE raylib)

# Common configuration
foreach(target chaos-assembler chaos-vm chaos-formatter chaos-font)
    target_include_directories(${target} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_compile_options(${target} PRIVATE
        -freflection
    )

    target_link_libraries(${target} PRIVATE
        stdc++exp
    )
endforeach()