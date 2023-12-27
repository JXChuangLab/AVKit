#include <iostream>
#include <fstream>
#include <string>

#pragma pack(1) // 使其结构体按照1字节对齐，便于二进制读取时可以对结构体赋值

#define AUDIO_TAG 0x08
#define VIDEO_TAG 0x09
#define SCRIPT_TAG 0x12
typedef struct {
    uint8_t  signature[3];
    uint8_t  version;
    uint8_t  flags;
    uint32_t data_offset;
} FLV_HEADER;

typedef struct {
    uint8_t type;
    uint8_t data_size[3];
    uint8_t timestamp[3];
    uint8_t timestamp_ex;
    uint8_t stream_id[3];
} TAG_HEADER;

typedef struct {
    uint8_t  data; // 4bit code type, 2bit sample, 1bit precision, 1bit video type
}AUDIO_TAG_HEADER;

typedef struct {
    uint8_t data; // 4bit frame type, 4bit codec type
}VIDEO_TAG_HEADER;

// from Leixiaohua 
uint32_t reverse_bytes(uint8_t* p, int c) {
    uint32_t r = 0;
    int i;
    for (i = 0; i < c; i++)
        r |= (*(p + i) << (((c - 1) * 8) - 8 * i));
    return r;
}

void print_video_frame_type(int type){
    std::cout << "Video Frame Type: ";
    switch (type)
    {  
        case 1:std::cout<<"key frame ";break;
        case 2:std::cout<<"inter frame ";break;
        case 3:std::cout<<"disposable inter frame ";break;
        case 4:std::cout<<"generated keyframe ";break;
        case 5:std::cout<<"video info/command frame ";break;
        default: std::cout<<"UNKNOWN ";break;
    }
}

void print_video_codec_type(int type){
    std::cout << "Video Code Type: ";
    switch (type) {
        case 1:std::cout<< "JPEG (currently unused) ";break;
        case 2:std::cout<< "Sorenson H.263 ";break;
        case 3:std::cout<< "Screen video ";break;
        case 4:std::cout<< "On2 VP6 ";break;
        case 5:std::cout<< "On2 VP6 with alpha channel ";break;
        case 6:std::cout<< "Screen video version 2 ";break;
        case 7:std::cout<< "AVC ";break;
        default:std::cout<< "UNKNOWN ";break;
    }
}

void print_audio_code_type(int type){
    std::cout << "Audio Code Type: ";
    switch (type) {
        case 0:std::cout << "Linear PCM, platform endian ";break;
        case 1:std::cout << "ADPCM ";break;
        case 2:std::cout << "MP3 ";break;
        case 3:std::cout << "Linear PCM, little endian ";break;
        case 4:std::cout << "Nellymoser 16-kHz mono ";break;
        case 5:std::cout << "Nellymoser 8-kHz mono ";break;
        case 6:std::cout << "Nellymoser ";break;
        case 7:std::cout << "G.711 A-law logarithmic PCM ";break;
        case 8:std::cout << "G.711 mu-law logarithmic PCM ";break;
        case 9:std::cout << "reserved ";break;
        case 10:std::cout << "AAC ";break;
        case 11:std::cout << "Speex ";break;
        case 14:std::cout << "MP3 8-Khz ";break;
        case 15:std::cout << "Device-specific sound ";break;    
        default:std::cout << "UNKNOWN ";break;
    }
}

void print_sample_rate(int rate){
    std::cout << "Sample Rate: ";
    switch (rate) {
        case 0:std::cout << "5.5-kHz ";break;
        case 1:std::cout << "1-kHz ";break;
        case 2:std::cout << "2-kHz ";break;
        case 3:std::cout << "4-kHz ";break;
        default:std::cout << "UNKNOWN ";break;
    }
}

void print_sample_deep(int deep) {
    std::cout<< "Sample Deep: ";
    switch (deep) {
        case 0:std::cout << "8Bit ";break;
        case 1:std::cout << "16Bit ";break;
        default:std::cout << "UNKNOWN ";break;
    }
}

void print_audio_type(int type) {
    std::cout << "Audio Type: ";
    switch (type) {
        case 0:std::cout << "Mono ";break;
        case 1:std::cout << "Stereo ";break;
        default:std::cout << "UNKNOWN ";break;
    }
}
int main() {

    FLV_HEADER flv_header;
    // open file
    std::ifstream file("/Users/zhongxing/github/AVKit/AVFormat/test_file/cuc_ieschool.flv", std::ios::binary);

    if (!file.is_open()) {
        std::cout << "File open failed!";
        return -1;
    }

    file.read(reinterpret_cast<char*>(&flv_header), sizeof(FLV_HEADER));

    std::cout << "Signature: " << flv_header.signature << std::endl;
    std::cout << "Version: " << std::bitset<8>(flv_header.version) << std::endl;
    std::cout << "Flags: " << std::bitset<8>(flv_header.flags) << std::endl;
    std::cout << "Data offset: " << std::bitset<32>(flv_header.data_offset) << std::endl;

    // reverse flv body to get detail msg
    uint8_t data_header;
    uint32_t data_size;
    int x;
    while(!file.eof()){
        uint32_t previous_tag_size;
        file.read(reinterpret_cast<char*>(&previous_tag_size), sizeof(uint32_t));

        if(file.eof()) {
            std::cout<<"parse finsh !!!"<<std::endl;
            break;
        }

        TAG_HEADER tag_header;
        file.read(reinterpret_cast<char*>(&tag_header), sizeof(TAG_HEADER));

        if (file.eof()) {
            std::cout << "parse finsh !!!";
            break;
        }

        switch (tag_header.type) {
            case AUDIO_TAG:
                file.read(reinterpret_cast<char*>(&data_header), sizeof(uint8_t));
                // 对音频编码类型进行解析
                x = data_header & 0xF0;
                x = x >> 4;
                print_audio_code_type(x);

                // 对音频采样率进行解析
                x = data_header & 0x0C;
                x = x >> 2;
                print_sample_rate(x);

                // 对音频采样深度进行解析
                x = data_header & 0x02;
                x = x >> 1;
                print_sample_deep(x);

                // 对音频类型进行解析
                x = data_header & 0x01;
                print_audio_type(x);

                std::cout << std::endl;

                // 读取音频数据大小
                data_size = reverse_bytes(tag_header.data_size, sizeof(tag_header.data_size));
                //上面已经读区了数据的第一个字节，所以data_size需要减去1
                data_size -= 1;
                file.seekg(data_size, std::ios::cur);
                break;
            case VIDEO_TAG:
                file.read(reinterpret_cast<char*>(&data_header), sizeof(uint8_t));
                
                // 对视频帧类型进行解析
                x = data_header & 0xF0;
                x = x >> 4;
                print_video_frame_type(x);

                // 对视频编码类型进行解析
                x = data_header & 0x0F;
                print_video_codec_type(x);

                std::cout << std::endl;

                data_size = reverse_bytes(tag_header.data_size, sizeof(tag_header.data_size));
                //上面已经读区了数据的第一个字节，所以data_size需要减去1
                data_size -= 1;
                file.seekg(data_size, std::ios::cur);
                break;
            default:
                data_size = reverse_bytes(tag_header.data_size, sizeof(tag_header.data_size));
                file.seekg(data_size, std::ios::cur);
                break;
        }

        if (file.eof()) {
            std::cout << "parse finsh !!!";
            break;
        }
    };

    file.close();
}