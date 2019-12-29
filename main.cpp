#include <iostream>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <string.h>


struct boot_sector {
    uint16_t byter_per_sector;

};

typedef struct {
    char filename[11];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t creation_time_ms;
    uint16_t creat_time;
    uint16_t creat_date;
    uint16_t access_date;
    uint16_t first_cluster_addr_high;
    uint16_t modified_time;
    uint16_t modified_date;
    uint16_t first_cluster_addr_low;
    uint32_t file_size;
} dir_entry;

void print_directory_info(dir_entry *entry) {
    
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}