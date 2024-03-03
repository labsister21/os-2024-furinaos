#include "../header/driver/keyboard.h"
#include "../header/text/framebuffer.h"
#include "../header/cpu/portio.h"
#include "../header/stdlib/string.h"
#include <stdbool.h>
#include <stdint.h>

uint8_t NEWLINE_POS = 0;
uint8_t COLUMN = 0;
uint8_t FURINA_OFFSET = 22;

struct KeyboardDriverState keyboard_state = {
    .keyboard_input_on = false
} ;

const char keyboard_scancode_1_to_ascii_map[256] = {
      0, 0x1B, '1', '2', '3', '4', '5', '6',  '7', '8', '9',  '0',  '-', '=', '\b', '\t',
    'q',  'w', 'e', 'r', 't', 'y', 'u', 'i',  'o', 'p', '[',  ']', '\n',   0,  'a',  's',
    'd',  'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0, '\\',  'z', 'x',  'c',  'v',
    'b',  'n', 'm', ',', '.', '/',   0, '*',    0, ' ',   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0, '-',    0,    0,   0,  '+',    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,

      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
};

void keyboard_isr(void) {
    if(keyboard_state.keyboard_input_on){
        uint8_t scancode = (uint8_t) in(KEYBOARD_DATA_PORT);
        uint8_t ascii = (char) keyboard_scancode_1_to_ascii_map[scancode];

        if(ascii == '\n'){
            COLUMN = 0;
            NEWLINE_POS++;
            framebuffer_set_cursor(NEWLINE_POS, COLUMN);
            framebuffer_write(NEWLINE_POS, COLUMN, '\0', 0xF, 0);
        }
        else if(ascii == '\t'){
            framebuffer_write(NEWLINE_POS, COLUMN++, ' ', 0xF, 0);
            framebuffer_write(NEWLINE_POS, COLUMN++, ' ', 0xF, 0);
            framebuffer_write(NEWLINE_POS, COLUMN++, ' ', 0xF, 0);
            framebuffer_write(NEWLINE_POS, COLUMN++, ' ', 0xF, 0);
            framebuffer_set_cursor(NEWLINE_POS, COLUMN);
            framebuffer_write(NEWLINE_POS, COLUMN, '\0', 0xF, 0);
        }
        else if(ascii == '\b'){
            if(COLUMN != 0){
                framebuffer_set_cursor(NEWLINE_POS, (COLUMN--)-1);
                framebuffer_write(NEWLINE_POS, COLUMN, '\0', 0xF, 0);

                // TODO : Implement multi row deletion
            }
        }
        else{
            keyboard_state.keyboard_buffer = ascii;
            if(ascii != '\0'){
                framebuffer_write(NEWLINE_POS, COLUMN++, ascii, 0xF, 0);
                framebuffer_write(NEWLINE_POS, COLUMN, '\0', 0xF, 0);
                framebuffer_set_cursor(NEWLINE_POS,COLUMN);
                if(COLUMN > 80){
                    NEWLINE_POS++;
                    COLUMN = 0;
                }
    	}
        }
        pic_ack(IRQ_KEYBOARD + PIC1_OFFSET);

        return;
    }
}

void keyboard_state_activate(void){
    keyboard_state.keyboard_input_on = true;
}

void keyboard_state_deactivate(void){
    keyboard_state.keyboard_input_on = false; 
}

void get_keyboard_buffer(char *buf){
    char newC = keyboard_state.keyboard_buffer;
    *buf = newC;
    keyboard_state.keyboard_buffer = '\0';
}

