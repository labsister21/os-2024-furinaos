#include <stdint.h>
#include <stdbool.h>
#include "../header/filesystem/fat32.h"
#include "../header/driver/keyboard.h"
#include "../header/stdlib/string.h"

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

void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name,
                          uint32_t parent_dir_cluster) {
    
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
    dir_table->table[0].cluster_low = parent_dir_cluster & 0xFFFF;
    dir_table->table[0].cluster_high = (parent_dir_cluster >> 16) & 0xFFFF;
	dir_table->table[0].filesize = 0; 
}

bool is_empty_storage(void){
    struct ClusterBuffer boot_sector;
    read_blocks(boot_sector.buf, BOOT_SECTOR, 1);
    return memcmp(boot_sector.buf, fs_signature, BLOCK_SIZE) != 0;
}

void create_fat32(void){
    write_blocks(fs_signature, BOOT_SECTOR, 1);

	struct FAT32FileAllocationTable fat_table = {
        .cluster_map = {CLUSTER_0_VALUE, CLUSTER_1_VALUE, FAT32_FAT_END_OF_FILE,
                        [3 ... CLUSTER_MAP_SIZE - 1] = FAT32_FAT_EMPTY_ENTRY}};

    struct FAT32DirectoryTable root;
    init_directory_table(&root, "root\0\0\0\0", ROOT_CLUSTER_NUMBER);
    
    fat32_driver_state.fat_table = fat_table;
    write_clusters(&fat_table, FAT_CLUSTER_NUMBER, 1);
    write_clusters(&root, ROOT_CLUSTER_NUMBER, 1);

}

void initialize_filesystem_fat32(void){
    if (is_empty_storage()) {
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
        parent_dir_cluster = fat32_driver_state.fat_table.cluster_map[parent_dir_cluster];
    }
    return 0;
}

int8_t read_directory(struct FAT32DriverRequest *request){

    struct FAT32DirectoryEntry *entry_p = dir_table_seq_search(request->name, request->ext, request->parent_cluster_number);

    if(entry_p == 0){
        return 2; 
    }

    if(entry_p->attribute != ATTR_SUBDIRECTORY){
        return 1; 
    }

    uint32_t cluster_number = (entry_p->cluster_high << 16) | entry_p->cluster_low; 

    if(cluster_number){
        read_clusters(request->buf, cluster_number, 1);
        return 0;
    }

    else{
        return -1; 
    }
}

int8_t read(struct FAT32DriverRequest *request){
    struct FAT32DirectoryEntry *entry_p = dir_table_seq_search(request->name, request->ext, request->parent_cluster_number);
    if(entry_p == 0){
        return 2; 
    }
    
    if(entry_p->attribute == ATTR_SUBDIRECTORY){
        return 1; 
    }

    else if( entry_p->filesize > request->buffer_size){
        return -1; 
    }

    uint32_t cluster_number = (entry_p->cluster_high << 16) | entry_p->cluster_low; 
    if(cluster_number){
        while(cluster_number != FAT32_FAT_END_OF_FILE){
            read_clusters(request->buf, cluster_number, 1);
            request->buf++; 
            cluster_number = fat32_driver_state.fat_table.cluster_map[cluster_number];
        }

        return 0;
    }

    else{
        return -1; 
    }
}

uint32_t extend_dir_table(uint32_t dir_cluster_number) {
    if (fat32_driver_state.fat_table.cluster_map[dir_cluster_number] != FAT32_FAT_END_OF_FILE)
        return fat32_driver_state.fat_table.cluster_map[dir_cluster_number];

    uint32_t empty_cluster_number = 3;

    while (fat32_driver_state.fat_table.cluster_map[empty_cluster_number] !=
           FAT32_FAT_EMPTY_ENTRY) {
        empty_cluster_number++;
    }

    fat32_driver_state.fat_table.cluster_map[dir_cluster_number] = empty_cluster_number;
    fat32_driver_state.fat_table.cluster_map[empty_cluster_number] = FAT32_FAT_END_OF_FILE;

    struct ClusterBuffer empty_cluster_buffer = {
        .buf = {[0 ... CLUSTER_SIZE - 1] = 0} 
    };
    
    write_clusters(&empty_cluster_buffer, empty_cluster_number, 1);

    return empty_cluster_number;
}


int8_t add_entry(struct FAT32DriverRequest *request, uint32_t cluster_number) {
    read_clusters(&fat32_driver_state.dir_table_buf.table, request->parent_cluster_number, 1);

    uint32_t entry_i = 0;

    while (entry_i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry) &&
           fat32_driver_state.dir_table_buf.table[entry_i].user_attribute == UATTR_NOT_EMPTY) {
        entry_i++;
        if (entry_i == CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)) {
            request->parent_cluster_number = extend_dir_table(request->parent_cluster_number);
            entry_i = 0;
            read_clusters(&fat32_driver_state.dir_table_buf.table, request->parent_cluster_number, 1);
        }
    }

    if (cluster_number == request->parent_cluster_number)
        cluster_number++;

    for (size_t i = 0; i < 8; i++) {
        fat32_driver_state.dir_table_buf.table[entry_i].name[i] = request->name[i];
    }

    for (size_t i = 0; i < 8; i++) {
        fat32_driver_state.dir_table_buf.table[entry_i].ext[i] = request->ext[i];
    }
    fat32_driver_state.dir_table_buf.table[entry_i].user_attribute = UATTR_NOT_EMPTY;
    if (request->buffer_size == 0)
        fat32_driver_state.dir_table_buf.table[entry_i].attribute = ATTR_SUBDIRECTORY;

    fat32_driver_state.dir_table_buf.table[entry_i].cluster_high = (cluster_number >> 16) & 0xFFFF;
    fat32_driver_state.dir_table_buf.table[entry_i].cluster_low = cluster_number & 0xFFFF;
    fat32_driver_state.dir_table_buf.table[entry_i].filesize = request->buffer_size;

    write_clusters(&fat32_driver_state.dir_table_buf.table, request->parent_cluster_number, 1);

    return 0;
}

