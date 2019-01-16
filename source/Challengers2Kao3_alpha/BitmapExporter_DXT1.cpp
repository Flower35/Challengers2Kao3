#include <Challengers2Kao3_alpha/BitmapExporter.h>


////////////////////////////////////////////////////////////////
// DXT1 TEXTURE DECOMPRESSION
// https://bitbucket.org/Anteru/dxt-decompress/src
////////////////////////////////////////////////////////////////

static unsigned int PackRGBA (unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    // return r | (g << 8) | (b << 16) | (a << 24);
    return b | (g << 8) | (r << 16) | (a << 24); // Odwróć kolory (BGRA)
}


static void DecompressBlockBC1Internal (const unsigned char* block, unsigned char* output, unsigned int outputStride, const unsigned char* alphaValues) {

    unsigned int temp, code;

    unsigned short color0, color1;
    unsigned char r0, g0, b0, r1, g1, b1;

    int i, j;

    color0 = *(const unsigned short*)(block);
    color1 = *(const unsigned short*)(block + 2);

    temp = (color0 >> 11) * 255 + 16;
    r0 = (unsigned char)((temp/32 + temp)/32);
    temp = ((color0 & 0x07E0) >> 5) * 255 + 32;
    g0 = (unsigned char)((temp/64 + temp)/64);
    temp = (color0 & 0x001F) * 255 + 16;
    b0 = (unsigned char)((temp/32 + temp)/32);

    temp = (color1 >> 11) * 255 + 16;
    r1 = (unsigned char)((temp/32 + temp)/32);
    temp = ((color1 & 0x07E0) >> 5) * 255 + 32;
    g1 = (unsigned char)((temp/64 + temp)/64);
    temp = (color1 & 0x001F) * 255 + 16;
    b1 = (unsigned char)((temp/32 + temp)/32);

    code = *(const unsigned int*)(block + 4);

    if (color0 > color1) {
        for (j = 0; j < 4; ++j) {
            for (i = 0; i < 4; ++i) {
                unsigned int finalColor, positionCode;
                unsigned char alpha;

                alpha = alphaValues [j*4+i];

                finalColor = 0;
                positionCode = (code >>  2*(4*j+i)) & 0x03;

                switch (positionCode) {
                case 0:
                    finalColor = PackRGBA(r0, g0, b0, alpha);
                    break;
                case 1:
                    finalColor = PackRGBA(r1, g1, b1, alpha);
                    break;
                case 2:
                    finalColor = PackRGBA((2*r0+r1)/3, (2*g0+g1)/3, (2*b0+b1)/3, alpha);
                    break;
                case 3:
                    finalColor = PackRGBA((r0+2*r1)/3, (g0+2*g1)/3, (b0+2*b1)/3, alpha);
                    break;
                }

                *(unsigned int*)(output + j*outputStride + i * sizeof(unsigned int)) = finalColor;
            }
        }
    } else {
        for (j = 0; j < 4; ++j) {
            for (i = 0; i < 4; ++i) {
                unsigned int finalColor, positionCode;
                unsigned char alpha;

                alpha = alphaValues [j*4+i];

                finalColor = 0;
                positionCode = (code >>  2*(4*j+i)) & 0x03;

                switch (positionCode) {
                case 0:
                    finalColor = PackRGBA(r0, g0, b0, alpha);
                    break;
                case 1:
                    finalColor = PackRGBA(r1, g1, b1, alpha);
                    break;
                case 2:
                    finalColor = PackRGBA((r0+r1)/2, (g0+g1)/2, (b0+b1)/2, alpha);
                    break;
                case 3:
                    finalColor = PackRGBA(0, 0, 0, alpha);
                    break;
                }

                *(unsigned int*)(output + j*outputStride + i * sizeof(unsigned int)) = finalColor;
            }
        }
    }
}


////////////////////////////////////////////////////////////////
// Decompresses one block of a BC1 (DXT1) texture and stores the resulting pixels at the appropriate offset in 'image'.
// uint32_t x: x-coordinate of the first pixel in the block.
// uint32_t y: y-coordinate of the first pixel in the block.
// uint32_t stride: stride of a scanline in bytes.
// const uint8_t* blockStorage: pointer to the block to decompress.
// uint32_t* image: pointer to image where the decompressed pixel data should be stored.
////////////////////////////////////////////////////////////////
void CBitmapExporter::DecompressBlockBC1 (unsigned int x, unsigned int y, unsigned int stride, const unsigned char* blockStorage, unsigned char* image) {
    static const unsigned char const_alpha [] = {
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255 };

    DecompressBlockBC1Internal (blockStorage, image + x * sizeof(unsigned int) + (y * stride), stride, const_alpha);
}
