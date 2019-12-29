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

#pragma pack(1)

#define FAT_ID 4
#define FAT_ID_LARGE 6
#define HIDDEN_FAT 144
#define ACTIVE_PARTITION 128

typedef struct {
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_size;
    uint8_t fats_n;
    uint16_t max_files_n;
    uint16_t fat_size;
    uint16_t signature;
} boot_sector_info;


int read_boot_sector(boot_sector_info *bs, const std::string &data) {
    memcpy(&bs->bytes_per_sector, data.c_str() + 11, 2);
    memcpy(&bs->sectors_per_cluster, data.c_str() + 13, 1);
    memcpy(&bs->reserved_size, data.c_str() + 14, 2);
    memcpy(&bs->fats_n, data.c_str() + 16, 1);
    memcpy(&bs->max_files_n, data.c_str() + 17, 2);
    memcpy(&bs->fat_size, data.c_str() + 22, 2);
    memcpy(&bs->signature, data.c_str() + 510, 2);
    return 1;
}

void print_bs(boot_sector_info *bs) {
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
    uint8_t bootsrap_code[446];
    struct mbr_partition {
        uint8_t status;
        uint8_t head_first;
        uint8_t sector_first;
        uint8_t cylinder_first;
        uint8_t partition_type;
        uint8_t head_last;
        uint8_t sector_last;
        uint8_t cylinder_last;
        uint32_t LBA;
        uint32_t num_of_sectors;
    } partition[4];
    uint32_t mbr_signature;
} MBR;

int read_mbr(MBR *mbr, const std::string &src) {
    memcpy(mbr, src.c_str(), 512);
    size_t mbr_offset = 512;
    size_t offset;
    for (const auto &i : mbr->partition) {
        if (i.status == ACTIVE_PARTITION &&
            (i.partition_type == FAT_ID || i.partition_type == FAT_ID_LARGE || i.partition_type == HIDDEN_FAT)) {
            offset = mbr_offset + i.LBA;
            boot_sector_info boot;
            read_boot_sector(&boot, src.substr(offset, src.size() - offset));
            print_bs(&boot);
        }
    }
}

int main() {
    std::ifstream is{"../demodisk.img"};
    std::stringstream buffer;
    buffer << is.rdbuf();
    auto FAT = buffer.str();
    MBR MBRPartition;
    read_mbr(&MBRPartition, FAT);
    return 0;
}