int8_t write(struct FAT32DriverRequest *r) {

    struct FAT32DirectoryEntry *entry_p =
      dir_table_seq_search(r->name, r->ext, r->parent_cluster_number);

    if(!isValidDir(r->parent_cluster_number)){
        return 2; 
    }
    if(entry_p != 0){
        return 1;
    }
    // struct FAT32DirectoryEntry entry = *entry_p;

    // Write a directory
    if (r->buffer_size == 0) {
        uint32_t empty_cluster_number = 3;
        uint32_t current_fat_cluster;

        // Loop until find an empty entry
        while (true) {
            if (empty_cluster_number == CLUSTER_MAP_SIZE) {
                return -1;
            }

            current_fat_cluster = fat32_driver_state.fat_table.cluster_map[empty_cluster_number];

            if (current_fat_cluster == FAT32_FAT_EMPTY_ENTRY) {
                fat32_driver_state.fat_table.cluster_map[empty_cluster_number] =
                    FAT32_FAT_END_OF_FILE;
                init_directory_table(&fat32_driver_state.dir_table_buf, r->name,
                                     r->parent_cluster_number);
                write_clusters(&fat32_driver_state.dir_table_buf,
                               empty_cluster_number, 1);

                // Add entry to parent directory table
                add_entry(r, empty_cluster_number);
                write_clusters(&fat32_driver_state.fat_table, FAT_CLUSTER_NUMBER,
                               1);
                return 0;
            }
            empty_cluster_number++;
        }
    }

    // Write a File
    uint32_t remaining_size = r->buffer_size;
    uint32_t empty_cluster_number = 3;
    struct ClusterBuffer *buf_p = r->buf;
    uint32_t last_cluster_number = 0;

    while (remaining_size > 0) {
        if (empty_cluster_number == CLUSTER_MAP_SIZE) {
            return -1;
        }

        if (fat32_driver_state.fat_table.cluster_map[empty_cluster_number] ==
            FAT32_FAT_EMPTY_ENTRY) {
            if (last_cluster_number != 0) {
                fat32_driver_state.fat_table.cluster_map[last_cluster_number] =
                    empty_cluster_number;
            } else {
                // Add entry to parent directory table
                add_entry(r, empty_cluster_number);
            }

            while (fat32_driver_state.fat_table.cluster_map[empty_cluster_number] != FAT32_FAT_EMPTY_ENTRY)
                empty_cluster_number++;

            fat32_driver_state.fat_table.cluster_map[empty_cluster_number] = FAT32_FAT_END_OF_FILE;
            write_clusters(buf_p, empty_cluster_number, 1);

            buf_p++;
            if (remaining_size < CLUSTER_SIZE)
                remaining_size = 0;
            else
                remaining_size -= CLUSTER_SIZE;
            last_cluster_number = empty_cluster_number;
        }

        empty_cluster_number++;
    }

    if (empty_cluster_number) {
        write_clusters(&fat32_driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);
        return 0;
    }

    return -1;
}

int8_t deleteFAT32(struct FAT32DriverRequest *request){
    struct FAT32DirectoryEntry *entry_p = dir_table_seq_search(request->name, request->ext, request->parent_cluster_number);
    struct FAT32DirectoryTable dir_table;
    uint32_t i;
    // Not found
    if(entry_p == 0){
        return 1;
    }

    // Check if entry is a directory
    if(entry_p->attribute == ATTR_SUBDIRECTORY){
        // Check if it's empty
        read_clusters(&dir_table, (entry_p->cluster_high << 16) | entry_p->cluster_low, 1);
        for(i = 0; i < CLUSTER_SIZE/sizeof(struct FAT32DirectoryEntry); i++){
            if(dir_table.table[i].name[0] != 0){
                return 2; // Folder not empty
            }
        }
    }

    uint32_t cluster = entry_p->cluster_low | (entry_p->cluster_high << 16);
    while (cluster != FAT32_FAT_END_OF_FILE) {
        uint32_t next_cluster = fat32_driver_state.fat_table.cluster_map[cluster];
        fat32_driver_state.fat_table.cluster_map[cluster] = FAT32_FAT_EMPTY_ENTRY;
        cluster = next_cluster;
    }

    // Write the updated directory table back to the disk
    write_clusters(&dir_table, request->parent_cluster_number, 1);
    return 0;

    // TODO : Implement return -1
}

// -- CRUD HELPER FUNCTION -- 

bool isValidDir(uint32_t dir_cluster_number){
    if(fat32_driver_state.fat_table.cluster_map[dir_cluster_number] == FAT32_FAT_END_OF_FILE){
        return true;
    }
    else{
        return false;
    }
}
