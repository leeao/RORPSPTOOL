#include "Image.h"
#include <string.h>
#include <fstream>
#include <iostream>
void untile(char* pixel,char* outBuffer, int width, int height, int tile_w, int tile_h, int bpp)
{
    //bpp is bits per pixel
    //tile_w is tile width
    //tile_h is tile height
    int numTileW = width / tile_w;
    int numTileH = height / tile_h;
    int lineSize = tile_w * bpp / 8;
    int tileSize = tile_w * tile_h * bpp / 8;
    
    for (int y = 0; y < numTileH; y++)
    {
        for (int x = 0; x < numTileW; x++)
        {
            unsigned int dataPtr = y * numTileW * tileSize + tileSize * x;
            for ( int ty = 0; ty < tile_h; ty++)
            { 
                int curHeight = y * tile_h + ty;
                int curWidth = x * tile_w;
                int dstIndex = (curHeight * width + curWidth) * bpp / 8;
                int srcIndex = dataPtr + ty * lineSize;
                //memcpy(outBuffer + dstIndex, pixel + srcIndex, lineSize);
                memcpy_s(outBuffer + dstIndex, lineSize, pixel + srcIndex, lineSize);
            }
        }
    }
}
void crop(int width, int height, int storageWidth, int storageHeight, int bitsPerPixel,char* srcBuffer, char* dstBuffer)
{
    int srcLineSize = storageWidth  * bitsPerPixel / 8;
    int dstLineSize = width  * bitsPerPixel / 8;
    for (int y = 0; y < height; y++)
    {
        unsigned int srcPtr = y * srcLineSize;
        unsigned int dstPtr = y * dstLineSize;
        memcpy_s(dstBuffer + dstPtr, dstLineSize, (srcBuffer + srcPtr), dstLineSize);
    }
}
int getStorageWidth(int stride, int bitsPerPixel)
{
    int width = (stride * 8) / bitsPerPixel;
    return width;
}
//#alpha_settings,DXT1只能表示两种透明度，一种是完全透明,一种是完全不透明,可为alpha_settings设为True(DXT1A)/False(DXT1)
//dxt1 4x4像素为一个压缩单位,color0,color1为色盘,剩余4字节(32bits)为索引区域,每2bit代表一个像素颜色
//此时2bit可以表示4种颜色变化
//所以除了color0和color1还可以插值出2种颜色
//color2=color0*2/3+color1*1/3
//color3=color1*2/3+color0*1/3
void decodeRORDXT1(char* data, char* outBuffer, int width, int height, bool alphaSetting)
{    
    char* tiledRGBA32Buffer = new char[width * height * 4];
    unsigned int dxt1Size = width * height / 2;
    char* br = data;
    for (int block = 0; block < (dxt1Size / 8); block++)
    //for(int y = 0; y < height; y++)
    {
        unsigned short color0 = *(unsigned short*)(br+4);
        unsigned short color1 = *(unsigned short*)(br+6);
        //br += 4;
        unsigned char r0 = (unsigned char)(((color0 >> 11) & 0x1f) << 3);
        unsigned char g0 = (unsigned char)(((color0 >> 5) & 0x3f) << 2);
        unsigned char b0 = (unsigned char)(((color0 & 0x1f)) << 3);
        unsigned char r1 = (unsigned char)(((color1 >> 11) & 0x1f) << 3);
        unsigned char g1 = (unsigned char)(((color1 >> 5) & 0x3f) << 2);
        unsigned char b1 = (unsigned char)(((color1 & 0x1f)) << 3);

        unsigned char a0 = 0xff;

        unsigned char* blockPixels = new unsigned char[16];
        for (int i = 0; i < 4; i++)
        {
            unsigned char bt = *br++;
            for (int b = 0; b < 4; b++)
            {
                unsigned char j = (unsigned char)(((bt & (0x03 << (b * 2))) >> (b * 2)));
                blockPixels[i * 4 + b] = j;
            }
        }
        br += 4;
        for (int i = 0; i < 16; i++)
        {

            unsigned char colorIndex = blockPixels[i];
            if (colorIndex == 0) //如果索引为0，输出color0
            {
                tiledRGBA32Buffer[block * 64 + i * 4] = r0; tiledRGBA32Buffer[block * 64 + i * 4 + 1] = g0; tiledRGBA32Buffer[block * 64 + i * 4 + 2] = b0; tiledRGBA32Buffer[block * 64 + i * 4 + 3] = a0;
            }
            else if (colorIndex == 1)//如果索引为1，输出color1
            {
                tiledRGBA32Buffer[block * 64 + i * 4] = r1; tiledRGBA32Buffer[block * 64 + i * 4 + 1] = g1; tiledRGBA32Buffer[block * 64 + i * 4 + 2] = b1; tiledRGBA32Buffer[block * 64 + i * 4 + 3] = a0;
            }
            else if (colorIndex == 2)//如果索引为2，进行插值color2=color0*2/3+color1*1/3
            {
                if (!alphaSetting) {
                    tiledRGBA32Buffer[block * 64 + i * 4] = (unsigned char)((r0 * 2 + r1) / 3);
                    tiledRGBA32Buffer[block * 64 + i * 4 + 1] = (unsigned char)((g0 * 2 + g1) / 3);
                    tiledRGBA32Buffer[block * 64 + i * 4 + 2] = (unsigned char)((b0 * 2 + b1) / 3);
                    tiledRGBA32Buffer[block * 64 + i * 4 + 3] = a0;
                }
                else
                {
                    if (color0 > color1)
                    {
                        tiledRGBA32Buffer[block * 64 + i * 4] = (unsigned char)((r0 * 2 + r1) / 3);
                        tiledRGBA32Buffer[block * 64 + i * 4 + 1] = (unsigned char)((g0 * 2 + g1) / 3);
                        tiledRGBA32Buffer[block * 64 + i * 4 + 2] = (unsigned char)((b0 * 2 + b1) / 3);
                        tiledRGBA32Buffer[block * 64 + i * 4 + 3] = a0;
                    }
                    else
                    {
                        tiledRGBA32Buffer[block * 64 + i * 4] = (unsigned char)((r1 + r0) / 2);
                        tiledRGBA32Buffer[block * 64 + i * 4 + 1] = (unsigned char)((g1 + g0) / 2);
                        tiledRGBA32Buffer[block * 64 + i * 4 + 2] = (unsigned char)((b1 + b0) / 2);
                        tiledRGBA32Buffer[block * 64 + i * 4 + 3] = a0;
                    }
                }
            }
            else if (colorIndex == 3)//如果索引为3，进行插值color1*2/3+color0*1/3
            {
                if (!alphaSetting)
                {
                    tiledRGBA32Buffer[block * 64 + i * 4] = (unsigned char)((r1 * 2 + r0) / 3);
                    tiledRGBA32Buffer[block * 64 + i * 4 + 1] = (unsigned char)((g1 * 2 + g0) / 3);
                    tiledRGBA32Buffer[block * 64 + i * 4 + 2] = (unsigned char)((b1 * 2 + b0) / 3);
                    tiledRGBA32Buffer[block * 64 + i * 4 + 3] = a0;
                }
                else
                {
                    if (color0 > color1)
                    {
                        tiledRGBA32Buffer[block * 64 + i * 4] = (unsigned char)((r1 * 2 + r0) / 3);
                        tiledRGBA32Buffer[block * 64 + i * 4 + 1] = (unsigned char)((g1 * 2 + g0) / 3);
                        tiledRGBA32Buffer[block * 64 + i * 4 + 2] = (unsigned char)((b1 * 2 + b0) / 3);
                        tiledRGBA32Buffer[block * 64 + i * 4 + 3] = a0;
                    }
                    else
                    {
                        tiledRGBA32Buffer[block * 64 + i * 4] = 0;
                        tiledRGBA32Buffer[block * 64 + i * 4 + 1] = 0;
                        tiledRGBA32Buffer[block * 64 + i * 4 + 2] = 0;
                        tiledRGBA32Buffer[block * 64 + i * 4 + 3] = a0;
                    }
                }
            }
        }

    }
    untile(tiledRGBA32Buffer, outBuffer, width, height, 4, 4, 32);

    delete[] tiledRGBA32Buffer;

}

