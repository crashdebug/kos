#include <stdint.h>
#include <stdio.h>
#include <terminal.h>
#include <multiboot.h>

#ifdef __cplusplus
extern "C" {
#endif

void kernel_early()
{
	
}

void kernel_main(multiboot_info_t* mbd, uint32_t magic)
{
	Terminal::initialize((uint16_t*)0xB8000, 80, 25);
	printf("[kOS v0.0.1]\n");
	printf("BootInfo: 0x%8x, Magic: 0x%x\n", mbd, magic);
	//Terminal::write("\n");
}
#ifdef __cplusplus
}
#endif

extern "C" void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}
