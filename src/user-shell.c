#include <stdint.h>
#include "header/filesystem/fat32.h"

#define BLACK 0x00
#define DARK_BLUE 0x01
#define DARK_GREEN 0x2
#define DARK_AQUA 0x3
#define DARK_RED 0x4
#define DARK_PURPLE 0x5
#define GOLD 0x6
#define GRAY 0x7
#define DARK_GRAY 0x8
#define BLUE 0x09
#define GREEN 0x0A
#define AQUA 0x0B
#define RED 0x0C
#define LIGHT_PURPLE 0x0D
#define YELLOW 0x0E
#define WHITE 0x0F

#define MAX_COMMAND_LENGTH 256

void syscall_user(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
    //        so it need to be the last one to mov
    __asm__ volatile("int $0x30");
}

int strlen(char* c){
    int i = 0;
    while(c[i] != '\0'){
        i++;
    }
    return i;
}

bool streq(const char* str1, const char* str2){
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return (*str1 == '\0' && *str2 == '\0');
}

void putc_user(char c, uint8_t color){
    syscall_user(5, (uint32_t) c, color, 0);
}

void puts_user(char* c, uint8_t color){
    syscall_user(6, (uint32_t) c, strlen(c), color);
}

char getc(){
    char buf;
    syscall_user(4, (uint32_t) &buf, 0, 0);
    return buf;
}

void make_furina(){
    syscall_user(8, 0, 0, 0);
}

void clear(){
    syscall_user(9, 0, 0, 0);
}

// void ls(){
//     struct FAT32DirectoryTable current_table;
//     struct FAT32DriverRequest request = {
//         .buf = &current_table,
//         .ext = "\0\0\0",
//         .buffer_size = 0,
//     };
// }

int main(void) {
    struct ClusterBuffer      cl[2]   = {0};
    struct FAT32DriverRequest request = {
        .buf                   = &cl,
        .name                  = "shell",
        .ext                   = "\0\0\0",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = CLUSTER_SIZE,
    };
    int32_t retcode;
    syscall_user(0, (uint32_t) &request, (uint32_t) &retcode, 0);
    // if (retcode == 0){
    //     syscall_user(6, (uint32_t) "owo\n", 4, 0xF);
    // }
    char buf = '\0';
    syscall_user(7, 0, 0, 0);
    puts_user("FurinaOS@2024$ ", BLUE);

    char command[MAX_COMMAND_LENGTH];
    int command_len = 0;

    while (true) {
        buf = getc();
        if(buf != '\0'){
            putc_user(buf, WHITE);

            if(buf == '\n'){
                command[command_len] = '\0';
                if(streq(command, "makefurina")){
                    make_furina();
                }
                else if(streq(command, "clear")){
                    clear();
                }
                command_len = 0;
                puts_user("FurinaOS@2024$ ", BLUE);
            }
            else if(buf == '\b'){
                command[command_len] = '\0';
                command_len--;
            }
            else{
                command[command_len] = buf;
                command_len++;
            }
        
        }
    }

    return 0;
}
