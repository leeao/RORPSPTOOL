#include "MifTypes.h"
#include "Path.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>



using namespace std;
void getUnityHashFileNames(char* txtName)
{

    ifstream listFile(txtName, ios::in);
    vector<string> fileNameList;
    if (listFile.fail())
    {
        std::cout << "can't found "<< txtName <<" file!\n";
        return;
    }
    string buf;
    while (getline(listFile, buf))
    {
        fileNameList.push_back(buf);
    }
    listFile.close();

    //将数据进行排序
    sort(fileNameList.begin(), fileNameList.end());
    //将重复的数据移到后面
    vector<string>::iterator ite = unique(fileNameList.begin(), fileNameList.end());
    //删除重复的元素
    fileNameList.erase(ite, fileNameList.end());

    //std::cout << "count:" << fileNameList.size() << endl;
    char outFileName[512];
    //char* nameWithoutExt = txtName;
    string  nameWithoutExt = GetFileNameAddPathWithouExt(txtName);
    sprintf_s(outFileName, "%s_unity.txt", nameWithoutExt.c_str());


    ofstream out(outFileName, ios::out);
    char logbuffer[512];
    sort(fileNameList.begin(), fileNameList.end(), [](string a, string b) {return a < b; });
    for (unsigned int i = 0; i < (unsigned int)fileNameList.size(); i++)
    {
        sprintf_s(logbuffer, "%s\n", fileNameList[i].c_str());
        out.write(logbuffer, strlen(logbuffer));
    }
    out.close();
    cout << outFileName << endl;
}
void unpack(char* mifFileName, char* outPath)
{
    string path = GetExePath();
    ifstream listFile(path + "\\filenames.txt", ios::in);
    vector<string> fileNameList;
    if (listFile.fail())
    {
        std::cout << "Can't found filenames.txt file!\n" << endl;
        //std::cout << path << endl;
        //return;//这里待优化，如果找不到就不能运行了
    }
    string buf;
    while (getline(listFile, buf))
    {
        fileNameList.push_back(buf);
    }
    listFile.close();

    //将数据进行排序
    sort(fileNameList.begin(), fileNameList.end());
    //将重复的数据移到后面
    vector<string>::iterator ite = unique(fileNameList.begin(), fileNameList.end());
    //删除重复的元素
    fileNameList.erase(ite, fileNameList.end());

    
    unsigned int* fileNameHashList = new unsigned int[fileNameList.size()]{};

    for (unsigned int i = 0; i < (unsigned int)fileNameList.size(); i++)
    {
        fileNameHashList[i] = getHash((char*)fileNameList[i].c_str());
    }
    ifstream mif1(mifFileName, ios::binary | ios::in);

    char* entryBuffer = new char[0x4800];
    char* bufPtr = entryBuffer;
    mif1.read(entryBuffer, 0x4800);
    mifHeader* mifHdr = (mifHeader*)bufPtr;
    bufPtr += 32;
    entry_s* entryList = (entry_s*)(bufPtr);

    vector<hashName_s> matchHashList;
    for (int i = 0; i < mifHdr->numEntry; i++)
    {
        entry_s* curEntry = entryList + i;
        int haveName = 0;
        char logBuffer[512];
        for (unsigned int j = 0; j < (unsigned int)fileNameList.size(); j++)
        {
            if (curEntry->hash == fileNameHashList[j])
            {
                string fname = fileNameList[j];
                while (fname.find('/') != fname.npos)
                {
                    fname = fname.replace(fname.find('/'), 1, 1, '\\');
                }
                sprintf_s(logBuffer, "%s\\%s", outPath, fname.c_str());
                createDirectory(logBuffer);
                //std::cout << logBuffer << endl;
                //std::cout <<"offset:"<< curEntry->offset * mifHdr->alignBytes << " size：" << curEntry->size << endl;
                mif1.seekg(curEntry->offset * (unsigned int)mifHdr->alignBytes, ios::beg);
                cout << logBuffer << endl;
                char* buffer = new char[curEntry->size];
                mif1.read(buffer, curEntry->size);
                ofstream out(logBuffer, ios::binary | ios::out);
                if (out.fail())
                {
                    cout << "fail create: " << logBuffer << endl;
                }
                out.write(buffer, curEntry->size);
                out.close();
                delete[] buffer;
                haveName = 1;
                break;
            }
        }
        if (haveName == 0)
        {
            sprintf_s(logBuffer, "%s\\%X", outPath, curEntry->hash);
            cout << logBuffer << endl;
            mif1.seekg(curEntry->offset * (unsigned int)mifHdr->alignBytes, ios::beg);
            char* buffer = new char[curEntry->size];
            mif1.read(buffer, curEntry->size);
            ofstream out(logBuffer, ios::binary | ios::out);
            if (out.fail())
            {
                cout << "fail create: " << logBuffer << endl;
            }
            out.write(buffer, curEntry->size);
            out.close();
        }
    }
    cout << "unpack done." << endl;
    mif1.close();
}
void createDirectory(char* fileName)
{
    std::string dir = GetDirectory(fileName);
    if (CheckFolderExist(dir.c_str()) == false)
    {

        char cmd[512];
        sprintf_s(cmd, "mkdir \"%s\"", dir.c_str());
        //cout << cmd << endl;
        int ret = system(cmd);
        if (ret)
        {
            cout << "create dir error: " << ret << ", :" << endl;
        }

    }
}


