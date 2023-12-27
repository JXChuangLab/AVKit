#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)
typedef struct{
    uint8_t _signature[3];
    uint8_t _version;
    uint8_t _flags;
    uint32_t _data_offset;
}FLV_HEADER;

int main() {

    FLV_HEADER _flv_header;
    // open file
    std::ifstream file("/Users/zhongxing/github/AVKit/AVFormat/test_file/test_cuc_ieschool.flv", std::ios::binary);

    if (!file.is_open()) {
        std::cout << "File open failed!" << std::endl;
    }

    file.read(reinterpret_cast<char*>(&_flv_header), sizeof(FLV_HEADER));

    std::cout << "Signature: " << _flv_header._signature << std::endl;
    std::cout << "Version: " << std::bitset<8>(_flv_header._version) << std::endl;
    std::cout << "Flags: " << std::bitset<8>(_flv_header._flags) << std::endl;
    std::cout << "Data offset: " << std::bitset<32>(_flv_header._data_offset) << std::endl;

    file.close();
}