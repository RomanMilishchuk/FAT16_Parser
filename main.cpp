#include <iostream>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>


struct boot_sector_info {
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_size;
    uint8_t fats_n;
    uint16_t max_files_n;
    uint16_t fat_size;
    uint16_t signature;
};


int read_boot_sector(boot_sector_info* bs, const std::string& file_name){
    std::ifstream infile{file_name, std::ios::binary};
    std::string data((std::istreambuf_iterator<char>(infile)),
                     std::istreambuf_iterator<char>());
    memcpy(&bs->bytes_per_sector, data.c_str() + 11, 2);
    memcpy(&bs->sectors_per_cluster, data.c_str() + 13, 1);
    memcpy(&bs->reserved_size, data.c_str() + 14, 2);
    memcpy(&bs->fats_n, data.c_str() + 16, 1);
    memcpy(&bs->max_files_n, data.c_str() + 17, 2);
    memcpy(&bs->fat_size, data.c_str() + 22, 2);
    memcpy(&bs->signature, data.c_str() + 510, 2);
    return 1;
}

void print_bs(boot_sector_info* bs){
    printf("Bytes per sector %d\n", bs->bytes_per_sector);
    printf("Sectors per cluster %d\n", bs->sectors_per_cluster);
    printf("Reserved area size %d\n", bs->reserved_size);
    printf("FATS number %d\n", bs->fats_n);
    printf("Max files number %d\n", bs->max_files_n);
    printf("FAT size %d\n", bs->fat_size);
    printf("Valid signature  %d\n", bs->signature == 0xaa55);
}

int main() {
    return 0;
}