#include "../header/driver/keyboard.h"
#include "../header/text/framebuffer.h"
#include "../header/cpu/portio.h"
#include "../header/stdlib/string.h"
#include <stdbool.h>
#include <stdint.h>

uint8_t NEWLINE_POS = 0;
uint8_t COLUMN = 0;

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
    
    // uint8_t OFFSET = 0;

    // Layer 1
	framebuffer_write(24, 3, ' ', 0, 9);
	framebuffer_write(24, 4, ' ', 0, 9);
	framebuffer_write(24, 5, ' ', 0, 9);
	framebuffer_write(24, 6, ' ', 0, 9);
	framebuffer_write(24, 7, ' ', 0, 9);
	framebuffer_write(24, 8, ' ', 0, 9);
	framebuffer_write(24, 9, ' ', 0, 7);
	framebuffer_write(24, 10, ' ', 0, 7);
	framebuffer_write(24, 11, ' ', 0, 9);
	framebuffer_write(24, 12, ' ', 0, 9);
	framebuffer_write(24, 13, ' ', 0, 11);
	framebuffer_write(24, 14, ' ', 0, 11);
	framebuffer_write(24, 15, ' ', 0, 15);
	framebuffer_write(24, 16, ' ', 0, 15);
	framebuffer_write(24, 17, ' ', 0, 15);
	framebuffer_write(24, 18, ' ', 0, 15);
	framebuffer_write(24, 19, ' ', 0, 3);
	framebuffer_write(24, 20, ' ', 0, 3);
	framebuffer_write(24, 21, ' ', 0, 3);
	framebuffer_write(24, 22, ' ', 0, 3);
	framebuffer_write(24, 25, ' ', 0, 14);
	framebuffer_write(24, 26, ' ', 0, 14);
	framebuffer_write(24, 27, ' ', 0, 9);
	framebuffer_write(24, 28, ' ', 0, 9);
	framebuffer_write(24, 29, ' ', 0, 9);
	framebuffer_write(24, 30, ' ', 0, 9);

    // Layer 2
    framebuffer_write(23, 5, ' ', 0, 9);
    framebuffer_write(23, 6, ' ', 0, 9);
    framebuffer_write(23, 7, ' ', 0, 9);
    framebuffer_write(23, 8, ' ', 0, 9);
    framebuffer_write(23, 11, ' ', 0, 9);
    framebuffer_write(23, 12, ' ', 0, 9);
    framebuffer_write(23, 13, ' ', 0, 11);
    framebuffer_write(23, 14, ' ', 0, 11);
    framebuffer_write(23, 15, ' ', 0, 15);
    framebuffer_write(23, 16, ' ', 0, 15);
    framebuffer_write(23, 17, ' ', 0, 8);
    framebuffer_write(23, 18, ' ', 0, 8);
    framebuffer_write(23, 19, ' ', 0, 8);
    framebuffer_write(23, 20, ' ', 0, 8);
    framebuffer_write(23, 21, ' ', 0, 11);
    framebuffer_write(23, 22, ' ', 0, 11);
    framebuffer_write(23, 25, ' ', 0, 9);
    framebuffer_write(23, 26, ' ', 0, 9);
    framebuffer_write(23, 27, ' ', 0, 9);
    framebuffer_write(23, 28, ' ', 0, 9);

    // Layer 3
    framebuffer_write(22, 5, ' ', 0, 9);
    framebuffer_write(22, 6, ' ', 0, 9);
    framebuffer_write(22, 7, ' ', 0, 9);
    framebuffer_write(22, 8, ' ', 0, 9);
    framebuffer_write(22, 9, ' ', 0, 9);
    framebuffer_write(22, 10, ' ', 0, 9);
    framebuffer_write(22, 11, ' ', 0, 11);
    framebuffer_write(22, 12, ' ', 0, 11);
    framebuffer_write(22, 13, ' ', 0, 9);
    framebuffer_write(22, 14, ' ', 0, 9);
    framebuffer_write(22, 15, ' ', 0, 9);
    framebuffer_write(22, 16, ' ', 0, 9);
    framebuffer_write(22, 17, ' ', 0, 8);
    framebuffer_write(22, 18, ' ', 0, 8);
    framebuffer_write(22, 19, ' ', 0, 8);
    framebuffer_write(22, 20, ' ', 0, 8);
    framebuffer_write(22, 21, ' ', 0, 11);
    framebuffer_write(22, 22, ' ', 0, 11);
    framebuffer_write(22, 23, ' ', 0, 9);
    framebuffer_write(22, 24, ' ', 0, 9);
    framebuffer_write(22, 25, ' ', 0, 9);
    framebuffer_write(22, 26, ' ', 0, 9);

    // Layer 4
    framebuffer_write(21, 7, ' ', 0, 9);
    framebuffer_write(21, 8, ' ', 0, 9);
    framebuffer_write(21, 9, ' ', 0, 9);
    framebuffer_write(21, 10, ' ', 0, 9);
    framebuffer_write(21, 11, ' ', 0, 11);
    framebuffer_write(21, 12, ' ', 0, 11);
    framebuffer_write(21, 13, ' ', 0, 9);
    framebuffer_write(21, 14, ' ', 0, 9);
    framebuffer_write(21, 15, ' ', 0, 8);
    framebuffer_write(21, 16, ' ', 0, 8);
    framebuffer_write(21, 17, ' ', 0, 3);
    framebuffer_write(21, 18, ' ', 0, 3);
    framebuffer_write(21, 19, ' ', 0, 9);
    framebuffer_write(21, 20, ' ', 0, 9);
    framebuffer_write(21, 21, ' ', 0, 9);
    framebuffer_write(21, 22, ' ', 0, 9);
    framebuffer_write(21, 23, ' ', 0, 11);
    framebuffer_write(21, 24, ' ', 0, 11);
    framebuffer_write(21, 25, ' ', 0, 9);
    framebuffer_write(21, 26, ' ', 0, 9);

    // Layer 5
	framebuffer_write(20, 9, ' ', 0, 9);
	framebuffer_write(20, 10, ' ', 0, 9);
	framebuffer_write(20, 11, ' ', 0, 9);
	framebuffer_write(20, 12, ' ', 0, 9);
	framebuffer_write(20, 13, ' ', 0, 8);
	framebuffer_write(20, 14, ' ', 0, 8);
	framebuffer_write(20, 15, ' ', 0, 8);
	framebuffer_write(20, 16, ' ', 0, 8);
	framebuffer_write(20, 17, ' ', 0, 8);
	framebuffer_write(20, 18, ' ', 0, 8);
	framebuffer_write(20, 19, ' ', 0, 9);
	framebuffer_write(20, 20, ' ', 0, 9);
	framebuffer_write(20, 21, ' ', 0, 7);
	framebuffer_write(20, 22, ' ', 0, 7);
	framebuffer_write(20, 23, ' ', 0, 7);
	framebuffer_write(20, 24, ' ', 0, 7);

    // Layer 6
    framebuffer_write(19, 3, ' ', 0, 15);
    framebuffer_write(19, 4, ' ', 0, 15);
    framebuffer_write(19, 5, ' ', 0, 15);
    framebuffer_write(19, 6, ' ', 0, 15);
    framebuffer_write(19, 9, ' ', 0, 7);
    framebuffer_write(19, 10, ' ', 0, 7);
    framebuffer_write(19, 13, ' ', 0, 8);
    framebuffer_write(19, 14, ' ', 0, 8);
    framebuffer_write(19, 15, ' ', 0, 15);
    framebuffer_write(19, 16, ' ', 0, 15);
    framebuffer_write(19, 21, ' ', 0, 7);
    framebuffer_write(19, 22, ' ', 0, 7);
    framebuffer_write(19, 25, ' ', 0, 15);
    framebuffer_write(19, 26, ' ', 0, 15);
    framebuffer_write(19, 27, ' ', 0, 15);
    framebuffer_write(19, 28, ' ', 0, 15);

    //Layer 7
    framebuffer_write(18, 1, ' ', 0, 7);
    framebuffer_write(18, 2, ' ', 0, 7);
    framebuffer_write(18, 3, ' ', 0, 7);
    framebuffer_write(18, 4, ' ', 0, 7);
    framebuffer_write(18, 5, ' ', 0, 15);
    framebuffer_write(18, 6, ' ', 0, 15);
    framebuffer_write(18, 7, ' ', 0, 7);
    framebuffer_write(18, 8, ' ', 0, 7);
    framebuffer_write(18, 9, ' ', 0, 15);
    framebuffer_write(18, 10, ' ', 0, 15);
    framebuffer_write(18, 11, ' ', 0, 15);
    framebuffer_write(18, 12, ' ', 0, 15);
    framebuffer_write(18, 13, ' ', 0, 15);
    framebuffer_write(18, 14, ' ', 0, 15);
    framebuffer_write(18, 15, ' ', 0, 15);
    framebuffer_write(18, 16, ' ', 0, 15);
    framebuffer_write(18, 17, ' ', 0, 15);
    framebuffer_write(18, 18, ' ', 0, 15);
    framebuffer_write(18, 19, ' ', 0, 15);
    framebuffer_write(18, 20, ' ', 0, 15);
    framebuffer_write(18, 21, ' ', 0, 15);
    framebuffer_write(18, 22, ' ', 0, 15);
    framebuffer_write(18, 23, ' ', 0, 15);
    framebuffer_write(18, 24, ' ', 0, 15);
    framebuffer_write(18, 25, ' ', 0, 7);
    framebuffer_write(18, 26, ' ', 0, 7);
    framebuffer_write(18, 27, ' ', 0, 7);
    framebuffer_write(18, 28, ' ', 0, 7);
    framebuffer_write(18, 29, ' ', 0, 7);
    framebuffer_write(18, 30, ' ', 0, 7);

    // Layer 8
    framebuffer_write(17, 3, ' ', 0, 11);
    framebuffer_write(17, 4, ' ', 0, 11);
    framebuffer_write(17, 5, ' ', 0, 7);
    framebuffer_write(17, 6, ' ', 0, 7);
    framebuffer_write(17, 7, ' ', 0, 7);
    framebuffer_write(17, 8, ' ', 0, 7);
    framebuffer_write(17, 9, ' ', 0, 15);
    framebuffer_write(17, 10, ' ', 0, 15);
    framebuffer_write(17, 11, ' ', 0, 15);
    framebuffer_write(17, 12, ' ', 0, 15);
    framebuffer_write(17, 13, ' ', 0, 11);
    framebuffer_write(17, 14, ' ', 0, 11);
    framebuffer_write(17, 15, ' ', 0, 15);
    framebuffer_write(17, 16, ' ', 0, 15);
    framebuffer_write(17, 17, ' ', 0, 15);
    framebuffer_write(17, 18, ' ', 0, 15);
    framebuffer_write(17, 19, ' ', 0, 7);
    framebuffer_write(17, 20, ' ', 0, 7);
    framebuffer_write(17, 21, ' ', 0, 9);
    framebuffer_write(17, 22, ' ', 0, 9);
    framebuffer_write(17, 23, ' ', 0, 15);
    framebuffer_write(17, 24, ' ', 0, 15);
    framebuffer_write(17, 25, ' ', 0, 15);
    framebuffer_write(17, 26, ' ', 0, 15);
    framebuffer_write(17, 27, ' ', 0, 11);
    framebuffer_write(17, 28, ' ', 0, 11);

    // Layer 9
    framebuffer_write(16, 1, ' ', 0, 7);
    framebuffer_write(16, 2, ' ', 0, 7);
    framebuffer_write(16, 3, ' ', 0, 11);
    framebuffer_write(16, 4, ' ', 0, 11);
    framebuffer_write(16, 5, ' ', 0, 11);
    framebuffer_write(16, 6, ' ', 0, 11);
    framebuffer_write(16, 7, ' ', 0, 7);
    framebuffer_write(16, 8, ' ', 0, 7);
    framebuffer_write(16, 9, ' ', 0, 8);
    framebuffer_write(16, 10, ' ', 0, 8);
    framebuffer_write(16, 11, ' ', 0, 15);
    framebuffer_write(16, 12, ' ', 0, 15);
    framebuffer_write(16, 13, ' ', 0, 9);
    framebuffer_write(16, 14, ' ', 0, 9);
    framebuffer_write(16, 15, ' ', 0, 15);
    framebuffer_write(16, 16, ' ', 0, 15);
    framebuffer_write(16, 17, ' ', 0, 7);
    framebuffer_write(16, 18, ' ', 0, 7);
    framebuffer_write(16, 19, ' ', 0, 7);
    framebuffer_write(16, 20, ' ', 0, 7);
    framebuffer_write(16, 21, ' ', 0, 1);
    framebuffer_write(16, 22, ' ', 0, 1);
    framebuffer_write(16, 23, ' ', 0, 15);
    framebuffer_write(16, 24, ' ', 0, 15);
    framebuffer_write(16, 25, ' ', 0, 7);
    framebuffer_write(16, 26, ' ', 0, 7);
    framebuffer_write(16, 27, ' ', 0, 7);
    framebuffer_write(16, 28, ' ', 0, 7);

    // Layer 10
    framebuffer_write(15, 1, ' ', 0, 7);
    framebuffer_write(15, 2, ' ', 0, 7);
    framebuffer_write(15, 3, ' ', 0, 7);
    framebuffer_write(15, 4, ' ', 0, 7);
    framebuffer_write(15, 5, ' ', 0, 7);
    framebuffer_write(15, 6, ' ', 0, 7);
    framebuffer_write(15, 7, ' ', 0, 7);
    framebuffer_write(15, 8, ' ', 0, 7);
    framebuffer_write(15, 9, ' ', 0, 7);
    framebuffer_write(15, 10, ' ', 0, 7);
    framebuffer_write(15, 11, ' ', 0, 8);
    framebuffer_write(15, 12, ' ', 0, 8);
    framebuffer_write(15, 13, ' ', 0, 8);
    framebuffer_write(15, 14, ' ', 0, 8);
    framebuffer_write(15, 15, ' ', 0, 8);
    framebuffer_write(15, 16, ' ', 0, 8);
    framebuffer_write(15, 17, ' ', 0, 7);
    framebuffer_write(15, 18, ' ', 0, 7);
    framebuffer_write(15, 19, ' ', 0, 7);
    framebuffer_write(15, 20, ' ', 0, 7);
    framebuffer_write(15, 21, ' ', 0, 8);
    framebuffer_write(15, 22, ' ', 0, 8);
    framebuffer_write(15, 23, ' ', 0, 8);
    framebuffer_write(15, 24, ' ', 0, 8);
    framebuffer_write(15, 25, ' ', 0, 7);
    framebuffer_write(15, 26, ' ', 0, 7);
    framebuffer_write(15, 27, ' ', 0, 7);
    framebuffer_write(15, 28, ' ', 0, 7);
    framebuffer_write(15, 31, ' ', 0, 15);
    framebuffer_write(15, 32, ' ', 0, 15);

    // Layer 11
    framebuffer_write(14, 1, ' ', 0, 7);
    framebuffer_write(14, 2, ' ', 0, 7);
    framebuffer_write(14, 3, ' ', 0, 7);
    framebuffer_write(14, 4, ' ', 0, 7);
    framebuffer_write(14, 5, ' ', 0, 15);
    framebuffer_write(14, 6, ' ', 0, 15);
    framebuffer_write(14, 7, ' ', 0, 15);
    framebuffer_write(14, 8, ' ', 0, 15);
    framebuffer_write(14, 9, ' ', 0, 7);
    framebuffer_write(14, 10, ' ', 0, 7);
    framebuffer_write(14, 11, ' ', 0, 7);
    framebuffer_write(14, 12, ' ', 0, 7);
    framebuffer_write(14, 13, ' ', 0, 15);
    framebuffer_write(14, 14, ' ', 0, 15);
    framebuffer_write(14, 15, ' ', 0, 7);
    framebuffer_write(14, 16, ' ', 0, 7);
    framebuffer_write(14, 17, ' ', 0, 7);
    framebuffer_write(14, 18, ' ', 0, 7);
    framebuffer_write(14, 19, ' ', 0, 7);
    framebuffer_write(14, 20, ' ', 0, 7);
    framebuffer_write(14, 21, ' ', 0, 7);
    framebuffer_write(14, 22, ' ', 0, 7);
    framebuffer_write(14, 23, ' ', 0, 7);
    framebuffer_write(14, 24, ' ', 0, 7);
    framebuffer_write(14, 25, ' ', 0, 7);
    framebuffer_write(14, 26, ' ', 0, 7);
    framebuffer_write(14, 27, ' ', 0, 7);
    framebuffer_write(14, 28, ' ', 0, 7);
    framebuffer_write(14, 29, ' ', 0, 7);
    framebuffer_write(14, 30, ' ', 0, 7);
    framebuffer_write(14, 31, ' ', 0, 15);
    framebuffer_write(14, 32, ' ', 0, 15);

    // Layer 12
    framebuffer_write(13, 3, ' ', 0, 7);
    framebuffer_write(13, 4, ' ', 0, 7);
    framebuffer_write(13, 5, ' ', 0, 7);
    framebuffer_write(13, 6, ' ', 0, 7);
    framebuffer_write(13, 7, ' ', 0, 15);
    framebuffer_write(13, 8, ' ', 0, 15);
    framebuffer_write(13, 9, ' ', 0, 7);
    framebuffer_write(13, 10, ' ', 0, 7);
    framebuffer_write(13, 11, ' ', 0, 7);
    framebuffer_write(13, 12, ' ', 0, 7);
    framebuffer_write(13, 13, ' ', 0, 7);
    framebuffer_write(13, 14, ' ', 0, 7);
    framebuffer_write(13, 15, ' ', 0, 7);
    framebuffer_write(13, 16, ' ', 0, 7);
    framebuffer_write(13, 17, ' ', 0, 7);
    framebuffer_write(13, 18, ' ', 0, 7);
    framebuffer_write(13, 19, ' ', 0, 7);
    framebuffer_write(13, 20, ' ', 0, 7);
    framebuffer_write(13, 21, ' ', 0, 11);
    framebuffer_write(13, 22, ' ', 0, 11);
    framebuffer_write(13, 23, ' ', 0, 11);
    framebuffer_write(13, 24, ' ', 0, 11);
    framebuffer_write(13, 25, ' ', 0, 7);
    framebuffer_write(13, 26, ' ', 0, 7);
    framebuffer_write(13, 29, ' ', 0, 15);
    framebuffer_write(13, 30, ' ', 0, 15);

    // Layer 13
    framebuffer_write(12, 5, ' ', 0, 7);
    framebuffer_write(12, 6, ' ', 0, 7);
    framebuffer_write(12, 7, ' ', 0, 7);
    framebuffer_write(12, 8, ' ', 0, 7);
    framebuffer_write(12, 9, ' ', 0, 15);
    framebuffer_write(12, 10, ' ', 0, 15);
    framebuffer_write(12, 11, ' ', 0, 7);
    framebuffer_write(12, 12, ' ', 0, 7);
    framebuffer_write(12, 13, ' ', 0, 7);
    framebuffer_write(12, 14, ' ', 0, 7);
    framebuffer_write(12, 15, ' ', 0, 7);
    framebuffer_write(12, 16, ' ', 0, 7);
    framebuffer_write(12, 17, ' ', 0, 7);
    framebuffer_write(12, 18, ' ', 0, 7);
    framebuffer_write(12, 19, ' ', 0, 11);
    framebuffer_write(12, 20, ' ', 0, 11);
    framebuffer_write(12, 21, ' ', 0, 7);
    framebuffer_write(12, 22, ' ', 0, 7);
    framebuffer_write(12, 23, ' ', 0, 7);
    framebuffer_write(12, 24, ' ', 0, 7);
    framebuffer_write(12, 25, ' ', 0, 7);
    framebuffer_write(12, 26, ' ', 0, 7);
    framebuffer_write(12, 27, ' ', 0, 9);
    framebuffer_write(12, 28, ' ', 0, 9);
    framebuffer_write(12, 29, ' ', 0, 15);
    framebuffer_write(12, 30, ' ', 0, 15);

    // Layer 14
    framebuffer_write(11, 5, ' ', 0, 7);
    framebuffer_write(11, 6, ' ', 0, 7);
    framebuffer_write(11, 7, ' ', 0, 7);
    framebuffer_write(11, 8, ' ', 0, 7);
    framebuffer_write(11, 9, ' ', 0, 15);
    framebuffer_write(11, 10, ' ', 0, 15);
    framebuffer_write(11, 11, ' ', 0, 7);
    framebuffer_write(11, 12, ' ', 0, 7);
    framebuffer_write(11, 13, ' ', 0, 7);
    framebuffer_write(11, 14, ' ', 0, 7);
    framebuffer_write(11, 15, ' ', 0, 7);
    framebuffer_write(11, 16, ' ', 0, 7);
    framebuffer_write(11, 17, ' ', 0, 11);
    framebuffer_write(11, 18, ' ', 0, 11);
    framebuffer_write(11, 19, ' ', 0, 7);
    framebuffer_write(11, 20, ' ', 0, 7);
    framebuffer_write(11, 21, ' ', 0, 7);
    framebuffer_write(11, 22, ' ', 0, 7);
    framebuffer_write(11, 23, ' ', 0, 7);
    framebuffer_write(11, 24, ' ', 0, 7);
    framebuffer_write(11, 25, ' ', 0, 9);
    framebuffer_write(11, 26, ' ', 0, 9);
    framebuffer_write(11, 27, ' ', 0, 15);
    framebuffer_write(11, 28, ' ', 0, 15);

    // Layer 15
    framebuffer_write(10, 7, ' ', 0, 7);
    framebuffer_write(10, 8, ' ', 0, 7);
    framebuffer_write(10, 9, ' ', 0, 7);
    framebuffer_write(10, 10, ' ', 0, 7);
    framebuffer_write(10, 11, ' ', 0, 15);
    framebuffer_write(10, 12, ' ', 0, 15);
    framebuffer_write(10, 13, ' ', 0, 7);
    framebuffer_write(10, 14, ' ', 0, 7);
    framebuffer_write(10, 15, ' ', 0, 7);
    framebuffer_write(10, 16, ' ', 0, 7);
    framebuffer_write(10, 17, ' ', 0, 7);
    framebuffer_write(10, 18, ' ', 0, 7);
    framebuffer_write(10, 19, ' ', 0, 7);
    framebuffer_write(10, 20, ' ', 0, 7);
    framebuffer_write(10, 21, ' ', 0, 9);
    framebuffer_write(10, 22, ' ', 0, 9);
    framebuffer_write(10, 23, ' ', 0, 9);
    framebuffer_write(10, 24, ' ', 0, 9);
    framebuffer_write(10, 25, ' ', 0, 9);
    framebuffer_write(10, 26, ' ', 0, 9);

    // Layer 16
    framebuffer_write(9 , 9, ' ', 0, 11);
    framebuffer_write(9 , 10, ' ', 0, 11);
    framebuffer_write(9 , 11, ' ', 0, 7);
    framebuffer_write(9 , 12, ' ', 0, 7);
    framebuffer_write(9 , 13, ' ', 0, 7);
    framebuffer_write(9 , 14, ' ', 0, 7);
    framebuffer_write(9 , 15, ' ', 0, 7);
    framebuffer_write(9 , 16, ' ', 0, 7);
    framebuffer_write(9 , 17, ' ', 0, 7);
    framebuffer_write(9 , 18, ' ', 0, 7);
    framebuffer_write(9 , 19, ' ', 0, 9);
    framebuffer_write(9 , 20, ' ', 0, 9);
    framebuffer_write(9 , 21, ' ', 0, 9);
    framebuffer_write(9 , 22, ' ', 0, 9);
    framebuffer_write(9 , 23, ' ', 0, 3);
    framebuffer_write(9 , 24, ' ', 0, 3);
    framebuffer_write(9 , 25, ' ', 0, 3);
    framebuffer_write(9 , 26, ' ', 0, 3);
    framebuffer_write(9 , 27, ' ', 0, 9);
    framebuffer_write(9 , 28, ' ', 0, 9);

    // Layer 17
    framebuffer_write(8, 5, ' ', 0, 11);
    framebuffer_write(8, 6, ' ', 0, 11);
    framebuffer_write(8, 7, ' ', 0, 11);
    framebuffer_write(8, 8, ' ', 0, 11);
    framebuffer_write(8, 11, ' ', 0, 1);
    framebuffer_write(8, 12, ' ', 0, 1);
    framebuffer_write(8, 13, ' ', 0, 1);
    framebuffer_write(8, 14, ' ', 0, 1);
    framebuffer_write(8, 15, ' ', 0, 1);
    framebuffer_write(8, 16, ' ', 0, 1);
    framebuffer_write(8, 17, ' ', 0, 9);
    framebuffer_write(8, 18, ' ', 0, 9);
    framebuffer_write(8, 19, ' ', 0, 3);
    framebuffer_write(8, 20, ' ', 0, 3);
    framebuffer_write(8, 21, ' ', 0, 3);
    framebuffer_write(8, 22, ' ', 0, 3);
    framebuffer_write(8, 23, ' ', 0, 9);
    framebuffer_write(8, 24, ' ', 0, 9);
    framebuffer_write(8, 25, ' ', 0, 9);
    framebuffer_write(8, 26, ' ', 0, 9);
    framebuffer_write(8, 27, ' ', 0, 14);
    framebuffer_write(8, 28, ' ', 0, 14);
    framebuffer_write(8, 29, ' ', 0, 9);
    framebuffer_write(8, 30, ' ', 0, 9);

    // Layer 18
    framebuffer_write(7, 5, ' ', 0, 11);
    framebuffer_write(7, 6, ' ', 0, 11);
    framebuffer_write(7, 11, ' ', 0, 1);
    framebuffer_write(7, 12, ' ', 0, 1);
    framebuffer_write(7, 13, ' ', 0, 1);
    framebuffer_write(7, 14, ' ', 0, 1);
    framebuffer_write(7, 15, ' ', 0, 9);
    framebuffer_write(7, 16, ' ', 0, 9);
    framebuffer_write(7, 17, ' ', 0, 3);
    framebuffer_write(7, 18, ' ', 0, 3);
    framebuffer_write(7, 19, ' ', 0, 9);
    framebuffer_write(7, 20, ' ', 0, 9);
    framebuffer_write(7, 21, ' ', 0, 9);
    framebuffer_write(7, 22, ' ', 0, 9);
    framebuffer_write(7, 23, ' ', 0, 9);
    framebuffer_write(7, 24, ' ', 0, 9);
    framebuffer_write(7, 25, ' ', 0, 9);
    framebuffer_write(7, 26, ' ', 0, 9);
    framebuffer_write(7, 27, ' ', 0, 9);
    framebuffer_write(7, 28, ' ', 0, 9);
    framebuffer_write(7, 29, ' ', 0, 14);
    framebuffer_write(7, 30, ' ', 0, 14);

    // Layer 19
    framebuffer_write(6, 3, ' ', 0, 11);
    framebuffer_write(6, 4, ' ', 0, 11);
    framebuffer_write(6, 11, ' ', 0, 1);
    framebuffer_write(6, 12, ' ', 0, 1);
    framebuffer_write(6, 13, ' ', 0, 9);
    framebuffer_write(6, 14, ' ', 0, 9);
    framebuffer_write(6, 17, ' ', 0, 9);
    framebuffer_write(6, 18, ' ', 0, 9);
    framebuffer_write(6, 19, ' ', 0, 9);
    framebuffer_write(6, 20, ' ', 0, 9);
    framebuffer_write(6, 21, ' ', 0, 14);
    framebuffer_write(6, 22, ' ', 0, 14);
    framebuffer_write(6, 23, ' ', 0, 9);
    framebuffer_write(6, 24, ' ', 0, 9);
    framebuffer_write(6, 25, ' ', 0, 9);
    framebuffer_write(6, 26, ' ', 0, 9);
    framebuffer_write(6, 27, ' ', 0, 14);
    framebuffer_write(6, 28, ' ', 0, 14);
    framebuffer_write(6, 31, ' ', 0, 14);
    framebuffer_write(6, 32, ' ', 0, 14);

    // Layer 20
    framebuffer_write(5, 3, ' ', 0, 7);
    framebuffer_write(5, 4, ' ', 0, 7);
    framebuffer_write(5, 5, ' ', 0, 11);
    framebuffer_write(5, 6, ' ', 0, 11);
    framebuffer_write(5, 19, ' ', 0, 9);
    framebuffer_write(5, 20, ' ', 0, 9);
    framebuffer_write(5, 21, ' ', 0, 9);
    framebuffer_write(5, 22, ' ', 0, 9);
    framebuffer_write(5, 23, ' ', 0, 14);
    framebuffer_write(5, 24, ' ', 0, 14);

    // Layer 21
    framebuffer_write(4, 5, ' ', 0, 7);
    framebuffer_write(4, 6, ' ', 0, 7);
    framebuffer_write(4, 7, ' ', 0, 7);
    framebuffer_write(4, 8, ' ', 0, 7);
    framebuffer_write(4, 19, ' ', 0, 14);
    framebuffer_write(4, 20, ' ', 0, 14);
    framebuffer_write(4, 21, ' ', 0, 9);
    framebuffer_write(4, 22, ' ', 0, 9);
    framebuffer_write(4, 25, ' ', 0, 14);
    framebuffer_write(4, 26, ' ', 0, 14);

    // Layer 22
    framebuffer_write(3, 19, ' ', 0, 14);
    framebuffer_write(3, 20, ' ', 0, 14);
}