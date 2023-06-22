// rorpsptool.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ostream>
#include <vector>
#include <string>
#include <algorithm>
#include <string.h>
#include "MifTypes.h"
#include "stdlib.h"
#include "PACK.h"
#include "rbh.h"
#include "Path.h"

using namespace std;
void printHelp()
{    
    printf(" \nTool Usage Help:\n\n");
    printf(" Dragging the rbh or mif file into the program will auto complete the unpack.\n\n");
    printf(" You can also use the following commands:\n");
    printf(" <rorpsptool.exe -o \"*.mif\" \"Output directory - Optional\" >");
    printf(" Unpack mif file. Output directory is optional.\n\n");
    printf(" <rorpsptool.exe -d \"*.rbh\">");
    printf(" Decompress the PACK compressed datas inside the rbh file.\n\n");

    printf(" <rorpsptool.exe -da <\"input directory\">");
    printf(" Decompress all compressed Rbh files in the directory, including subfolders.\n\n");

    printf(" <rorpsptool.exe -dp \"*.PACK file\">");
    printf(" Decompress PACK/PKDW/PKDL/PKDB compressed file.\n\n");
    /*
    printf(" <rorpsptool.exe -h \"*.txt\" \"*.mif\">");
    printf(" Get the file name from txt that matches the HASH in the mif file.\n");
    printf(" For example: <rorpsptool.exe -h \"D:\\filenames.txt\" \"D:\\Game.mif\"> \n\n");
   
    printf(" <rorpsptool.exe -f \"*.txt\"");
    printf(" Remove duplicate file names from txt files and get unique file names.\n");
    printf(" For example: <rorpsptool.exe -t \"D:\\filenames.txt\"\n\n");
    */
    printf("Tools by Allen.\n");
}
void printArgs(int argc, char* args[])
{
    for (int i = 0; i < argc; i++)
    {
        printf("%s\n", args[i]);
    }
}
int main(int argc, char* argv[])
{
    //printArgs(argc, argv);
    char logBuffer[512];
    if (argc == 1)
    {
        printHelp();
        
    }
    else 
    {
        if (strcmp(argv[1], "-o") == 0)
        {
            if (argc == 3)
            {
                string path = GetDirectory(argv[2]);

                sprintf_s(logBuffer, "%s\\unpack\\", path.c_str());
                unpack(argv[2], logBuffer);
            }
            if (argc == 4)
            {
                unpack(argv[2], argv[3]);
            }
        }
        else if ((strcmp(argv[1], "-d") == 0) && (argc == 3))
        {
            readRBH(argv[2]);
        }
        else if ((strcmp(argv[1], "-dp") == 0) && (argc == 3))
        {
            decmpPACK(argv[2]);
        }
        else if ((strcmp(argv[1], "-da") == 0) && (argc == 3))
        {
            vector<string> allRbhNames;
            
            getAllFiles(argv[2], allRbhNames, ".rbh");
            for (int i=0;i<allRbhNames.size();i++)
            {
                std::cout << allRbhNames[i] << std::endl;
                readRBH(allRbhNames[i]);
            }
            
        }
        else if ((strcmp(argv[1], "-h") == 0) && (argc == 4))
        {
            getMatchHashFileNames(argv[2], argv[3]);
        }
        else if ((strcmp(argv[1], "-f") == 0) && (argc == 3))
        {
            getUnityHashFileNames(argv[2]);
        }
        else if (argc > 1)
        {
            for (int i = 1; i < argc; i++)
            {
                //string name = argv[i];
                std::string name = argv[i];
                for (int i = 0; i < name.size(); i++)
                    name[i] = tolower(name[i]);

                if (CheckFileReadable(name.c_str()))
                {
                    string ext = GetExtName(name);
                    if (ext == "rbh")
                    {
                        readRBH(name);
                    }
                    else if (ext == "mif")
                    {
                        string path = GetDirectory(name);

                        sprintf_s(logBuffer, "%s\\unpack\\", path.c_str());
                        unpack((char*)name.c_str(), logBuffer);
                    }
                    else if ((ext == "pack") ||(ext == "pkdw") ||(ext == "pkdl") || (ext == "pkdb"))
                    {
                        decmpPACK((char*)name.c_str());
                    }
                }
            }
        }
    }

    
    return 1;
    
}
void decmpPACK(char* cmpName)
{
    ifstream cmpFile(cmpName, ios::binary | ios::in);
    if (cmpFile.fail())
    {
        cout << "fail open: " << cmpName << endl;
    }
    rbhChunk chunk{};
    cmpFile.read((char*)&chunk, sizeof(rbhChunk));
    streampos curPos = cmpFile.tellg();
    unsigned int bodySize = 0;
    
    if ((chunk.chunkType == 'KCAP') || (chunk.chunkType == 'BDKP') || (chunk.chunkType == 'WDKP') || (chunk.chunkType == 'LDKP'))
    {

        unsigned short decmpSize = 0;
        unsigned short cmpSize = 0;
        while (cmpFile.read((char*)&decmpSize, 2), decmpSize != 0)
        {
            bodySize += decmpSize;
            cmpFile.read((char*)&cmpSize, 2);
            cmpFile.seekg(cmpSize, ios::cur);
        }
    }
    cmpFile.seekg(curPos, ios::beg);

    if ((chunk.chunkType == 'KCAP') || (chunk.chunkType == 'BDKP') || (chunk.chunkType == 'WDKP') || (chunk.chunkType == 'LDKP'))
    {
        char logBuffer[512];
        sprintf_s(logBuffer, "%s_decmp", cmpName);
        ofstream out(logBuffer, ios::binary | ios::out);
        if (out.fail())
        {
            cout << "fail create: " << logBuffer << endl;
        }

        unsigned short decmpSize = 0;
        unsigned short cmpSize = 0;

        rbhChunk newChunk{};
        newChunk.chunkType = 'YDOB';
        newChunk.chunkSize = bodySize;
        out.write((char*)&newChunk, sizeof(newChunk));

        char* decmpOutBuffer = new char[bodySize];
        memset((char*)decmpOutBuffer, 0, bodySize);
        unsigned int decmpBuffer = (unsigned int)decmpOutBuffer;
        while (cmpFile.read((char*)&decmpSize, 2), decmpSize != 0)
        {
            cmpFile.read((char*)&cmpSize, 2);
            char* cmpBuffer = new char[cmpSize];
            cmpFile.read((char*)cmpBuffer, cmpSize);


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

        out.write(decmpOutBuffer, bodySize);
        
        out.close();
        std::cout << logBuffer << std::endl;
        
        delete[] decmpOutBuffer;
    }

    cmpFile.close();
    
    



}