unsigned int getHash(char* fileName)
{
    char curByte = *fileName;
    int findHash = 0LL;
    unsigned char* v9;
    int v10;
    if (*fileName)
    {
        v9 = (unsigned char*)fileName;
        do
        {
            v10 = 92;
            if (curByte != 47)
            {
                v10 = (unsigned __int8)curByte;
                if ((unsigned char)(curByte - 97) < 0x1A)
                    v10 = (unsigned __int8)(curByte - 32);
            }
            curByte = (char)*++v9;
            findHash = findHash + v10 + findHash * 2;
        } while (curByte);
    }
    return (unsigned int)findHash;
    //std::cout << findHash;
}

void getMatchHashFileNames(char* nameListfile, char* mifName)
{
    //char* nameListfile = (char*)"D:\\下载\\GameRE\\Cars ROR PSP EUR mif\\fileList_psp2.txt";
    //char* mifName1 = (char*)"C:\\Cars ROR PSP EUR\\PSP_GAME\\USRDIR\\Media\\Game.mif";
    //char* mifName2 = (char*)"C:\\Cars ROR PSP EUR\\PSP_GAME\\USRDIR\\Media\\GameAudio.mif";

    ifstream listFile(nameListfile, ios::in);
    vector<string> fileNameList;
    if (listFile.fail())
    {
        std::cout << "can't found fileLsit.txt name file!\n";
        return;
    }
    string buf;
    while (getline(listFile, buf))
    {
        fileNameList.push_back(buf);
    }
    listFile.close();

    //将数据进行排序
    sort(fileNameList.begin(), fileNameList.end());
    //将重复的数据移到后面
    vector<string>::iterator ite = unique(fileNameList.begin(), fileNameList.end());
    //删除重复的元素
    fileNameList.erase(ite, fileNameList.end());

    //std::cout << "count:" << fileNameList.size() <<endl;
    unsigned int* fileNameHashList = new unsigned int[fileNameList.size()]{};

    for (unsigned int i = 0; i < (unsigned int)fileNameList.size(); i++)
    {
        fileNameHashList[i] = getHash((char*)fileNameList[i].c_str());
    }

    ifstream mif1(mifName, ios::binary | ios::in);

    char* entryBuffer = new char[0x4800];
    char* bufPtr = entryBuffer;
    mif1.read(entryBuffer, 0x4800);
    mifHeader* mifHdr = (mifHeader*)bufPtr;
    bufPtr += 32;
    entry_s* entryList = (entry_s*)(bufPtr);

    vector<hashName_s> matchHashList;
    for (int i = 0; i < mifHdr->numEntry; i++)
    {
        entry_s* curEntry = entryList + i;
        for (unsigned int j = 0; j < (unsigned int)fileNameList.size(); j++)
        {
            if (curEntry->hash == fileNameHashList[j])
            {
                hashName_s hashName;
                sprintf_s(hashName.fileName, "%s", fileNameList[j].c_str());
                hashName.hash = curEntry->hash;
                matchHashList.push_back(hashName);
                break;
            }
        }

    }
    mif1.close();
    /*
    ifstream mif2(mifName2, ios::binary | ios::in);

    char* entryBuffer2 = new char[0x4800];
    bufPtr = entryBuffer2;
    mif2.read(entryBuffer2, 0x4800);

    mifHdr = (mifHeader*)bufPtr;
    bufPtr += 32;
    entryList = (entry_s*)(bufPtr);

    vector<hashName_s> matchHashList2;
    for (int i = 0; i < mifHdr->numEntry; i++)
    {
        entry_s* curEntry = entryList + i;
        for (int j = 0; j < fileNameList.size(); j++)
        {
            if (curEntry->hash == fileNameHashList[j])
            {
                hashName_s hashName;
                sprintf_s(hashName.fileName, "%s", fileNameList[j].c_str());
                hashName.hash = curEntry->hash;
                matchHashList2.push_back(hashName);
                break;
            }
        }

    }
    mif2.close();
    */

    //char* outFileName = (char*)"D:\\下载\\GameRE\\Cars ROR PSP EUR mif\\matchFileList.txt";

    char outFileName[512];

    string fname = GetFileNameAddPathWithouExt(nameListfile);
    sprintf_s(outFileName, "%s_%s_match_hash.txt", fname.c_str(), GetFileNameWithExt(mifName).c_str());

    ofstream out(outFileName, ios::out);
    vector<string> allMatchNameList;
    for (unsigned int i = 0; i < (unsigned int)matchHashList.size(); i++)
    {
        string fileName = matchHashList[i].fileName + (string)"\n";
        //out.write(fileName.c_str(), fileName.size());
        allMatchNameList.push_back(fileName);
    }
    /*
    for (int i = 0; i < matchHashList2.size(); i++)
    {
        string fileName = (string)matchHashList2[i].fileName + (string)"\n";
        //out.write(fileName.c_str(), fileName.size());
        allMatchNameList.push_back(fileName);
    }*/
    sort(allMatchNameList.begin(), allMatchNameList.end(), [](string a, string b) {return a < b; });
    for (unsigned int i = 0; i < (unsigned int)allMatchNameList.size(); i++)
    {
        out.write(allMatchNameList[i].c_str(), allMatchNameList[i].size());
    }
    out.close();
    cout << outFileName << endl;
}
