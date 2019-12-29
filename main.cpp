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

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}