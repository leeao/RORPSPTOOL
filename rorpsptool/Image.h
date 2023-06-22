#pragma once
#include <string>

#pragma  pack(push,1)
typedef struct {
	unsigned short   FirstIndexEntry;
	unsigned short   ColorMapLength;
	unsigned char  ColorMapEntrySize;
}COLORMAPSPECIFICATION;


typedef struct {
	unsigned short   XOrigin;
	unsigned short   YOrigin;
	unsigned short   Width;
	unsigned short   Height;
	unsigned char  PixelDepth;
	unsigned char  ImageDescriptor;
}IMAGESPECIFICATION;


typedef struct {
	unsigned char  IDLength;
	unsigned char  ColorMapType;
	unsigned char  ImageType;
	COLORMAPSPECIFICATION CMSpecification;
	IMAGESPECIFICATION ISpecification;
}TGAFILEHEADER;

typedef struct {
	unsigned int B : 5;
	unsigned int G : 5;
	unsigned int R : 5;
	unsigned int X : 1;
}XRGB1555;


typedef struct {
	unsigned char B;
	unsigned char G;
	unsigned char R;
}RGB888;


typedef struct {
	unsigned char B;
	unsigned char G;
	unsigned char R;
	unsigned char A;
}ARGB8888;

typedef struct {
	unsigned short R : 5;
	unsigned short G : 5;
	unsigned short B : 5;
	unsigned short A : 1;
}ARGB1555;
typedef struct {
	unsigned short R : 5;
	unsigned short G : 6;
	unsigned short B : 5;
}RGB565;
typedef struct {
	unsigned short R : 4;
	unsigned short G : 4;
	unsigned short B : 4;
	unsigned short A : 4;
}ARGB4444;

#pragma  pack(pop)


void untile(char* pixel, char* outBuffer, int width, int height, int tile_w, int tile_h, int bpp);
void crop(int width, int height, int storageWidth, int storageHeight, int bitsPerPixel, char* srcBuffer, char* dstBuffer);
int getStorageWidth(int stride, int bitsPerPixel);
void decodeRORDXT1(char* data, char* outBuffer, int width, int height, bool alphaSetting);
void decodeRORDXT3(char* data, char* outBuffer, int width, int height);
void decodeRORDXT5(char* data, char* outBuffer, int width, int height);
ARGB8888 decodeARGB1555(ARGB1555 color);
ARGB8888 decodeRGB565(RGB565 color);
ARGB8888 decodeARGB4444(ARGB4444 color);
ARGB8888 decodeXRGB1555(ARGB1555 color);
void saveTGA(char* tgaName, int width, int height, char* rgba32Buffer);
void decodeRawPal(char* pixel, char* palette, char* rgba32Buffer, int width, int height, int bpp, std::string palFormat);
ARGB8888 decodePal(std::string palFormat, char* palette, unsigned int index);
void decodeRaw(char* pixel, char* rgba32Buffer, int width, int height, int bpp, std::string pixelFormat);