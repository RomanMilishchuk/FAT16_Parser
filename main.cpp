#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>

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
} dir_entry_info;

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

void read_boot_sector(boot_sector_info *bs, const std::string &data, off_t offset) {
    memcpy(&bs->bytes_per_sector, data.c_str() + offset + 11, 8);
    memcpy(&bs->fat_size, data.c_str() + offset + 22, 2);
    memcpy(&bs->signature, data.c_str() + offset + 510, 2);
}

void read_dir_info(dir_entry_info *info, const std::string data, off_t offset) {
    memcpy(&info->filename, data.c_str() + offset, 32);
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

std::string parse_filename(char filename[11]) {
    std::string name;
    int trailing_spaces = 0, i = 7;
    while (filename[i] == ' ') {
        trailing_spaces++;
        --i;
    }
    for (i = 0; i < 8 - trailing_spaces; i++)
        name += filename[i];
    while (name[i] == ' ') {
        name = name.substr(0, --i);
    }
    i = 10;
    trailing_spaces = 0;
    while (filename[i] == ' ') {
        trailing_spaces++;
        --i;
    }
    if (i > 7) {
        name += '.';
        for (i = 8; i < 11 - trailing_spaces; i++)
            name += filename[i];
    }
    return name;
}

std::string parse_date(uint16_t date, uint16_t time) {
    uint32_t year, month, day;
    year = 1980 + ((date >> 9) & 0x7f);
    month = 1 + ((date >> 5) & 0x0f);
    day = 1 + (date & 0x1f);
    uint32_t hours, minutes, seconds;
    hours = (time >> 11) & 0x1f;
    minutes = (time >> 5) & 0x3f;
    seconds = (time & 0x1f) * 2;
    char data[20];
    sprintf(data, "%d/%02d/%02d %02d:%02d:%02d", day, month, year, hours, minutes, seconds);
    data[19] = '\0';
    return std::string{data};
}

void print_directory_info(dir_entry_info *entry) {
    if (entry->filename[0] == 0x00) {
        printf("Unallocated\n");
        return;
    } else if (static_cast<unsigned char>(entry->filename[0]) == 0xe5) {
        printf("Deleted\n");
        return;
    }
    std::string name = parse_filename(entry->filename);
    printf("Filename: %s\n", name.c_str());
    if (entry->file_size) {
        printf("File has size: %d\n", entry->file_size);
    } else {
        printf("File is a directory\n");
    }
    printf("Creation date: %s\n", parse_date(entry->creat_date, entry->creat_time).c_str());
    printf("Modification date: %s\n", parse_date(entry->modified_date, entry->modified_time).c_str());
    printf("File has next attributes: \n");
    if (entry->attributes & 0x01)
        printf("\tRead-only File\n");
    if (entry->attributes & 0x02)
        printf("\tHidden file\n");
    if (entry->attributes & 0x04)
        printf("\tSystem file\n");
    if (entry->attributes & 0x08)
        printf("\tVolume label\n");
    if (entry->attributes & 0x0f)
        printf("\tLong file name\n");
    if (entry->attributes & 0x10)
        printf("\tDirectory\n");
    if (entry->attributes & 0x20)
        printf("\tArchive\n");
    printf("Number of first sector: %d\n",
           (entry->first_cluster_addr_high << 16) | (entry->first_cluster_addr_low));
}

void all_dirs_info(boot_sector_info *boot, const std::string &data) {
    dir_entry_info root_dir_info{};
    printf("\n");
    auto file_num = 0, i = 0;
    while (i < boot->max_files_n) {
        read_dir_info(&root_dir_info, data,
                      (boot->reserved_size + boot->fats_n * boot->fat_size) * boot->bytes_per_sector + 32 * i);
        if (root_dir_info.filename[0] != 0x00 && static_cast<unsigned char>(root_dir_info.filename[0]) != 0xe5) {
            print_directory_info(&root_dir_info);
            printf("\n");
            file_num++;
        }
        i++;
    }
    printf("Total number of files: %d\n", file_num);
}

void read_mbr(MBR *mbr, const std::string &src) {
    memcpy(mbr, src.c_str(), 512);
    size_t offset;
    for (const auto &i : mbr->partition) {
        if (i.status == ACTIVE_PARTITION &&
            (i.partition_type == FAT_ID || i.partition_type == FAT_ID_LARGE || i.partition_type == HIDDEN_FAT)) {
            offset = 512 * i.LBA;
            boot_sector_info boot;
            read_boot_sector(&boot, src.substr(offset, src.size() - offset), 0);
            print_bs(&boot);
            all_dirs_info(&boot, src.substr(offset, src.size() - offset));
        }
    }
}

int main() {
    std::ifstream infile{"../hd0_just_FAT16_without_MBR.img", std::ios::binary};
    std::string data((std::istreambuf_iterator<char>(infile)),
                     std::istreambuf_iterator<char>());
    boot_sector_info bs_info{};
    read_boot_sector(&bs_info, data, 0);
    print_bs(&bs_info);
    all_dirs_info(&bs_info, data);

    std::cout << std::endl << "---------------------------------------------------------------------------------------"
                              "---------------------------------------------" << std::endl << "With MBR" << std::endl;
    std::ifstream is{"../hd0_with_mbr.img"};
    std::stringstream buffer;
    buffer << is.rdbuf();
    auto FAT = buffer.str();
    MBR MBRPartition;
    read_mbr(&MBRPartition, FAT);
    return 0;
}