#pragma once
#include <string>

#pragma  pack(push,1)
struct rbhHeader {
	char	PIFF[4];
	unsigned int	piffSize;
	char	RBHF[4];
};

typedef struct {
    int		unk;
	unsigned int	size;
    short	unk2; // 2
    short	type; // 4 , 16
}RBHHBodyInfo;

struct rbhChunk {
	unsigned int	chunkType;
	unsigned int	chunkSize;
};
struct TexEntry{
    short   unk;
    short   texNameID;
    unsigned int   pixelInfoOffset;
    unsigned int   paletteInfoOffset;
};

struct TexInfo{
    // PSP Tile Size:
    // 4bpp 32x8, 8bpp 16x8, 16bpp 8x8, 32bpp 4x8
    unsigned char   imageType;  // BitsPerPixel, Depth type. 
                                // PSP 3=32bpp, 4=4bpp index, 5=8bpp index, 0x88 = DXT1, 0x89 = DXT3, 0x8A = DXT5
                                // DS 1 = A3I5 Translucent 32 color, 3 = 4bpp index 16 color, 4 = 8bpp 256 color, 7=16bpp direct color
    unsigned char   widthPower;         // width = 1 << widthPower or 2^widthPower
    unsigned char   heightPower;        // 0 = palette chunk;
    unsigned char   DSAlphaFlag;        //0x64
    unsigned int    offset;
    short   dummyWidth;
    short   stride;
};

#pragma  pack(pop)


enum PSPImageType
{
    RGBA16 = 1,
    RGBA32 = 3,
    Index4bpp,
    Index8bpp,
    DXT1 = 0x88,
    DXT3 = 0x89,
    DXT5 = 0x8A
};
enum DSImageType
{
    DSA3I5Translucent = 1,
    DSIndex4bpp = 3,
    DSIndex8bpp = 4,
    DSRGBA16 = 7,
};


void readRBH(std::string rbhName);