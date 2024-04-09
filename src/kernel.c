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
    // makeFurina();
    initialize_filesystem_fat32();

    struct BlockBuffer b;
    for (int i = 0; i < 512; i++) b.buf[i] = i % 16;
    write_blocks(&b, 17, 1);

    struct FAT32DriverRequest request;

    request.name[0] = 't';
    request.name[1] = 'e';
    request.name[2] = 's';
    request.name[3] = 't';
    request.name[4] = 'f';
    request.name[5] = 'i';
    request.name[6] = 'l';
    request.name[7] = 'e';
    request.ext[0] = '\0';
    request.ext[1] = '\0';
    request.ext[2] = '\0';
    request.parent_cluster_number = ROOT_CLUSTER_NUMBER;
    request.buffer_size = 0;
	/* write_clusters("nata de coco", 2, 1); */
    // uint8_t hentai[512];
    // for(int i = 0; i < 512; i++){
    //     hentai[i] = i%16;
    // }

    // request.buf = hentai;

    //makeFurina();

    uint8_t test = write(&request);
	framebuffer_write(9, 9, test, 0, 0);

}