/*
# DXT3为64bit的alpha索引+64bit DXT1颜色插值，每个4x4tile占16字节
#############
#-------------8字节alpha索引，每像素的alpha占4bit------------------
# 每个像素可以有16种alpha值
# 0   1   2   3
# 4   5   6   7
# 8   9   10  11
#12   13  14  15
#-------------4字节颜色值，color0和color1--------------------------
#
#-------------4字节插值判断,每个像素占2bit,共4种插值结果
#0=color0,1=color1,2=color0*2/3+color1*1/3,3=color1*2/3+color0*1/3
# 共16字节 128bit
*/
void decodeRORDXT3(char* data, char* outBuffer, int width, int height)
{
    char* tiledRGBA32Buffer = new char[width * height * 4];
    unsigned int dxt3Size = width * height ;
    char* br = data;
    for (int block = 0; block < (dxt3Size / 16); block++)
    {
        unsigned char* blockPixels = new unsigned char[16];
        for (int i = 0; i < 4; i++)
        {
            unsigned char bt = *(unsigned char*)br++;
            for (int b = 0; b < 4; b++)
            {
                unsigned char j = (unsigned char)(((bt & (0x03 << (b * 2))) >> (b * 2)));
                blockPixels[i * 4 + b] = j;
            }
        }
        unsigned short color0 = *(unsigned short*)(br);
        unsigned short color1 = *(unsigned short*)(br + 2);
        br += 4;
        unsigned char r0 = (unsigned char)(((color0 >> 11) & 0x1f) << 3);
        unsigned char g0 = (unsigned char)(((color0 >> 5) & 0x3f) << 2);
        unsigned char b0 = (unsigned char)(((color0 & 0x1f)) << 3);
        unsigned char r1 = (unsigned char)(((color1 >> 11) & 0x1f) << 3);
        unsigned char g1 = (unsigned char)(((color1 >> 5) & 0x3f) << 2);
        unsigned char b1 = (unsigned char)(((color1 & 0x1f)) << 3);

        unsigned char* alphaList = new unsigned char[16];
        for (int i = 0; i < 2; i++)
        {
            unsigned int dw = *(unsigned int*)br; br += 4;
            for (int j = 0; j < 8; j++)
            {
                unsigned char alpha = (unsigned char)(((dw & 0xf) << 4) & 0xff);
                alphaList[i * 8 + j] = alpha;
                dw >>= 4;
            }
        }




        for (int i = 0; i < 16; i++)
        {

            unsigned char colorIndex = blockPixels[i];
            unsigned char alpha = alphaList[i];
            if (colorIndex == 0) //如果索引为0，输出color0
            {
                tiledRGBA32Buffer[block * 64 + i * 4] = r0; tiledRGBA32Buffer[block * 64 + i * 4 + 1] = g0; tiledRGBA32Buffer[block * 64 + i * 4 + 2] = b0; tiledRGBA32Buffer[block * 64 + i * 4 + 3] = alpha;
            }
            else if (colorIndex == 1)//如果索引为1，输出color1
            {
                tiledRGBA32Buffer[block * 64 + i * 4] = r1; tiledRGBA32Buffer[block * 64 + i * 4 + 1] = g1; tiledRGBA32Buffer[block * 64 + i * 4 + 2] = b1; tiledRGBA32Buffer[block * 64 + i * 4 + 3] = alpha;
            }
            else if (colorIndex == 2)
            {
                tiledRGBA32Buffer[block * 64 + i * 4] = (unsigned char)((2 * r0 + r1) / 3);
                tiledRGBA32Buffer[block * 64 + i * 4 + 1] = (unsigned char)((2 * g0 + g1) / 3);
                tiledRGBA32Buffer[block * 64 + i * 4 + 2] = (unsigned char)((b0 * 2 + b1) / 3);
                tiledRGBA32Buffer[block * 64 + i * 4 + 3] = alpha;
            }
            else if (colorIndex == 3)
            {
                tiledRGBA32Buffer[block * 64 + i * 4] = (unsigned char)((r1 * 2 + r0) / 3);
                tiledRGBA32Buffer[block * 64 + i * 4 + 1] = (unsigned char)((g1 * 2 + g0) / 3);
                tiledRGBA32Buffer[block * 64 + i * 4 + 2] = (unsigned char)((b1 * 2 + b0) / 3);
                tiledRGBA32Buffer[block * 64 + i * 4 + 3] = alpha;
            }
        }
    }
    untile(tiledRGBA32Buffer, outBuffer, width, height, 4, 4, 32);

    delete[] tiledRGBA32Buffer;
}
/*
# DXT5为64bit的alpha索引+64bit DXT1颜色插值，每个4x4tile占16字节
#
#-------------8字节alpha索引，每像素的alpha占4bit------------------
# 头2字节为alpha_0和alpha_1，后6字节为alpha插值索引。每个像素占3bit
# 每个像素可以插值出6种或者8种
#0=alpha0,1=alpha1,2-5插值,6=0x0完全透明,7=0xff完全不透明
#6字节alpha插值区域
# 0   1   2   3
# 4   5   6   7
# 8   9   10  11
#12   13  14  15
#3bit索引读取方法如下
# 比如索引区为'\xa0\x93\x24\x49\x92\x24'
#2进制为 10100000,10010011,00100100,01001001,10010010,00100100
# 索引表为
#000,100,110,001,
#001,001,001,001,
#001,001,001,001,
#001,001,001,001,
# 即对应索引表为
# 0  4   6   1
# 1  1   1   1
# 1  1   1   1
# 1  1   1   1
#-------------4字节颜色值，color0和color1--------------------------
#-------------4字节颜色插值判断,每个像素占2bit,共4种插值结果
*/
void decodeRORDXT5(char* data, char* outBuffer, int width, int height)
{
    char* tiledRGBA32Buffer = new char[width * height * 4];
    unsigned int dxt5Size = width * height;
    char* br = data;
    for (int block = 0; block < (dxt5Size / 16); block++)
    {
        unsigned char* blockPixels = new unsigned char[16];
        for (int i = 0; i < 4; i++)
        {
            unsigned char bt = *(unsigned char*)br++;;
            for (int b = 0; b < 4; b++)
            {
                unsigned char j = (unsigned char)(((bt & (0x03 << (b * 2))) >> (b * 2)));
                blockPixels[i * 4 + b] = j;
            }
        }
        unsigned short color0 = *(unsigned short*)(br);
        unsigned short color1 = *(unsigned short*)(br + 2);
        br += 4;
        unsigned char r0 = (unsigned char)(((color0 >> 11) & 0x1f) << 3);
        unsigned char g0 = (unsigned char)(((color0 >> 5) & 0x3f) << 2);
        unsigned char b0 = (unsigned char)(((color0 & 0x1f)) << 3);
        unsigned char r1 = (unsigned char)(((color1 >> 11) & 0x1f) << 3);
        unsigned char g1 = (unsigned char)(((color1 >> 5) & 0x3f) << 2);
        unsigned char b1 = (unsigned char)(((color1 & 0x1f)) << 3);

        unsigned char alpha_0 = *(unsigned char*)(br+6);
        unsigned char alpha_1 = *(unsigned char*)(br+7);
        unsigned char* alphaPalette = new unsigned char[8];

        alphaPalette[0] = alpha_0;
        alphaPalette[1] = alpha_1;

        float alphaf0 = (float)alpha_0 * (1.0f / 255.0f);
        float alphaf1 = (float)alpha_1 * (1.0f / 255.0f);
        if (alpha_0 > alpha_1)
        {
            for (int i = 1; i < 7; i++)
                alphaPalette[i + 1] = (unsigned char)(((alphaf0 * (7 - i) + alphaf1 * i) * (1.0f / 7.0f)) * 255.0f);

        }
        else
        {
            for (int i = 1; i < 5; i++)
                alphaPalette[i + 1] = (unsigned char)(((alphaf0 * (5 - i) + alphaf1 * i) * (1.0f / 5.0f)) * 255.0f);


            alphaPalette[6] = 0;
            alphaPalette[7] = 255;
        }


        unsigned char* alphaPixelList = new unsigned char[16];

        for (int i = 0; i < 2; i++)
        {
            unsigned int alphaIndex0 = *(unsigned char*)br++;
            unsigned int alphaIndex1 = *(unsigned char*)br++;
            unsigned int alphaIndex2 = *(unsigned char*)br++;
            unsigned int dw = (alphaIndex0 | (alphaIndex1 << 8) | alphaIndex2 << 16);
            for (int j = 0; j < 8; j++)
            {
                int dataPos = i * 8 + j;
                unsigned int alphaIndex = dw & 0x7;
                alphaPixelList[dataPos] = alphaPalette[alphaIndex];
                //alphaIndexList[dataPos] = (byte)alphaIndex;
                dw >>= 3;
            }
        }
        br += 2;




        for (int i = 0; i < 16; i++)
        {

            unsigned char colorIndex = blockPixels[i];
            unsigned char alpha = alphaPixelList[i];
            if (colorIndex == 0) //如果索引为0，输出color0
            {
                tiledRGBA32Buffer[block * 64 + i * 4] = r0; tiledRGBA32Buffer[block * 64 + i * 4 + 1] = g0; tiledRGBA32Buffer[block * 64 + i * 4 + 2] = b0; tiledRGBA32Buffer[block * 64 + i * 4 + 3] = alpha;
            }
            else if (colorIndex == 1)//如果索引为1，输出color1
            {
                tiledRGBA32Buffer[block * 64 + i * 4] = r1; tiledRGBA32Buffer[block * 64 + i * 4 + 1] = g1; tiledRGBA32Buffer[block * 64 + i * 4 + 2] = b1; tiledRGBA32Buffer[block * 64 + i * 4 + 3] = alpha;
            }
            else if (colorIndex == 2)
            {
                tiledRGBA32Buffer[block * 64 + i * 4] = (unsigned char)((2 * r0 + r1) / 3);
                tiledRGBA32Buffer[block * 64 + i * 4 + 1] = (unsigned char)((2 * g0 + g1) / 3);
                tiledRGBA32Buffer[block * 64 + i * 4 + 2] = (unsigned char)((b0 * 2 + b1) / 3);
                tiledRGBA32Buffer[block * 64 + i * 4 + 3] = alpha;
            }
            else if (colorIndex == 3)
            {
                tiledRGBA32Buffer[block * 64 + i * 4] = (unsigned char)((r1 * 2 + r0) / 3);
                tiledRGBA32Buffer[block * 64 + i * 4 + 1] = (unsigned char)((g1 * 2 + g0) / 3);
                tiledRGBA32Buffer[block * 64 + i * 4 + 2] = (unsigned char)((b1 * 2 + b0) / 3);
                tiledRGBA32Buffer[block * 64 + i * 4 + 3] = alpha;
            }
        }
    }
    untile(tiledRGBA32Buffer, outBuffer, width, height, 4, 4, 32);
    delete[] tiledRGBA32Buffer;
}
ARGB8888 decodeARGB1555(ARGB1555 color)
{
    ARGB8888 rgba32{};

    rgba32.R = (unsigned char)((color.R * 255 + 15) / 31);
    rgba32.G = (unsigned char)((color.G * 255 + 15) / 31);
    rgba32.B = (unsigned char)((color.B * 255 + 15) / 31);
    rgba32.A = color.A * 255;

    return rgba32;
}

