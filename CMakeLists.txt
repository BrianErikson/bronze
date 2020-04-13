cmake_minimum_required(VERSION 3.10)

project(bronze CXX)

set(CMAKE_SYSTEM_NAME i686)
set(CMAKE_SYSTEM_PROCESSOR x86)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_CXX_COMPILER i686-elf-g++)
set(CMAKE_CXX_SIZEOF_DATA_PTR 4)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -ffreestanding)
set(CMAKE_EXE_LINKER_FLAGS -nostdlib)

set(CMAKE_SYSROOT ${CMAKE_BINARY_DIR}/sysroot)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SYSROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(kernel_SRCS
  src/kernel/kernel/kmain.cc
  src/kernel/kernel/tty.cc
)

add_library(kernel STATIC
  ${kernel_SRCS}
)

target_include_directories(kernel PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/kernel/include
)

add_custom_command(TARGET kernel POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/kernel/include ${CMAKE_SYSROOT}/usr/include
)

# crt ====================================================
enable_language(ASM_NASM)
set(CAN_USE_ASSEMBLER TRUE)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
    <FLAGS> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -o <OBJECT> <SOURCE>")
set(CMAKE_ASM_NASM_LINK_EXECUTABLE "/usr/bin/i686-elf-ld <FLAGS> <CMAKE_ASM_NASM_LINK_FLAGS> \
    <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

add_library(crti OBJECT
  src/kernel/arch/i686/crti.asm
)

add_library(crtn OBJECT
  src/kernel/arch/i686/crtn.asm
)

# bronze =================================================

set(bronze_SRCS
  src/kernel/arch/i686/boot.asm
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  foreach(asm_file bronze_SRCS)
    set_source_files_properties(asm_file "-g -Fdwarf")
  endforeach()
endif()

add_executable(bronze
  ${bronze_SRCS}
)
add_dependencies(bronze kernel crti crtn)

target_link_libraries(bronze
  $<TARGET_OBJECTS:crtn>
  kernel
  $<TARGET_OBJECTS:crti>
)

add_custom_command(TARGET bronze POST_BUILD
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/sanity_checks.sh
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Running sanity checks..."
)

add_custom_command(TARGET bronze POST_BUILD
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/mkiso.sh
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Creating iso..."
)

add_custom_target(run_bronze
  qemu-system-i386 -cdrom bronze.iso -m 256M
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_dependencies(run_bronze bronze)
