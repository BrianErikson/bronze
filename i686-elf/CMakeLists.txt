cmake_minimum_required(VERSION 3.10)

project(i686 CXX)

set(CMAKE_SYSTEM_NAME i686)
set(CMAKE_SYSTEM_PROCESSOR x86)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_CXX_COMPILER i686-elf-g++)
set(CMAKE_CXX_SIZEOF_DATA_PTR 4)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -ffreestanding)
set(CMAKE_EXE_LINKER_FLAGS -nostdlib)

add_library(kernel STATIC
  src/kmain.cc
)

# NASM =================================================
enable_language(ASM_NASM)
set(CAN_USE_ASSEMBLER TRUE)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
    <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
set(CMAKE_ASM_NASM_LINK_EXECUTABLE "/usr/bin/i686-elf-ld <FLAGS> <CMAKE_ASM_NASM_LINK_FLAGS> \
    <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

set(ASM_SRCS
  src/boot.asm
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  foreach(asm_file ASM_SRCS)
    set_source_files_properties(asm_file "-g -Fdwarf")
  endforeach()
endif()

add_executable(boot
  ${ASM_SRCS}
)
add_dependencies(boot kernel)

target_link_libraries(boot
  kernel
)
