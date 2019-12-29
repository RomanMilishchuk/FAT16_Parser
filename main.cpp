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
#include <sstream>


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

typedef struct {
    char bootsrap_code[446];
    struct mbr_partition {
        char status;
        char head_first;
        char sector_first;
        char cylinder_first;
        char partition_type;
        char head_last;
        char sector_last;
        char cylinder_last;
        uint32_t LBA;
        uint32_t num_of_sectors;
    } partition[4];
    int mbr_signature;
} MBR;

int main() {
    std::ifstream is{"../hd0_with_mbr.img"};
    std::stringstream buffer;
    buffer << is.rdbuf();
    auto FAT = buffer.str();
    MBR MBRPartition;
    memcpy(&MBRPartition, FAT.c_str(), 512);
    std::cout<<MBRPartition.mbr_signature<<std::endl;
    return 0;
}