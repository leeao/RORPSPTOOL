#include <string>
#include <fstream>
#include <iostream>

#include "rbh.h"
#include "PACK.h"
#include "Image.h"
#include "Path.h"
#include <vector>
void readRBH(std::string rbhName)
{
    std::string tmpTname = rbhName;
    for (int i = 0; i < tmpTname.size(); i++)
        tmpTname[i] = tolower(tmpTname[i]);

    int haveMesh = tmpTname.find(".prop");
    int haveMapMesh = tmpTname.find(".world");
    int haveTex = tmpTname.find(".textures");
    if (haveTex < 0) haveTex = tmpTname.find(".vram"); // Font
    if (haveTex < 0) haveTex = tmpTname.find(".skin"); // DS skin

	std::ifstream rbh(rbhName, std::ios::binary | std::ios::in);

	rbhHeader hdr{};
	rbh.read((char*)&hdr, sizeof(hdr));

    unsigned int rbhSize = hdr.piffSize - 4;
    unsigned int endOffset = (unsigned int)rbh.tellg() + rbhSize;
	
    unsigned int backOffset = rbh.tellg();
    unsigned int tempOffset = 0;
    bool havePack = false;
    while (rbh.tellg() < endOffset)
    {
        rbhChunk chunk{};
        rbh.read((char*)&chunk, sizeof(rbhChunk));
        tempOffset = (unsigned int)rbh.tellg() + chunk.chunkSize;

        if ((chunk.chunkType == 'KCAP') || (chunk.chunkType == 'BDKP') || (chunk.chunkType == 'WDKP') || (chunk.chunkType == 'LDKP'))
        {
            havePack = true;
            break;
        }

        rbh.seekg(tempOffset, std::ios::beg);
    }
    rbh.seekg(backOffset, std::ios::beg);
    
    if (havePack == false)
    {
        
        std::cout << "no pack file input." << std::endl;
        //rbh.close();
        //return;
    }

    std::string platform = "PSP";

    std::vector<char*> vecTmpBuffer;


    char outFileName[512];    
    std::string ofname = GetFileNameAddPathWithouExt(rbhName);
    sprintf_s(outFileName, "%s_decmp.rbh", ofname.c_str());
    std::ofstream deRbh;

    std::string curTypeFileName = ofname;
    std::fstream deOut;
    if (havePack == true) 
    {
        deRbh.open(outFileName, std::ios::binary | std::ios::out);
        //std::ofstream deRbh(outFileName, std::ios::binary | std::ios::out);
        deRbh.write((char*)&hdr, sizeof(hdr));

        deOut.open(curTypeFileName.c_str(), std::ios::binary | std::ios::out | std::ios::in|std::ios::trunc);
        if (deOut.fail())
        {
            std::cout << "fail create: " << curTypeFileName << std::endl;
        }
        deOut.write((char*)&hdr, sizeof(hdr));
    }



    rbhChunk rbhhChunk{};
    rbh.read((char*)&rbhhChunk, sizeof(rbhChunk));

    char* rbhhDatas = new char[rbhhChunk.chunkSize];
    rbh.read(rbhhDatas, rbhhChunk.chunkSize);
    if (havePack == true)
    {
        deRbh.write((char*)&rbhhChunk, sizeof(rbhhChunk));
        deRbh.write(rbhhDatas, rbhhChunk.chunkSize);
        deOut.write((char*)&rbhhChunk, sizeof(rbhhChunk));
        deOut.write(rbhhDatas, rbhhChunk.chunkSize);
    }


    int numBodys = rbhhChunk.chunkSize / 12;

    RBHHBodyInfo* bodyInfo = (RBHHBodyInfo*)rbhhDatas;

    if ((bodyInfo->type == 4) && (haveTex > 0))
    {
        platform = "DS";
    }

    unsigned int rbhhEndOfs = rbh.tellg();
    unsigned int* bodyDataOffsets = new unsigned int[numBodys];
    unsigned int bodyOffset = rbhhEndOfs + 8;
    bodyDataOffsets[0] = bodyOffset;
    for (int i = 1; i < numBodys; i++)
    {
        RBHHBodyInfo info = *(bodyInfo + i-1);
        bodyOffset += info.size + 8;
        bodyDataOffsets[i] = bodyOffset;

    }

    int index = 0;
    for(int i = 0; i < numBodys; i++)
    {
        unsigned int bodySize = bodyInfo[i].size;
        rbhChunk chunk{};
        rbh.read((char*)&chunk, sizeof(rbhChunk));
        tempOffset = (unsigned int)rbh.tellg() + chunk.chunkSize;

        char* tempBuffer = new char[bodySize];

        if ((chunk.chunkType == 'KCAP') || (chunk.chunkType == 'BDKP') || (chunk.chunkType == 'WDKP') || (chunk.chunkType == 'LDKP'))
        {
            
            unsigned short decmpSize = 0;
            unsigned short cmpSize = 0;

            rbhChunk newChunk{};
            newChunk.chunkType = 'YDOB';
            newChunk.chunkSize = bodySize;
            if (havePack == true) deRbh.write((char*)&newChunk, sizeof(newChunk));
            if (havePack == true) deOut.write((char*)&newChunk, sizeof(newChunk));
            char* decmpOutBuffer = new char[bodySize];
            memset((char*)decmpOutBuffer, 0, bodySize);
            unsigned int decmpBuffer = (unsigned int)decmpOutBuffer;
            while (rbh.read((char*)&decmpSize, 2), decmpSize != 0)
            {
                rbh.read((char*)&cmpSize, 2);
                char* cmpBuffer = new char[cmpSize];
                rbh.read((char*)cmpBuffer, cmpSize);


                RORStream cmpDataStream{};
                cmpDataStream.streamPtr = (unsigned int)cmpBuffer;
                cmpDataStream.basePos = (unsigned int)cmpBuffer;
                cmpDataStream.curPos = 0;


                int curOutBufferPtr = DecompressPACK((int*)&cmpDataStream.streamPtr, decmpBuffer, decmpSize, cmpSize);

                // delta code
                if (chunk.chunkType == 'WDKP')
                {
                    if (1 < (unsigned int)((int)curOutBufferPtr >> 1)) {
                        unsigned int uVar3 = 1;
                        int* piVar2 = (int*)decmpBuffer;
                        do {
                            uVar3 = uVar3 + 1;
                            *(short*)((int)piVar2 + 2) = *(short*)((int)piVar2 + 2) + *(short*)piVar2;
                            piVar2 = (int*)((int)piVar2 + 2);
                        } while ((int)curOutBufferPtr >> 1 != uVar3);
                    }
                }
                else if (chunk.chunkType == 'LDKP')
                {
                    if (1 < (unsigned int)((int)curOutBufferPtr >> 2)) {
                        unsigned int uVar3 = 1;
                        int* piVar2 = (int*)decmpBuffer;
                        do {
                            uVar3 = uVar3 + 1;
                            piVar2[1] = piVar2[1] + *piVar2;
                            piVar2 = piVar2 + 1;
                        } while ((int)curOutBufferPtr >> 2 != uVar3);
                    }
                }
                else if (chunk.chunkType == 'BDKP')
                {
                    if (curOutBufferPtr > 1)
                    {
                        unsigned int uVar3 = 1;
                        int* piVar2 = (int*)decmpBuffer;
                        do {
                            *(char*)((int)piVar2 + 1) = *(char*)((int)piVar2 + 1) + *(char*)piVar2;
                            if (curOutBufferPtr == uVar3 + 1) break;
                            uVar3 = uVar3 + 2;
                            *(char*)((int)piVar2 + 2) = *(char*)((int)piVar2 + 2) + *(char*)((int)piVar2 + 1);
                            piVar2 = (int*)((int)piVar2 + 2);
                        } while (curOutBufferPtr != uVar3);
                    }

                }

                decmpBuffer += decmpSize;
            }

            if (havePack == true) deRbh.write(decmpOutBuffer, bodySize);
            if (havePack == true) deOut.write(decmpOutBuffer, bodySize);
            memcpy(tempBuffer, decmpOutBuffer, bodySize);


            index++;
            delete[] decmpOutBuffer;

        }
        else
        {
            char* chunkData = new char[chunk.chunkSize];
            rbh.read(chunkData, chunk.chunkSize);
            if (havePack == true) deRbh.write((char*)&chunk, sizeof(chunk));
            if (havePack == true) deRbh.write(chunkData, chunk.chunkSize);
            if (havePack == true) deOut.write((char*)&chunk, sizeof(chunk));
            if (havePack == true) deOut.write(chunkData, chunk.chunkSize);
            memcpy(tempBuffer, chunkData, bodySize);
            delete[] chunkData;
        }

        rbh.seekg(tempOffset, std::ios::beg);

        if (haveTex > 0)
        {
            vecTmpBuffer.push_back(tempBuffer);
        }

    }

    
	while (rbh.tellg() < endOffset)
	{
		rbhChunk chunk{};
		rbh.read((char*)&chunk, sizeof(rbhChunk));
		tempOffset = (unsigned int)rbh.tellg() + chunk.chunkSize;

        char* chunkData = new char[chunk.chunkSize];
        rbh.read(chunkData, chunk.chunkSize);
        if (havePack == true)
        {
            deRbh.write((char*)&chunk, sizeof(chunk));
            deRbh.write(chunkData, chunk.chunkSize);
            deOut.write((char*)&chunk, sizeof(chunk));
            deOut.write(chunkData, chunk.chunkSize);
            if (chunk.chunkType == 'CLER')
            {
                unsigned int backOffset = deOut.tellp();
                unsigned int* relcPtr = (unsigned int*)(chunkData);
                unsigned int dstBodyIndex = *relcPtr++;
                unsigned int baseBodyIndex = *relcPtr++;
                int numOffset = (chunk.chunkSize - 8) / 4;
                deOut.clear();
                for (int i = 0; i < numOffset; i++)
                {
                    unsigned int offsetPtr = *relcPtr++;
                    deOut.seekg((bodyDataOffsets[dstBodyIndex]) + offsetPtr);
                    char* offset = new char[4];
                    deOut.read(offset, 4);
                    //printf_s("read before:0x%X,%X\n", bodyDataOffsets[dstBodyIndex] + offsetPtr, *(unsigned int*)offset);
                    *(unsigned int*)offset = *(unsigned int*)offset + bodyDataOffsets[baseBodyIndex];
                    deOut.seekg((bodyDataOffsets[dstBodyIndex]) + offsetPtr);
                    deOut.write(offset, 4);
                    //printf_s("read after:0x%X,%X\n", bodyDataOffsets[dstBodyIndex] + offsetPtr, *(unsigned int*)offset);

                }
                deOut.seekp(backOffset, std::ios::beg);
            }

        }
       
		rbh.seekg(tempOffset, std::ios::beg);
	}
    rbh.close();
    if (havePack == true)
    {
        unsigned int deRbhFileSize = (unsigned int)deRbh.tellp() - 8;
        deRbh.seekp(4, std::ios::beg);
        deRbh.write((char*)&deRbhFileSize, 4);

        deRbh.close();
        std::cout << outFileName << std::endl;
        
        deOut.clear();
        deOut.seekp(0, std::ios::end);
        //std::cout << deOut.tellp() << std::endl;
        //std::cout << deOut.tellg() << std::endl;
        unsigned int deOutFileSize = (unsigned int)deOut.tellp() - 8; 
        deOut.seekp(4, std::ios::beg);
        deOut.write((char*)&deOutFileSize, 4);

        deOut.close();
        std::cout << curTypeFileName << std::endl;
    }


   
    
    if (haveTex > 0)
    {
        char* texBody = vecTmpBuffer[0];
        char* texNameBody = vecTmpBuffer[1];
        char* texDataBody = nullptr;
        if ((platform == "DS") && (numBodys > 2)) texDataBody = vecTmpBuffer[2];
        
        // Read Texture Names
        int numNames = 0;
        unsigned int nameOffset = 0;
        unsigned int texNameBodyOffset = (unsigned int)texNameBody;
        do {
            nameOffset = *(unsigned int*)texNameBodyOffset;
            texNameBodyOffset += 4;
            if (nameOffset > 0) numNames++;
        } while (nameOffset != 0);
        
        std::string *texNames = new std::string[numNames];

        for (int i = 0; i < numNames; i++)
        {
            unsigned int strPtr = *(unsigned int*)(texNameBody + i * 4) + (unsigned int)texNameBody;            
            texNames[i] = (char*)strPtr;
            //std::cout << texNames[i] << std::endl;
        }
        
        // Read Texture Data Body
        int numTex = *(int*)texBody;
        TexEntry* texEntry = (TexEntry*)(texBody + 4);

        unsigned int dataBaseOffset = (unsigned int)texBody;
        if (platform == "DS") dataBaseOffset = (unsigned int)texDataBody;
        int haveUnsupportformat = false;
        for (int i = 0; i < numTex; i++)
        {           
            TexEntry curTexEntry = texEntry[i];
            std::string texName = texNames[curTexEntry.texNameID];
            TexInfo pixelInfo = *(TexInfo*)(texBody + curTexEntry.pixelInfoOffset);
            TexInfo paletteInfo{};
            if (curTexEntry.paletteInfoOffset > 0)
            {
                paletteInfo = *(TexInfo*)(texBody + curTexEntry.paletteInfoOffset);
            }
            unsigned int width = 1 << pixelInfo.widthPower;
            unsigned int height = 1 << pixelInfo.heightPower;

            char log[512];
            std::string path = GetDirectory(rbhName);
            sprintf_s(log, "%s\\%s.tga", path.c_str(), texName.c_str());

            if (platform == "PSP")
            {
                if (pixelInfo.imageType == Index4bpp)
                {
                    int depth = 4;
                    int _width = getStorageWidth(pixelInfo.stride, 4);

                    unsigned int size = _width * height / 2;
                    char* pixel = (char*)(dataBaseOffset + pixelInfo.offset);
                    char* untileBuffer = new char[size];
                    if ((_width > 31) && (height > 7))
                        untile(pixel, untileBuffer, _width, height, 32, 8, 4);
                    else untileBuffer = pixel;
                    char* rgba32Buffer = new char[_width * height * 4];
                    char* pal = (char*)(dataBaseOffset + paletteInfo.offset);
                    if (paletteInfo.imageType == RGBA32)
                    {
                        decodeRawPal(untileBuffer, pal, rgba32Buffer, _width, height, depth, "R8G8B8A8");
                    }
                    if (width == _width)
                    {
                        saveTGA(log, width, height, rgba32Buffer);
                    }
                    else
                    {
                       char* cropRgba32Buffer = new char[width * height * 4];
                       crop(width, height, _width, height, 32, rgba32Buffer, cropRgba32Buffer);
                       saveTGA(log, width, height, cropRgba32Buffer);
                    }
                    
                }
                else if (pixelInfo.imageType == Index8bpp)
                {
                    int depth = 8;
                    int _width = getStorageWidth(pixelInfo.stride, 8);
                    unsigned int size = _width * height ;
                    char* pixel = (char*)(dataBaseOffset + pixelInfo.offset);
                    char* untileBuffer = new char[size];
                    if ((_width > 15) && (height > 7))
                        untile(pixel, untileBuffer, _width, height, 16, 8, 8);
                    else untileBuffer = pixel;
                    char* rgba32Buffer = new char[_width * height * 4];
                    char* pal = (char*)(dataBaseOffset + paletteInfo.offset);
                    if (paletteInfo.imageType == RGBA32)
                    {
                        decodeRawPal(untileBuffer, pal, rgba32Buffer, _width, height, depth, "R8G8B8A8");
                    }
                    if (width == _width)
                    {
                        saveTGA(log, width, height, rgba32Buffer);
                    }
                    else
                    {
                        char* cropRgba32Buffer = new char[width * height * 4];
                        crop(width, height, _width, height, 32, rgba32Buffer, cropRgba32Buffer);
                        saveTGA(log, width, height, cropRgba32Buffer);
                    }
                }
                else if (pixelInfo.imageType == RGBA32)
                {
                    int depth = 32;
                    int _width = getStorageWidth(pixelInfo.stride, 32);
                    unsigned int size = _width * height * 4;
                    char* pixel = (char*)(dataBaseOffset + pixelInfo.offset);
                    char* untileBuffer = new char[size];
                    untile(pixel, untileBuffer, _width, height, 4, 8, 32);
                    char* rgba32Buffer = new char[_width * height * 4];
                    decodeRaw(untileBuffer, rgba32Buffer, _width, height, depth, "R8G8B8A8");
                    if (width == _width)
                    {
                        saveTGA(log, width, height, rgba32Buffer);
                    }
                    else
                    {
                        char* cropRgba32Buffer = new char[width * height * 4];
                        crop(width, height, _width, height, 32, rgba32Buffer, cropRgba32Buffer);
                        saveTGA(log, width, height, cropRgba32Buffer);
                    }
                }
                else if (pixelInfo.imageType == DXT1) 
                {
                    int depth = 4;
                    int _width = getStorageWidth(pixelInfo.stride, 4);
                    unsigned int size = _width * height / 2;
                    char* pixel = (char*)(dataBaseOffset + pixelInfo.offset);
                    char* untileRgba32Buffer = new char[_width * height * 4];
                    decodeRORDXT1(pixel, untileRgba32Buffer, _width, height, true);
                    char* rgba32Buffer = new char[_width * height * 4];
                    decodeRaw(untileRgba32Buffer, rgba32Buffer, _width, height, 32, "R8G8B8A8");
                    //sprintf_s(log, "%s\\%s_dxt1.tga", path.c_str(), texName.c_str());
                    if (width == _width)
                    {
                        saveTGA(log, width, height, rgba32Buffer);
                    }
                    else
                    {
                        char* cropRgba32Buffer = new char[width * height * 4];
                        crop(width, height, _width, height, 32, rgba32Buffer, cropRgba32Buffer);
                        saveTGA(log, width, height, cropRgba32Buffer);
                    }
                }
                else if (pixelInfo.imageType == DXT3)
                {
                    int depth = 8;
                    int _width = getStorageWidth(pixelInfo.stride, 8);
                    unsigned int size = _width * height;
                    char* pixel = (char*)(dataBaseOffset + pixelInfo.offset);
                    char* untileRgba32Buffer = new char[_width * height * 4];
                    decodeRORDXT3(pixel, untileRgba32Buffer, _width, height);
                    char* rgba32Buffer = new char[_width * height * 4];
                    decodeRaw(untileRgba32Buffer, rgba32Buffer, _width, height, 32, "R8G8B8A8");
                    //sprintf_s(log, "%s\\%s_dxt3.tga", path.c_str(), texName.c_str());
                    if (width == _width)
                    {
                        saveTGA(log, width, height, rgba32Buffer);
                    }
                    else
                    {
                        char* cropRgba32Buffer = new char[width * height * 4];
                        crop(width, height, _width, height, 32, rgba32Buffer, cropRgba32Buffer);
                        saveTGA(log, width, height, cropRgba32Buffer);
                    }
                }
                else if (pixelInfo.imageType == DXT5)
                {
                    int depth = 8;
                    int _width = getStorageWidth(pixelInfo.stride, 8);
                    unsigned int size = _width * height;
                    char* pixel = (char*)(dataBaseOffset + pixelInfo.offset);
                    char* untileRgba32Buffer = new char[_width * height * 4];
                    decodeRORDXT5(pixel, untileRgba32Buffer, _width, height);
                    char* rgba32Buffer = new char[_width * height * 4];
                    decodeRaw(untileRgba32Buffer, rgba32Buffer, _width, height, 32, "R8G8B8A8");
                    //sprintf_s(log, "%s\\%s_dxt5.tga", path.c_str(), texName.c_str());
                    if (width == _width)
                    {
                        saveTGA(log, width, height, rgba32Buffer);
                    }
                    else
                    {
                        char* cropRgba32Buffer = new char[width * height * 4];
                        crop(width, height, _width, height, 32, rgba32Buffer, cropRgba32Buffer);
                        saveTGA(log, width, height, cropRgba32Buffer);
                    }
                }
                else if (pixelInfo.imageType == RGBA16)
                {
                    int depth = 16;
                    int _width = getStorageWidth(pixelInfo.stride, 16);
                    unsigned int size = _width * height * 2;
                    char* pixel = (char*)(dataBaseOffset + pixelInfo.offset);
                    char* untileBuffer = new char[size];
                    untile(pixel, untileBuffer, _width, height, 8, 8, 16);
                    char* rgba32Buffer = new char[_width * height * 4];
                    decodeRaw(untileBuffer, rgba32Buffer, _width, height, depth, "R5G5B5A1");
                    if (width == _width)
                    {
                        saveTGA(log, width, height, rgba32Buffer);
                    }
                    else
                    {
                        char* cropRgba32Buffer = new char[width * height * 4];
                        crop(width, height, _width, height, 32, rgba32Buffer, cropRgba32Buffer);
                        saveTGA(log, width, height, cropRgba32Buffer);
                    }
                }
                else
                {
                    printf_s("can't support format: %d,TexName:%s. TexID:%d\n", pixelInfo.imageType,texName.c_str(),i);
                    haveUnsupportformat = true;
                }
            }
            else if (platform == "DS") 
            {
                if (pixelInfo.imageType == DSIndex4bpp)
                {
                    int depth = 4;
                    unsigned int size = width * height / 2;
                    char* pixel = (char*)(dataBaseOffset + pixelInfo.offset);
                    char* rgba32Buffer = new char[width * height * 4];
                    char* pal = (char*)(dataBaseOffset + paletteInfo.offset);

                    if (paletteInfo.imageType == DSRGBA16)
                    {
                        if (pixelInfo.DSAlphaFlag > 0)
                        {
                            decodeRawPal(pixel, pal, rgba32Buffer, width, height, depth, "DSRGBA5551");
                        }
                        else
                        {
                            decodeRawPal(pixel, pal, rgba32Buffer, width, height, depth, "R5G5B5P1");
                        }
                        
                    }
                    
                    saveTGA(log, width, height, rgba32Buffer);
                }
                else if (pixelInfo.imageType == DSA3I5Translucent)
                {
                    int depth = 8;
                    unsigned int size = width * height;
                    char* pixel = (char*)(dataBaseOffset + pixelInfo.offset);
                    char* rgba32Buffer = new char[width * height * 4];
                    char* pal = (char*)(dataBaseOffset + paletteInfo.offset);
                    if (paletteInfo.imageType == DSRGBA16)
                    {
                        decodeRawPal(pixel, pal, rgba32Buffer, width, height, depth, "R5G5B5A3");
                    }
                    saveTGA(log, width, height, rgba32Buffer);
                }
                else if (pixelInfo.imageType == DSIndex8bpp)
                {
                    int depth = 8;
                    unsigned int size = width * height;
                    char* pixel = (char*)(dataBaseOffset + pixelInfo.offset);
                    char* rgba32Buffer = new char[width * height * 4];
                    char* pal = (char*)(dataBaseOffset + paletteInfo.offset);
                    if (paletteInfo.imageType == DSRGBA16)
                    {
                        if (pixelInfo.DSAlphaFlag > 0)
                        {
                            decodeRawPal(pixel, pal, rgba32Buffer, width, height, depth, "DSRGBA5551");
                        }
                        else
                        {
                            decodeRawPal(pixel, pal, rgba32Buffer, width, height, depth, "R5G5B5P1");
                        }
                        
                    }
                    saveTGA(log, width, height, rgba32Buffer);
                }
                else if (pixelInfo.imageType == DSRGBA16)
                {
                    int depth = 16;
                    unsigned int size = width * height * 2;
                    char* pixel = (char*)(dataBaseOffset + pixelInfo.offset);
                    char* rgba32Buffer = new char[width * height * 4];
                    decodeRaw(pixel, rgba32Buffer, width, height, depth, "R5G5B5A1");
                    saveTGA(log, width, height, rgba32Buffer);
                }
                else
                {
                    printf_s("can't support format: %d,TexName:%s. TexID:%d\n", pixelInfo.imageType, texName.c_str(), i);
                    haveUnsupportformat = true;
                }
            }




        }

        if (haveUnsupportformat)
        {
            std::cout << "\nPlease check some unsupported image formats.\nPress Enter key to continue\n" << std::endl;
            std::cin.get();

        }
    }

}