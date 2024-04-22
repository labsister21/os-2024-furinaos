#include <stdint.h>
#include "header/filesystem/fat32.h"
#include "header/driver/keyboard.h"
void syscall_user(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
    //        so it need to be the last one to mov
    __asm__ volatile("int $0x30");
}

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
    if (retcode == 0)
        syscall_user(6, (uint32_t) "owo\n", 4, 0xF);

    char buf = '\0';
    syscall_user(7, 0, 0, 0);
    while (true) {
        syscall_user(4, (uint32_t) &buf, 0, 0);
        if(buf != '\0'){
            syscall_user(5, (uint32_t) &buf, 0xF, 0);
        }
        buf = '\0';
    }

    return 0;
}