ARGB8888 decodeRGB565(RGB565 color)
{
    ARGB8888 rgba32{};

    rgba32.R = (unsigned char)(color.R << 3);
    rgba32.G = (unsigned char)(color.G << 2);
    rgba32.B = (unsigned char)(color.B << 3);
    rgba32.A = 255;

    return rgba32;
}
ARGB8888 decodeARGB4444(ARGB4444 color)
{
    ARGB8888 rgba32{};

    rgba32.R = (unsigned char)(color.R << 4);
    rgba32.G = (unsigned char)(color.G << 4);
    rgba32.B = (unsigned char)(color.B << 4);
    rgba32.A = (unsigned char)(color.A << 4);

    return rgba32;
}

ARGB8888 decodeXRGB1555(ARGB1555 color)
{
    ARGB8888 rgba32{};

    rgba32.R = (unsigned char)((color.R * 255 + 15) / 31);
    rgba32.G = (unsigned char)((color.G * 255 + 15) / 31);
    rgba32.B = (unsigned char)((color.B * 255 + 15) / 31);
    rgba32.A = 255;

    return rgba32;
}

void saveTGA(char* tgaName, int width, int height, char* rgba32Buffer)
{
    TGAFILEHEADER tgaHdr{};
    memset((char*)&tgaHdr, 0, sizeof(TGAFILEHEADER));
    tgaHdr.ImageType = 2;
    tgaHdr.ISpecification.Width = width;
    tgaHdr.ISpecification.Height = height;
    tgaHdr.ISpecification.PixelDepth = 32;

    std::ofstream outTga(tgaName, std::ios::binary | std::ios::out);
    if (outTga.fail())
    {
        std::cout << "fail create: " << tgaName << std::endl;
    }
    std::cout << tgaName << std::endl;
    outTga.write((char*)&tgaHdr, sizeof(TGAFILEHEADER));
    outTga.write(rgba32Buffer, width * height * 4);
    outTga.close();
}

