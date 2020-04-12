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

add_executable(i686-os
  ${ASM_SRCS}
)
add_dependencies(i686-os kernel)

target_link_libraries(i686-os
  kernel
)

add_custom_command(TARGET i686-os POST_BUILD
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/sanity_checks.sh
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Running sanity checks..."
)

add_custom_command(TARGET i686-os POST_BUILD
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/mkiso.sh
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Creating iso..."
)

add_custom_target(run_i686-os
  qemu-system-i386 -cdrom i686-os.iso -m 256M
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_dependencies(run_i686-os i686-os)
