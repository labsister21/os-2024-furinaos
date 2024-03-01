#include <stdbool.h>
#include "header/cpu/gdt.h"
#include "header/cpu/idt.h"
#include "header/cpu/interrupt.h"
#include "header/kernel-entrypoint.h"
#include "header/text/framebuffer.h"
#include "header/driver/keyboard.h"

void kernel_setup(void) {
    load_gdt(&_gdt_gdtr);
    pic_remap();
    activate_keyboard_interrupt();
    initialize_idt();
    framebuffer_clear();
    framebuffer_write(0, 0, '\0', 0xF, 0);
    framebuffer_set_cursor(0, 0);
    int col = 0;
    //int col_cursor =1;
    keyboard_state_activate();
    while (true) {
      char c;
      get_keyboard_buffer(&c);
      if(c != '\0'){
        framebuffer_write(0, col++, c, 0xF, 0);
        framebuffer_write(0, col, '\0', 0xF, 0);
        framebuffer_set_cursor(0,col);
      }
    }
}
