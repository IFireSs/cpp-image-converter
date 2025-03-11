#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>


using namespace std;

namespace img_lib {

    PACKED_STRUCT_BEGIN BitmapFileHeader {
        std::array<uint8_t, 2> sign{'B', 'M'};
        uint32_t size = 0;
        uint32_t reserve = 0;
        uint32_t padding = 54;
    }
    PACKED_STRUCT_END

    PACKED_STRUCT_BEGIN BitmapInfoHeader {
        uint32_t size = 40;
        int32_t width = 0;
        int32_t height = 0;
        uint16_t planes = 1;
        uint16_t pixelSize = 24;
        uint32_t compression = 0;
        uint32_t dataSize = 0;
        int32_t horizontalResolution = 11811;
        int32_t verticalResolution = 11811;
        int32_t colors = 0;
        int32_t significantColors = 0x1000000;
    }
    PACKED_STRUCT_END

// функция вычисления отступа по ширине
    static int GetBMPStride(int w) {
        return 4 * ((w * 3 + 3) / 4);
    }

// напишите эту функцию
    bool SaveBMP(const Path& file, const Image& image) {
        ofstream ofs(file, ios::binary);
        BitmapFileHeader fileHeader;
        BitmapInfoHeader infoHeader;
        int w = image.GetWidth();
        int h = image.GetHeight();
        int BMPStride = GetBMPStride(w);
        fileHeader.size = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) + BMPStride * h;
        infoHeader.width = w;
        infoHeader.height = h;
        infoHeader.dataSize = BMPStride * infoHeader.height ;
        ofs.write(reinterpret_cast<char*>(&fileHeader),sizeof(BitmapFileHeader));
        ofs.write(reinterpret_cast<char*>(&infoHeader),sizeof(BitmapInfoHeader));
        vector<char> buff(BMPStride);
        for (int y = 0; y < infoHeader.height; ++y) {
            const Color* line = image.GetLine(infoHeader.height - 1 - y);
            for (int x = 0; x < infoHeader.width; ++x) {
                buff[x * 3 + 0] = static_cast<char>(line[x].b);
                buff[x * 3 + 1] = static_cast<char>(line[x].g);
                buff[x * 3 + 2] = static_cast<char>(line[x].r);
            }
            ofs.write(buff.data(), BMPStride);
        }
        return ofs.good();
    }

// напишите эту функцию
    Image LoadBMP(const Path& file) {
        BitmapFileHeader fileHeader;
        BitmapInfoHeader infoHeader;
        ifstream ifs(file, ios::binary);
        ifs.read(reinterpret_cast<char*>(&fileHeader), sizeof(BitmapFileHeader));
        ifs.read(reinterpret_cast<char*>(&infoHeader), sizeof(BitmapInfoHeader));
        Image result(infoHeader.width, infoHeader.height, Color::Black());
        int BMPStride = GetBMPStride(infoHeader.width);
        vector<char> buff(BMPStride);
        for (int y = 0; y < infoHeader.height; ++y) {
            Color* line = result.GetLine(infoHeader.height - 1 - y);
            ifs.read(buff.data(), BMPStride);
            for (int x = 0; x < infoHeader.width; ++x) {
                line[x].b = static_cast<byte>(buff[x * 3 + 0]);
                line[x].g = static_cast<byte>(buff[x * 3 + 1]);
                line[x].r = static_cast<byte>(buff[x * 3 + 2]);
            }
        }
        return result;
    }

}  // namespace img_lib