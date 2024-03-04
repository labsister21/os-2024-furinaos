#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../header/stdlib/string.h"
#include "../header/filesystem/fat32.h"

const uint8_t fs_signature[BLOCK_SIZE] = {
    'C', 'o', 'u', 'r', 's', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
    'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
    'L', 'a', 'b', ' ', 'S', 'i', 's', 't', 'e', 'r', ' ', 'I', 'T', 'B', ' ',  ' ',
    'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
    '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '4', '\n',
    [BLOCK_SIZE-2] = 'O',
    [BLOCK_SIZE-1] = 'k',
};

struct FAT32DriverState fat32_driver_state;

uint32_t cluster_to_lba(uint32_t cluster){
    return CLUSTER_BLOCK_COUNT * cluster;
}

void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster){
    memset(dir_table, 0, sizeof(struct FAT32DirectoryTable));
    uint8_t i;
    for(i = 0; i < 8; i++){
        dir_table->table[0].name[i] = '\0'; 
        dir_table->table[1].name[i] = '\0';
    }
    for(i = 0; i < 8; i++){
        dir_table->table[0].name[i] = name[i]; 
    }
    dir_table->table[0].attribute = ATTR_SUBDIRECTORY;
    dir_table->table[0].user_attribute = UATTR_NOT_EMPTY;
    dir_table->table[0].cluster_low = parent_dir_cluster;

    for(i = 0; i < 2; i++){
        dir_table->table[1].name[i] = '.'; 
    }

    dir_table->table[1].attribute = ATTR_SUBDIRECTORY;
    dir_table->table[1].attribute = UATTR_NOT_EMPTY;
    dir_table->table[1].cluster_low = parent_dir_cluster;
}

bool is_empty_storage(void){
    struct ClusterBuffer boot_sector;
    read_blocks(boot_sector.buf, BOOT_SECTOR, 1);
    return memcmp(boot_sector.buf, fs_signature, BLOCK_SIZE) != 0;
}

void create_fat32(void){
    write_blocks(fs_signature, BOOT_SECTOR, 1);

    struct FAT32FileAllocationTable fat_table;
    memset(&fat_table, 0, sizeof(fat_table));
    fat_table.cluster_map[0] = CLUSTER_0_VALUE;
    fat_table.cluster_map[1] = CLUSTER_1_VALUE;

    struct FAT32DirectoryTable root;
    init_directory_table(&root, "ROOT", ROOT_CLUSTER_NUMBER);

    write_clusters(&fat_table, FAT_CLUSTER_NUMBER, 1);
    write_clusters(&root, ROOT_CLUSTER_NUMBER, 1);

    fat32_driver_state.fat_table = fat_table;
    fat32_driver_state.dir_table_buf = root;
}

void initialize_filesystem_fat32(void){
    if(is_empty_storage()){
        create_fat32();
    }
    else{
        read_clusters(&fat32_driver_state.fat_table, FAT_CLUSTER_NUMBER, 1);
    }
}

void write_clusters(const void *ptr, uint32_t cluster_number, uint8_t cluster_count){
    write_blocks(ptr, cluster_to_lba(cluster_number), cluster_count*CLUSTER_BLOCK_COUNT);
}

void read_clusters(void *ptr, uint32_t cluster_number, uint8_t cluster_count){
    read_blocks(ptr, cluster_to_lba(cluster_number), cluster_count*CLUSTER_BLOCK_COUNT);
}

int8_t read_directory(struct FAT32DriverRequest request){
    struct FAT32DirectoryTable dir_table = fat32_driver_state.dir_table_buf ;
    uint32_t counter = 3; // Check from Cluster 3 after root dir 
    uint8_t i;
    uint32_t pos = 0; 

    // Find cluster position on the table  
    while(counter != FAT32_FAT_END_OF_FILE){
        bool isSameName = true;
        for(i = 0; i < 8; i++){
            if(dir_table.table[counter].name[i] != request.name[i]){
                isSameName = false;
            }
        }
        if(isSameName) {
            pos = counter; 
        }
        counter++;
    }

    // not found 
    if(!pos){
        return 2;
    }

    // not a directory 
    if(dir_table.table[counter].attribute != ATTR_SUBDIRECTORY){
        return 1;
    }

    // Success
    if (pos) {
        read_clusters(request.buf, pos, 1);
        return 0;
    }

    return -1;
}

int8_t read(struct FAT32DriverRequest request){
    struct FAT32DirectoryTable dir_table = fat32_driver_state.dir_table_buf ;
    uint32_t counter = request.parent_cluster_number;
    uint8_t i;
    uint32_t pos = 0; 

    // Find position of the cluster
    while(counter != FAT32_FAT_END_OF_FILE && pos != counter){
        bool isSameName = true;
        for(i = 0; i < 8; i++){
            if(dir_table.table[counter].name[i] != request.name[i]){
                isSameName = false;
            }
        }

        if(isSameName) {
            for(i =0 ; i < 3; i++){
                if(dir_table.table[counter].ext[i] != request.ext[i]){
                    isSameName = false; 
                }
            } 
        }

        if(isSameName){
            pos = counter; 
        }

        else {
            counter++;
        }
    }

    // Not Found 
    if(!pos){
        return 2;
    }

    // Not a File (a directory)
    else if(dir_table.table[pos].attribute == ATTR_SUBDIRECTORY){
        return 1;
    }

    // Success
    else if (pos) {
        read_clusters(request.buf, pos, 1);
        return 0;
    }

    else if(dir_table.table[pos].filesize > request.buffer_size){
        return -1;
    }
    else {
        return -1; 
    }
}
