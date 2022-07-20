#include <stdint.h>

extern uint32_t __main_stack_end__;
extern void ResetHandler(void);

typedef void (*Vector)(void);

// Place copies of the stack pointer and reset vector at the start of memory so
// that our application will run standalone, since this space is reserved for
// the bootloader.
__attribute__ ((section(".bootloader")))
Vector PlaceholderVectors[] =
{
    (Vector)(&__main_stack_end__),
    ResetHandler,
};