void decodeRawPal(char* pixel, char* palette, char* rgba32Buffer, int width, int height, int bpp, std::string palFormat)
{
    int lineSize = width * 4;
    if (bpp == 4)
    {        
        for (int h = 0; h < height; h++)
        {
            unsigned int baseOffset = lineSize * (height - h - 1);
            for (int w = 0; w < width; w++)
            {
                unsigned char pix = pixel[(width * h + w) / 2];
                unsigned char index = 0;
                if (((width * h + w) % 2) == 0)
                {
                    index = pix & 0xf;
                }
                else {
                    index = (pix >> 4) & 0xf;
                }
                char* dstPtr = rgba32Buffer + baseOffset + w * 4;
                ARGB8888 color{};
                if (palFormat == "DSRGBA5551")
                {
                    color = decodePal("R5G5B5P1", palette, index);
                    unsigned char alpha = 255;
                    if (index == 0) alpha = 0;
                    color.A = alpha;
                }
                else
                {
                    color = decodePal(palFormat, palette, index);
                }
                //ARGB8888 color = decodePal(palFormat, palette, index);

                char* srcPtr = (char*)(&color);
                memcpy_s(dstPtr, 4, srcPtr, 4);

            }
        }
    }
    else if (bpp == 8)
    {
        for (int h = 0; h < height; h++)
        {
            unsigned int baseOffset = lineSize * (height - h - 1);
            for (int w = 0; w < width; w++)
            {
                unsigned char index = pixel[width * h + w];

                char* dstPtr = rgba32Buffer + baseOffset + w * 4;
                //char* dstPtr = rgba32Buffer + (width*h+w) * 4;
                ARGB8888 color{};
                if (palFormat == "R5G5B5A3")
                {
                    char colorIndex = index & 0x1f;
                    unsigned char alpha = index >> 5;
                    alpha = (alpha << 2) | (alpha >> 1);
                    alpha = (alpha << 3) | (alpha >> 2);
                    color = decodePal("R5G5B5P1", palette, colorIndex);
                    color.A = alpha;
                    
                }
                else if (palFormat == "DSRGBA5551")
                {
                    color = decodePal("R5G5B5P1", palette, index);
                    unsigned char alpha = 255;
                    if (index == 0) alpha = 0;
                    color.A = alpha;
                }
                else 
                {
                    color = decodePal(palFormat, palette, index);
                }
                
                char* srcPtr = (char*)(&color);
                memcpy_s(dstPtr, 4, srcPtr, 4);

            }
        }
    }
}
ARGB8888 decodePal(std::string palFormat,char* palette,unsigned int index)
{
    ARGB8888 color{};
    if (palFormat == "R8G8B8A8")
    {
        color.B = *(char*)(palette + index * 4 + 2);
        color.G = *(char*)(palette + index * 4 + 1);
        color.R = *(char*)(palette + index * 4);
        color.A = *(char*)(palette + index * 4 + 3);
    }
    else if (palFormat == "R5G5B5A1")
    {
        ARGB1555 color16 = *(ARGB1555*)(palette + index * 2);
        color = decodeARGB1555(color16);
    }
    else if (palFormat == "R5G5B5P1")
    {
        ARGB1555 color16 = *(ARGB1555*)(palette + index * 2);
        color = decodeXRGB1555(color16);
    }
    else if (palFormat == "R4G4B4A4")
    {
        ARGB4444 color16 = *(ARGB4444*)(palette + index * 2);
        color = decodeARGB4444(color16);
    }
    return color;
}

void decodeRaw(char* pixel, char* rgba32Buffer, int width, int height, int bpp,std::string pixelFormat)
{
    int lineSize = width * 4;
    for (int h = 0; h < height; h++)
    {
        unsigned int baseOffset = lineSize * (height - h - 1);
        for (int w = 0; w < width; w++)
        {
            unsigned int index = width * h + w;

            char* dstPtr = rgba32Buffer + baseOffset + w * 4;

            ARGB8888 color = decodePal(pixelFormat, pixel, index);

            char* srcPtr = (char*)(&color);
            memcpy_s(dstPtr, 4, srcPtr, 4);
        }
    }
}