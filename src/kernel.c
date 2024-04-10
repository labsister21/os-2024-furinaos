#include <stdbool.h>
#include <stdint.h>
#include "header/cpu/gdt.h"
#include "header/cpu/idt.h"
#include "header/cpu/interrupt.h"
#include "header/kernel-entrypoint.h"
#include "header/text/framebuffer.h"
#include "header/driver/keyboard.h"
#include "header/driver/disk.h"
#include "header/filesystem/fat32.h"

void kernel_setup(void) {
    load_gdt(&_gdt_gdtr);
    pic_remap();
    activate_keyboard_interrupt();
    initialize_idt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);
    initialize_filesystem_fat32();
	
    struct FAT32DriverRequest request;

    request.name[0] = 'k';
    request.name[1] = 'a';
    request.name[2] = 'n';
    request.name[3] = 'o';
    request.name[4] = '\0';
    request.name[5] = '\0';
    request.name[6] = '\0';
    request.name[7] = '\0';
    request.ext[0] = '\0';
    request.ext[1] = '\0';
    request.ext[2] = '\0';
    request.parent_cluster_number = ROOT_CLUSTER_NUMBER;
    request.buffer_size = 16000;

	int8_t result = read(&request);
	if(result == 0){
	  makeFurina();
	};


    

}
