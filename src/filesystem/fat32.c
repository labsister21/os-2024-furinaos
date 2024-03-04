#include <stdint.h>
#include <stdbool.h>
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

struct FAT32DirectoryEntry *dir_table_seq_search(char *name, char *ext, uint32_t parent_dir_cluster) {
    while (parent_dir_cluster != FAT32_FAT_END_OF_FILE) {
        read_clusters(&fat32_driver_state.dir_table_buf.table, parent_dir_cluster, 1);

        // Iterate through the directory table
        for (int32_t i = 0; i < (int32_t)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++) {
            // If we find a directory entry with matching name and ext, return its index
            if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, name, 8) == 0 &&
                memcmp(fat32_driver_state.dir_table_buf.table[i].ext, ext, 3) == 0 &&
                fat32_driver_state.dir_table_buf.table[i].user_attribute == UATTR_NOT_EMPTY) {
                return fat32_driver_state.dir_table_buf.table + i;
            }
        }

    } 

    return 0;
}

int8_t read_directory(struct FAT32DriverRequest request){
    struct FAT32DirectoryEntry *entry_p = dir_table_seq_search(request.name, request.ext, request.parent_cluster_number);
    struct FAT32DirectoryEntry dir_entry = *entry_p;

    if(entry_p == 0){
        return 2; 
    }
    else if(dir_entry.attribute != ATTR_SUBDIRECTORY){
        return 1; 
    }

    uint32_t cluster_number = (dir_entry.cluster_high << 16) | dir_entry.cluster_low; 

    if(cluster_number){
        read_clusters(request.buf, cluster_number, 1);
        return 0;
    }

    else{
        return -1; 
    }
}

int8_t read(struct FAT32DriverRequest request){
    struct FAT32DirectoryEntry *entry_p = dir_table_seq_search(request.name, request.ext, request.parent_cluster_number);
    struct FAT32DirectoryEntry dir_entry = *entry_p;

    if(entry_p == 0){
        return 2; 
    }
    else if(dir_entry.attribute == ATTR_SUBDIRECTORY){
        return 1; 
    }

    else if( dir_entry.filesize > request.buffer_size){
        return -1; 
    }

    uint32_t cluster_number = (dir_entry.cluster_high << 16) | dir_entry.cluster_low; 

    if(cluster_number){
        while(cluster_number != FAT32_FAT_END_OF_FILE){
            read_clusters(request.buf, cluster_number, 1);
            request.buf++; 
            cluster_number = fat32_driver_state.fat_table.cluster_map[cluster_number];
        }

        return 0;
    }

    else{
        return -1; 
    }
}

int8_t write (struct FAT32DriverRequest request){

}