void makeFurina(void){

    // Layer 1
	framebuffer_write(24, FURINA_OFFSET + 3, ' ', 0, 9);
	framebuffer_write(24, FURINA_OFFSET + 4, ' ', 0, 9);
	framebuffer_write(24, FURINA_OFFSET + 5, ' ', 0, 9);
	framebuffer_write(24, FURINA_OFFSET + 6, ' ', 0, 9);
	framebuffer_write(24, FURINA_OFFSET + 7, ' ', 0, 9);
	framebuffer_write(24, FURINA_OFFSET + 8, ' ', 0, 9);
	framebuffer_write(24, FURINA_OFFSET + 9, ' ', 0, 7);
	framebuffer_write(24, FURINA_OFFSET + 10, ' ', 0, 7);
	framebuffer_write(24, FURINA_OFFSET + 11, ' ', 0, 9);
	framebuffer_write(24, FURINA_OFFSET + 12, ' ', 0, 9);
	framebuffer_write(24, FURINA_OFFSET + 13, ' ', 0, 11);
	framebuffer_write(24, FURINA_OFFSET + 14, ' ', 0, 11);
	framebuffer_write(24, FURINA_OFFSET + 15, ' ', 0, 15);
	framebuffer_write(24, FURINA_OFFSET + 16, ' ', 0, 15);
	framebuffer_write(24, FURINA_OFFSET + 17, ' ', 0, 15);
	framebuffer_write(24, FURINA_OFFSET + 18, ' ', 0, 15);
	framebuffer_write(24, FURINA_OFFSET + 19, ' ', 0, 3);
	framebuffer_write(24, FURINA_OFFSET + 20, ' ', 0, 3);
	framebuffer_write(24, FURINA_OFFSET + 21, ' ', 0, 3);
	framebuffer_write(24, FURINA_OFFSET + 22, ' ', 0, 3);
	framebuffer_write(24, FURINA_OFFSET + 25, ' ', 0, 14);
	framebuffer_write(24, FURINA_OFFSET + 26, ' ', 0, 14);
	framebuffer_write(24, FURINA_OFFSET + 27, ' ', 0, 9);
	framebuffer_write(24, FURINA_OFFSET + 28, ' ', 0, 9);
	framebuffer_write(24, FURINA_OFFSET + 29, ' ', 0, 9);
	framebuffer_write(24, FURINA_OFFSET + 30, ' ', 0, 9);

    // Layer 2
    framebuffer_write(23, FURINA_OFFSET + 5, ' ', 0, 9);
    framebuffer_write(23, FURINA_OFFSET + 6, ' ', 0, 9);
    framebuffer_write(23, FURINA_OFFSET + 7, ' ', 0, 9);
    framebuffer_write(23, FURINA_OFFSET + 8, ' ', 0, 9);
    framebuffer_write(23, FURINA_OFFSET + 11, ' ', 0, 9);
    framebuffer_write(23, FURINA_OFFSET + 12, ' ', 0, 9);
    framebuffer_write(23, FURINA_OFFSET + 13, ' ', 0, 11);
    framebuffer_write(23, FURINA_OFFSET + 14, ' ', 0, 11);
    framebuffer_write(23, FURINA_OFFSET + 15, ' ', 0, 15);
    framebuffer_write(23, FURINA_OFFSET + 16, ' ', 0, 15);
    framebuffer_write(23, FURINA_OFFSET + 17, ' ', 0, 8);
    framebuffer_write(23, FURINA_OFFSET + 18, ' ', 0, 8);
    framebuffer_write(23, FURINA_OFFSET + 19, ' ', 0, 8);
    framebuffer_write(23, FURINA_OFFSET + 20, ' ', 0, 8);
    framebuffer_write(23, FURINA_OFFSET + 21, ' ', 0, 11);
    framebuffer_write(23, FURINA_OFFSET + 22, ' ', 0, 11);
    framebuffer_write(23, FURINA_OFFSET + 25, ' ', 0, 9);
    framebuffer_write(23, FURINA_OFFSET + 26, ' ', 0, 9);
    framebuffer_write(23, FURINA_OFFSET + 27, ' ', 0, 9);
    framebuffer_write(23, FURINA_OFFSET + 28, ' ', 0, 9);

    // Layer 3
    framebuffer_write(22, FURINA_OFFSET + 5, ' ', 0, 9);
    framebuffer_write(22, FURINA_OFFSET + 6, ' ', 0, 9);
    framebuffer_write(22, FURINA_OFFSET + 7, ' ', 0, 9);
    framebuffer_write(22, FURINA_OFFSET + 8, ' ', 0, 9);
    framebuffer_write(22, FURINA_OFFSET + 9, ' ', 0, 9);
    framebuffer_write(22, FURINA_OFFSET + 10, ' ', 0, 9);
    framebuffer_write(22, FURINA_OFFSET + 11, ' ', 0, 11);
    framebuffer_write(22, FURINA_OFFSET + 12, ' ', 0, 11);
    framebuffer_write(22, FURINA_OFFSET + 13, ' ', 0, 9);
    framebuffer_write(22, FURINA_OFFSET + 14, ' ', 0, 9);
    framebuffer_write(22, FURINA_OFFSET + 15, ' ', 0, 9);
    framebuffer_write(22, FURINA_OFFSET + 16, ' ', 0, 9);
    framebuffer_write(22, FURINA_OFFSET + 17, ' ', 0, 8);
    framebuffer_write(22, FURINA_OFFSET + 18, ' ', 0, 8);
    framebuffer_write(22, FURINA_OFFSET + 19, ' ', 0, 8);
    framebuffer_write(22, FURINA_OFFSET + 20, ' ', 0, 8);
    framebuffer_write(22, FURINA_OFFSET + 21, ' ', 0, 11);
    framebuffer_write(22, FURINA_OFFSET + 22, ' ', 0, 11);
    framebuffer_write(22, FURINA_OFFSET + 23, ' ', 0, 9);
    framebuffer_write(22, FURINA_OFFSET + 24, ' ', 0, 9);
    framebuffer_write(22, FURINA_OFFSET + 25, ' ', 0, 9);
    framebuffer_write(22, FURINA_OFFSET + 26, ' ', 0, 9);

    // Layer 4
    framebuffer_write(21, FURINA_OFFSET + 7, ' ', 0, 9);
    framebuffer_write(21, FURINA_OFFSET + 8, ' ', 0, 9);
    framebuffer_write(21, FURINA_OFFSET + 9, ' ', 0, 9);
    framebuffer_write(21, FURINA_OFFSET + 10, ' ', 0, 9);
    framebuffer_write(21, FURINA_OFFSET + 11, ' ', 0, 11);
    framebuffer_write(21, FURINA_OFFSET + 12, ' ', 0, 11);
    framebuffer_write(21, FURINA_OFFSET + 13, ' ', 0, 9);
    framebuffer_write(21, FURINA_OFFSET + 14, ' ', 0, 9);
    framebuffer_write(21, FURINA_OFFSET + 15, ' ', 0, 8);
    framebuffer_write(21, FURINA_OFFSET + 16, ' ', 0, 8);
    framebuffer_write(21, FURINA_OFFSET + 17, ' ', 0, 3);
    framebuffer_write(21, FURINA_OFFSET + 18, ' ', 0, 3);
    framebuffer_write(21, FURINA_OFFSET + 19, ' ', 0, 9);
    framebuffer_write(21, FURINA_OFFSET + 20, ' ', 0, 9);
    framebuffer_write(21, FURINA_OFFSET + 21, ' ', 0, 9);
    framebuffer_write(21, FURINA_OFFSET + 22, ' ', 0, 9);
    framebuffer_write(21, FURINA_OFFSET + 23, ' ', 0, 11);
    framebuffer_write(21, FURINA_OFFSET + 24, ' ', 0, 11);
    framebuffer_write(21, FURINA_OFFSET + 25, ' ', 0, 9);
    framebuffer_write(21, FURINA_OFFSET + 26, ' ', 0, 9);

    // Layer 5
	framebuffer_write(20, FURINA_OFFSET + 9, ' ', 0, 9);
	framebuffer_write(20, FURINA_OFFSET + 10, ' ', 0, 9);
	framebuffer_write(20, FURINA_OFFSET + 11, ' ', 0, 9);
	framebuffer_write(20, FURINA_OFFSET + 12, ' ', 0, 9);
	framebuffer_write(20, FURINA_OFFSET + 13, ' ', 0, 8);
	framebuffer_write(20, FURINA_OFFSET + 14, ' ', 0, 8);
	framebuffer_write(20, FURINA_OFFSET + 15, ' ', 0, 8);
	framebuffer_write(20, FURINA_OFFSET + 16, ' ', 0, 8);
	framebuffer_write(20, FURINA_OFFSET + 17, ' ', 0, 8);
	framebuffer_write(20, FURINA_OFFSET + 18, ' ', 0, 8);
	framebuffer_write(20, FURINA_OFFSET + 19, ' ', 0, 9);
	framebuffer_write(20, FURINA_OFFSET + 20, ' ', 0, 9);
	framebuffer_write(20, FURINA_OFFSET + 21, ' ', 0, 7);
	framebuffer_write(20, FURINA_OFFSET + 22, ' ', 0, 7);
	framebuffer_write(20, FURINA_OFFSET + 23, ' ', 0, 7);
	framebuffer_write(20, FURINA_OFFSET + 24, ' ', 0, 7);

    // Layer 6
    framebuffer_write(19, FURINA_OFFSET + 3, ' ', 0, 15);
    framebuffer_write(19, FURINA_OFFSET + 4, ' ', 0, 15);
    framebuffer_write(19, FURINA_OFFSET + 5, ' ', 0, 15);
    framebuffer_write(19, FURINA_OFFSET + 6, ' ', 0, 15);
    framebuffer_write(19, FURINA_OFFSET + 9, ' ', 0, 7);
    framebuffer_write(19, FURINA_OFFSET + 10, ' ', 0, 7);
    framebuffer_write(19, FURINA_OFFSET + 13, ' ', 0, 8);
    framebuffer_write(19, FURINA_OFFSET + 14, ' ', 0, 8);
    framebuffer_write(19, FURINA_OFFSET + 15, ' ', 0, 15);
    framebuffer_write(19, FURINA_OFFSET + 16, ' ', 0, 15);
    framebuffer_write(19, FURINA_OFFSET + 21, ' ', 0, 7);
    framebuffer_write(19, FURINA_OFFSET + 22, ' ', 0, 7);
    framebuffer_write(19, FURINA_OFFSET + 25, ' ', 0, 15);
    framebuffer_write(19, FURINA_OFFSET + 26, ' ', 0, 15);
    framebuffer_write(19, FURINA_OFFSET + 27, ' ', 0, 15);
    framebuffer_write(19, FURINA_OFFSET + 28, ' ', 0, 15);

    //Layer 7
    framebuffer_write(18, FURINA_OFFSET + 1, ' ', 0, 7);
    framebuffer_write(18, FURINA_OFFSET + 2, ' ', 0, 7);
    framebuffer_write(18, FURINA_OFFSET + 3, ' ', 0, 7);
    framebuffer_write(18, FURINA_OFFSET + 4, ' ', 0, 7);
    framebuffer_write(18, FURINA_OFFSET + 5, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 6, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 7, ' ', 0, 7);
    framebuffer_write(18, FURINA_OFFSET + 8, ' ', 0, 7);
    framebuffer_write(18, FURINA_OFFSET + 9, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 10, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 11, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 12, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 13, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 14, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 15, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 16, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 17, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 18, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 19, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 20, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 21, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 22, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 23, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 24, ' ', 0, 15);
    framebuffer_write(18, FURINA_OFFSET + 25, ' ', 0, 7);
    framebuffer_write(18, FURINA_OFFSET + 26, ' ', 0, 7);
    framebuffer_write(18, FURINA_OFFSET + 27, ' ', 0, 7);
    framebuffer_write(18, FURINA_OFFSET + 28, ' ', 0, 7);
    framebuffer_write(18, FURINA_OFFSET + 29, ' ', 0, 7);
    framebuffer_write(18, FURINA_OFFSET + 30, ' ', 0, 7);

    // Layer 8
    framebuffer_write(17, FURINA_OFFSET + 3, ' ', 0, 11);
    framebuffer_write(17, FURINA_OFFSET + 4, ' ', 0, 11);
    framebuffer_write(17, FURINA_OFFSET + 5, ' ', 0, 7);
    framebuffer_write(17, FURINA_OFFSET + 6, ' ', 0, 7);
    framebuffer_write(17, FURINA_OFFSET + 7, ' ', 0, 7);
    framebuffer_write(17, FURINA_OFFSET + 8, ' ', 0, 7);
    framebuffer_write(17, FURINA_OFFSET + 9, ' ', 0, 15);
    framebuffer_write(17, FURINA_OFFSET + 10, ' ', 0, 15);
    framebuffer_write(17, FURINA_OFFSET + 11, ' ', 0, 15);
    framebuffer_write(17, FURINA_OFFSET + 12, ' ', 0, 15);
    framebuffer_write(17, FURINA_OFFSET + 13, ' ', 0, 11);
    framebuffer_write(17, FURINA_OFFSET + 14, ' ', 0, 11);
    framebuffer_write(17, FURINA_OFFSET + 15, ' ', 0, 15);
    framebuffer_write(17, FURINA_OFFSET + 16, ' ', 0, 15);
    framebuffer_write(17, FURINA_OFFSET + 17, ' ', 0, 15);
    framebuffer_write(17, FURINA_OFFSET + 18, ' ', 0, 15);
    framebuffer_write(17, FURINA_OFFSET + 19, ' ', 0, 7);
    framebuffer_write(17, FURINA_OFFSET + 20, ' ', 0, 7);
    framebuffer_write(17, FURINA_OFFSET + 21, ' ', 0, 9);
    framebuffer_write(17, FURINA_OFFSET + 22, ' ', 0, 9);
    framebuffer_write(17, FURINA_OFFSET + 23, ' ', 0, 15);
    framebuffer_write(17, FURINA_OFFSET + 24, ' ', 0, 15);
    framebuffer_write(17, FURINA_OFFSET + 25, ' ', 0, 15);
    framebuffer_write(17, FURINA_OFFSET + 26, ' ', 0, 15);
    framebuffer_write(17, FURINA_OFFSET + 27, ' ', 0, 11);
    framebuffer_write(17, FURINA_OFFSET + 28, ' ', 0, 11);

    // Layer 9
    framebuffer_write(16, FURINA_OFFSET + 1, ' ', 0, 7);
    framebuffer_write(16, FURINA_OFFSET + 2, ' ', 0, 7);
    framebuffer_write(16, FURINA_OFFSET + 3, ' ', 0, 11);
    framebuffer_write(16, FURINA_OFFSET + 4, ' ', 0, 11);
    framebuffer_write(16, FURINA_OFFSET + 5, ' ', 0, 11);
    framebuffer_write(16, FURINA_OFFSET + 6, ' ', 0, 11);
    framebuffer_write(16, FURINA_OFFSET + 7, ' ', 0, 7);
    framebuffer_write(16, FURINA_OFFSET + 8, ' ', 0, 7);
    framebuffer_write(16, FURINA_OFFSET + 9, ' ', 0, 8);
    framebuffer_write(16, FURINA_OFFSET + 10, ' ', 0, 8);
    framebuffer_write(16, FURINA_OFFSET + 11, ' ', 0, 15);
    framebuffer_write(16, FURINA_OFFSET + 12, ' ', 0, 15);
    framebuffer_write(16, FURINA_OFFSET + 13, ' ', 0, 9);
    framebuffer_write(16, FURINA_OFFSET + 14, ' ', 0, 9);
    framebuffer_write(16, FURINA_OFFSET + 15, ' ', 0, 15);
    framebuffer_write(16, FURINA_OFFSET + 16, ' ', 0, 15);
    framebuffer_write(16, FURINA_OFFSET + 17, ' ', 0, 7);
    framebuffer_write(16, FURINA_OFFSET + 18, ' ', 0, 7);
    framebuffer_write(16, FURINA_OFFSET + 19, ' ', 0, 7);
    framebuffer_write(16, FURINA_OFFSET + 20, ' ', 0, 7);
    framebuffer_write(16, FURINA_OFFSET + 21, ' ', 0, 1);
    framebuffer_write(16, FURINA_OFFSET + 22, ' ', 0, 1);
    framebuffer_write(16, FURINA_OFFSET + 23, ' ', 0, 15);
    framebuffer_write(16, FURINA_OFFSET + 24, ' ', 0, 15);
    framebuffer_write(16, FURINA_OFFSET + 25, ' ', 0, 7);
    framebuffer_write(16, FURINA_OFFSET + 26, ' ', 0, 7);
    framebuffer_write(16, FURINA_OFFSET + 27, ' ', 0, 7);
    framebuffer_write(16, FURINA_OFFSET + 28, ' ', 0, 7);

    // Layer 10
    framebuffer_write(15, FURINA_OFFSET + 1, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 2, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 3, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 4, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 5, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 6, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 7, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 8, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 9, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 10, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 11, ' ', 0, 8);
    framebuffer_write(15, FURINA_OFFSET + 12, ' ', 0, 8);
    framebuffer_write(15, FURINA_OFFSET + 13, ' ', 0, 8);
    framebuffer_write(15, FURINA_OFFSET + 14, ' ', 0, 8);
    framebuffer_write(15, FURINA_OFFSET + 15, ' ', 0, 8);
    framebuffer_write(15, FURINA_OFFSET + 16, ' ', 0, 8);
    framebuffer_write(15, FURINA_OFFSET + 17, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 18, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 19, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 20, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 21, ' ', 0, 8);
    framebuffer_write(15, FURINA_OFFSET + 22, ' ', 0, 8);
    framebuffer_write(15, FURINA_OFFSET + 23, ' ', 0, 8);
    framebuffer_write(15, FURINA_OFFSET + 24, ' ', 0, 8);
    framebuffer_write(15, FURINA_OFFSET + 25, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 26, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 27, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 28, ' ', 0, 7);
    framebuffer_write(15, FURINA_OFFSET + 31, ' ', 0, 15);
    framebuffer_write(15, FURINA_OFFSET + 32, ' ', 0, 15);

    // Layer 11
    framebuffer_write(14, FURINA_OFFSET + 1, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 2, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 3, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 4, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 5, ' ', 0, 15);
    framebuffer_write(14, FURINA_OFFSET + 6, ' ', 0, 15);
    framebuffer_write(14, FURINA_OFFSET + 7, ' ', 0, 15);
    framebuffer_write(14, FURINA_OFFSET + 8, ' ', 0, 15);
    framebuffer_write(14, FURINA_OFFSET + 9, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 10, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 11, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 12, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 13, ' ', 0, 15);
    framebuffer_write(14, FURINA_OFFSET + 14, ' ', 0, 15);
    framebuffer_write(14, FURINA_OFFSET + 15, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 16, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 17, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 18, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 19, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 20, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 21, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 22, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 23, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 24, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 25, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 26, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 27, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 28, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 29, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 30, ' ', 0, 7);
    framebuffer_write(14, FURINA_OFFSET + 31, ' ', 0, 15);
    framebuffer_write(14, FURINA_OFFSET + 32, ' ', 0, 15);

    // Layer 12
    framebuffer_write(13, FURINA_OFFSET + 3, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 4, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 5, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 6, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 7, ' ', 0, 15);
    framebuffer_write(13, FURINA_OFFSET + 8, ' ', 0, 15);
    framebuffer_write(13, FURINA_OFFSET + 9, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 10, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 11, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 12, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 13, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 14, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 15, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 16, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 17, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 18, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 19, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 20, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 21, ' ', 0, 11);
    framebuffer_write(13, FURINA_OFFSET + 22, ' ', 0, 11);
    framebuffer_write(13, FURINA_OFFSET + 23, ' ', 0, 11);
    framebuffer_write(13, FURINA_OFFSET + 24, ' ', 0, 11);
    framebuffer_write(13, FURINA_OFFSET + 25, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 26, ' ', 0, 7);
    framebuffer_write(13, FURINA_OFFSET + 29, ' ', 0, 15);
    framebuffer_write(13, FURINA_OFFSET + 30, ' ', 0, 15);

    // Layer 13
    framebuffer_write(12, FURINA_OFFSET + 5, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 6, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 7, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 8, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 9, ' ', 0, 15);
    framebuffer_write(12, FURINA_OFFSET + 10, ' ', 0, 15);
    framebuffer_write(12, FURINA_OFFSET + 11, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 12, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 13, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 14, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 15, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 16, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 17, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 18, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 19, ' ', 0, 11);
    framebuffer_write(12, FURINA_OFFSET + 20, ' ', 0, 11);
    framebuffer_write(12, FURINA_OFFSET + 21, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 22, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 23, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 24, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 25, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 26, ' ', 0, 7);
    framebuffer_write(12, FURINA_OFFSET + 27, ' ', 0, 9);
    framebuffer_write(12, FURINA_OFFSET + 28, ' ', 0, 9);
    framebuffer_write(12, FURINA_OFFSET + 29, ' ', 0, 15);
    framebuffer_write(12, FURINA_OFFSET + 30, ' ', 0, 15);

    // Layer 14
    framebuffer_write(11, FURINA_OFFSET + 5, ' ', 0, 7);
    framebuffer_write(11, FURINA_OFFSET + 6, ' ', 0, 7);
    framebuffer_write(11, FURINA_OFFSET + 7, ' ', 0, 7);
    framebuffer_write(11, FURINA_OFFSET + 8, ' ', 0, 7);
    framebuffer_write(11, FURINA_OFFSET + 9, ' ', 0, 15);
    framebuffer_write(11, FURINA_OFFSET + 10, ' ', 0, 15);
    framebuffer_write(11, FURINA_OFFSET + 11, ' ', 0, 7);
    framebuffer_write(11, FURINA_OFFSET + 12, ' ', 0, 7);
    framebuffer_write(11, FURINA_OFFSET + 13, ' ', 0, 7);
    framebuffer_write(11, FURINA_OFFSET + 14, ' ', 0, 7);
    framebuffer_write(11, FURINA_OFFSET + 15, ' ', 0, 7);
    framebuffer_write(11, FURINA_OFFSET + 16, ' ', 0, 7);
    framebuffer_write(11, FURINA_OFFSET + 17, ' ', 0, 11);
    framebuffer_write(11, FURINA_OFFSET + 18, ' ', 0, 11);
    framebuffer_write(11, FURINA_OFFSET + 19, ' ', 0, 7);
    framebuffer_write(11, FURINA_OFFSET + 20, ' ', 0, 7);
    framebuffer_write(11, FURINA_OFFSET + 21, ' ', 0, 7);
    framebuffer_write(11, FURINA_OFFSET + 22, ' ', 0, 7);
    framebuffer_write(11, FURINA_OFFSET + 23, ' ', 0, 7);
    framebuffer_write(11, FURINA_OFFSET + 24, ' ', 0, 7);
    framebuffer_write(11, FURINA_OFFSET + 25, ' ', 0, 9);
    framebuffer_write(11, FURINA_OFFSET + 26, ' ', 0, 9);
    framebuffer_write(11, FURINA_OFFSET + 27, ' ', 0, 15);
    framebuffer_write(11, FURINA_OFFSET + 28, ' ', 0, 15);

    // Layer 15
    framebuffer_write(10, FURINA_OFFSET + 7, ' ', 0, 7);
    framebuffer_write(10, FURINA_OFFSET + 8, ' ', 0, 7);
    framebuffer_write(10, FURINA_OFFSET + 9, ' ', 0, 7);
    framebuffer_write(10, FURINA_OFFSET + 10, ' ', 0, 7);
    framebuffer_write(10, FURINA_OFFSET + 11, ' ', 0, 15);
    framebuffer_write(10, FURINA_OFFSET + 12, ' ', 0, 15);
    framebuffer_write(10, FURINA_OFFSET + 13, ' ', 0, 7);
    framebuffer_write(10, FURINA_OFFSET + 14, ' ', 0, 7);
    framebuffer_write(10, FURINA_OFFSET + 15, ' ', 0, 7);
    framebuffer_write(10, FURINA_OFFSET + 16, ' ', 0, 7);
    framebuffer_write(10, FURINA_OFFSET + 17, ' ', 0, 7);
    framebuffer_write(10, FURINA_OFFSET + 18, ' ', 0, 7);
    framebuffer_write(10, FURINA_OFFSET + 19, ' ', 0, 7);
    framebuffer_write(10, FURINA_OFFSET + 20, ' ', 0, 7);
    framebuffer_write(10, FURINA_OFFSET + 21, ' ', 0, 9);
    framebuffer_write(10, FURINA_OFFSET + 22, ' ', 0, 9);
    framebuffer_write(10, FURINA_OFFSET + 23, ' ', 0, 9);
    framebuffer_write(10, FURINA_OFFSET + 24, ' ', 0, 9);
    framebuffer_write(10, FURINA_OFFSET + 25, ' ', 0, 9);
    framebuffer_write(10, FURINA_OFFSET + 26, ' ', 0, 9);

    // Layer 16
    framebuffer_write(9 , FURINA_OFFSET + 9, ' ', 0, 11);
    framebuffer_write(9 , FURINA_OFFSET + 10, ' ', 0, 11);
    framebuffer_write(9 , FURINA_OFFSET + 11, ' ', 0, 7);
    framebuffer_write(9 , FURINA_OFFSET + 12, ' ', 0, 7);
    framebuffer_write(9 , FURINA_OFFSET + 13, ' ', 0, 7);
    framebuffer_write(9 , FURINA_OFFSET + 14, ' ', 0, 7);
    framebuffer_write(9 , FURINA_OFFSET + 15, ' ', 0, 7);
    framebuffer_write(9 , FURINA_OFFSET + 16, ' ', 0, 7);
    framebuffer_write(9 , FURINA_OFFSET + 17, ' ', 0, 7);
    framebuffer_write(9 , FURINA_OFFSET + 18, ' ', 0, 7);
    framebuffer_write(9 , FURINA_OFFSET + 19, ' ', 0, 9);
    framebuffer_write(9 , FURINA_OFFSET + 20, ' ', 0, 9);
    framebuffer_write(9 , FURINA_OFFSET + 21, ' ', 0, 9);
    framebuffer_write(9 , FURINA_OFFSET + 22, ' ', 0, 9);
    framebuffer_write(9 , FURINA_OFFSET + 23, ' ', 0, 3);
    framebuffer_write(9 , FURINA_OFFSET + 24, ' ', 0, 3);
    framebuffer_write(9 , FURINA_OFFSET + 25, ' ', 0, 3);
    framebuffer_write(9 , FURINA_OFFSET + 26, ' ', 0, 3);
    framebuffer_write(9 , FURINA_OFFSET + 27, ' ', 0, 9);
    framebuffer_write(9 , FURINA_OFFSET + 28, ' ', 0, 9);

    // Layer 17
    framebuffer_write(8, FURINA_OFFSET + 5, ' ', 0, 11);
    framebuffer_write(8, FURINA_OFFSET + 6, ' ', 0, 11);
    framebuffer_write(8, FURINA_OFFSET + 7, ' ', 0, 11);
    framebuffer_write(8, FURINA_OFFSET + 8, ' ', 0, 11);
    framebuffer_write(8, FURINA_OFFSET + 11, ' ', 0, 1);
    framebuffer_write(8, FURINA_OFFSET + 12, ' ', 0, 1);
    framebuffer_write(8, FURINA_OFFSET + 13, ' ', 0, 1);
    framebuffer_write(8, FURINA_OFFSET + 14, ' ', 0, 1);
    framebuffer_write(8, FURINA_OFFSET + 15, ' ', 0, 1);
    framebuffer_write(8, FURINA_OFFSET + 16, ' ', 0, 1);
    framebuffer_write(8, FURINA_OFFSET + 17, ' ', 0, 9);
    framebuffer_write(8, FURINA_OFFSET + 18, ' ', 0, 9);
    framebuffer_write(8, FURINA_OFFSET + 19, ' ', 0, 3);
    framebuffer_write(8, FURINA_OFFSET + 20, ' ', 0, 3);
    framebuffer_write(8, FURINA_OFFSET + 21, ' ', 0, 3);
    framebuffer_write(8, FURINA_OFFSET + 22, ' ', 0, 3);
    framebuffer_write(8, FURINA_OFFSET + 23, ' ', 0, 9);
    framebuffer_write(8, FURINA_OFFSET + 24, ' ', 0, 9);
    framebuffer_write(8, FURINA_OFFSET + 25, ' ', 0, 9);
    framebuffer_write(8, FURINA_OFFSET + 26, ' ', 0, 9);
    framebuffer_write(8, FURINA_OFFSET + 27, ' ', 0, 14);
    framebuffer_write(8, FURINA_OFFSET + 28, ' ', 0, 14);
    framebuffer_write(8, FURINA_OFFSET + 29, ' ', 0, 9);
    framebuffer_write(8, FURINA_OFFSET + 30, ' ', 0, 9);

    // Layer 18
    framebuffer_write(7, FURINA_OFFSET + 5, ' ', 0, 11);
    framebuffer_write(7, FURINA_OFFSET + 6, ' ', 0, 11);
    framebuffer_write(7, FURINA_OFFSET + 11, ' ', 0, 1);
    framebuffer_write(7, FURINA_OFFSET + 12, ' ', 0, 1);
    framebuffer_write(7, FURINA_OFFSET + 13, ' ', 0, 1);
    framebuffer_write(7, FURINA_OFFSET + 14, ' ', 0, 1);
    framebuffer_write(7, FURINA_OFFSET + 15, ' ', 0, 9);
    framebuffer_write(7, FURINA_OFFSET + 16, ' ', 0, 9);
    framebuffer_write(7, FURINA_OFFSET + 17, ' ', 0, 3);
    framebuffer_write(7, FURINA_OFFSET + 18, ' ', 0, 3);
    framebuffer_write(7, FURINA_OFFSET + 19, ' ', 0, 9);
    framebuffer_write(7, FURINA_OFFSET + 20, ' ', 0, 9);
    framebuffer_write(7, FURINA_OFFSET + 21, ' ', 0, 9);
    framebuffer_write(7, FURINA_OFFSET + 22, ' ', 0, 9);
    framebuffer_write(7, FURINA_OFFSET + 23, ' ', 0, 9);
    framebuffer_write(7, FURINA_OFFSET + 24, ' ', 0, 9);
    framebuffer_write(7, FURINA_OFFSET + 25, ' ', 0, 9);
    framebuffer_write(7, FURINA_OFFSET + 26, ' ', 0, 9);
    framebuffer_write(7, FURINA_OFFSET + 27, ' ', 0, 9);
    framebuffer_write(7, FURINA_OFFSET + 28, ' ', 0, 9);
    framebuffer_write(7, FURINA_OFFSET + 29, ' ', 0, 14);
    framebuffer_write(7, FURINA_OFFSET + 30, ' ', 0, 14);

    // Layer 19
    framebuffer_write(6, FURINA_OFFSET + 3, ' ', 0, 11);
    framebuffer_write(6, FURINA_OFFSET + 4, ' ', 0, 11);
    framebuffer_write(6, FURINA_OFFSET + 11, ' ', 0, 1);
    framebuffer_write(6, FURINA_OFFSET + 12, ' ', 0, 1);
    framebuffer_write(6, FURINA_OFFSET + 13, ' ', 0, 9);
    framebuffer_write(6, FURINA_OFFSET + 14, ' ', 0, 9);
    framebuffer_write(6, FURINA_OFFSET + 17, ' ', 0, 9);
    framebuffer_write(6, FURINA_OFFSET + 18, ' ', 0, 9);
    framebuffer_write(6, FURINA_OFFSET + 19, ' ', 0, 9);
    framebuffer_write(6, FURINA_OFFSET + 20, ' ', 0, 9);
    framebuffer_write(6, FURINA_OFFSET + 21, ' ', 0, 14);
    framebuffer_write(6, FURINA_OFFSET + 22, ' ', 0, 14);
    framebuffer_write(6, FURINA_OFFSET + 23, ' ', 0, 9);
    framebuffer_write(6, FURINA_OFFSET + 24, ' ', 0, 9);
    framebuffer_write(6, FURINA_OFFSET + 25, ' ', 0, 9);
    framebuffer_write(6, FURINA_OFFSET + 26, ' ', 0, 9);
    framebuffer_write(6, FURINA_OFFSET + 27, ' ', 0, 14);
    framebuffer_write(6, FURINA_OFFSET + 28, ' ', 0, 14);
    framebuffer_write(6, FURINA_OFFSET + 31, ' ', 0, 14);
    framebuffer_write(6, FURINA_OFFSET + 32, ' ', 0, 14);

    // Layer 20
    framebuffer_write(5, FURINA_OFFSET + 3, ' ', 0, 7);
    framebuffer_write(5, FURINA_OFFSET + 4, ' ', 0, 7);
    framebuffer_write(5, FURINA_OFFSET + 5, ' ', 0, 11);
    framebuffer_write(5, FURINA_OFFSET + 6, ' ', 0, 11);
    framebuffer_write(5, FURINA_OFFSET + 19, ' ', 0, 9);
    framebuffer_write(5, FURINA_OFFSET + 20, ' ', 0, 9);
    framebuffer_write(5, FURINA_OFFSET + 21, ' ', 0, 9);
    framebuffer_write(5, FURINA_OFFSET + 22, ' ', 0, 9);
    framebuffer_write(5, FURINA_OFFSET + 23, ' ', 0, 14);
    framebuffer_write(5, FURINA_OFFSET + 24, ' ', 0, 14);

    // Layer 21
    framebuffer_write(4, FURINA_OFFSET + 5, ' ', 0, 7);
    framebuffer_write(4, FURINA_OFFSET + 6, ' ', 0, 7);
    framebuffer_write(4, FURINA_OFFSET + 7, ' ', 0, 7);
    framebuffer_write(4, FURINA_OFFSET + 8, ' ', 0, 7);
    framebuffer_write(4, FURINA_OFFSET + 19, ' ', 0, 14);
    framebuffer_write(4, FURINA_OFFSET + 20, ' ', 0, 14);
    framebuffer_write(4, FURINA_OFFSET + 21, ' ', 0, 9);
    framebuffer_write(4, FURINA_OFFSET + 22, ' ', 0, 9);
    framebuffer_write(4, FURINA_OFFSET + 25, ' ', 0, 14);
    framebuffer_write(4, FURINA_OFFSET + 26, ' ', 0, 14);

    // Layer 22
    framebuffer_write(3, FURINA_OFFSET + 19, ' ', 0, 14);
    framebuffer_write(3, FURINA_OFFSET + 20, ' ', 0, 14);
}