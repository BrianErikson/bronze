#include <kernel/tty.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

extern "C"
__attribute__ ((constructor)) void startctor() {
  terminal_writestring("Ctor");
}

extern "C"
__attribute__ ((destructor)) void enddtor() {
  terminal_writestring("Dtor");
}

extern "C"
void kernel_early_main(void)
{
  terminal_initialize();
  terminal_writestring("Early Hello, kernel World!\n");
}

extern "C"
void kernel_main(void)
{
  /* Newline support is left as an exercise. */
  terminal_writestring("Hello, kernel World!\n");
}