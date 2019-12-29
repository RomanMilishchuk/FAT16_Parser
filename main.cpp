#include <iostream>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <sstream>


struct boot_sector {
    uint16_t byter_per_sector;

};

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