#include "PACK.h"
#include <string.h>



void fileRead(int* stream, int tempBuffer, int numBytes)
{
    unsigned int streamPos = *stream + stream[2];
    memcpy((char*)tempBuffer, (char*)streamPos, sizeof(char) * numBytes);
    stream[2] += numBytes;
}

int DecompressPACK(int* stream, unsigned int outBufferBasePtr, int decmpSize, int cmpSize)
{
    char uVar1;
    bool bVar2;
    unsigned char bVar3;
    //int uVar4;
    unsigned int uVar5;
    char* puVar6;
    //int uVar7;
    unsigned int uVar8;
    char* puVar9;
    //int uVar10;
    //int* puVar11;
    unsigned int uVar12;
    //int* puVar13;
    int curOutBufferPtr;
    int iVar14;
    int iVar15;
    unsigned int uVar16;
    unsigned char* pbVar17;
    unsigned int uVar18;
    int iVar19;
    int streamPtr;
    unsigned char  flag;
    unsigned int curSplitBlockIndex;
    int tempBuffer[512]{};
    int curTmpBufferPtr;
    int curTmpBufferLength;
    int curCmpSize;
    int outBufferBasePtr_local_40;
    int decmpSize_local_3c;
    //int* puNextTmpBuffer;
    //int* puPrevTmpBuffer;
    curSplitBlockIndex = 0;
    decmpSize_local_3c = decmpSize;
    outBufferBasePtr_local_40 = outBufferBasePtr;
    /*
    puNextTmpBuffer = &streamPtr;    
    puPrevTmpBuffer = &DAT_08b5e350;
    do {
        puVar13 = puPrevTmpBuffer;
        puVar11 = puNextTmpBuffer;
        uVar4 = puVar13[1];
        uVar7 = puVar13[2];
        uVar10 = puVar13[3];
        *puVar11 = *puVar13;
        puVar11[1] = uVar4;
        puVar11[2] = uVar7;
        puVar11[3] = uVar10;
        puNextTmpBuffer = puVar11 + 4;
        puPrevTmpBuffer = puVar13 + 4;
    } while (puVar13 + 4 != (undefined4*)&DAT_08b5eb60);*/
    streamPtr = (int)stream;
    //puVar11[5] = puVar13[5];
    curOutBufferPtr = 0;
    //puVar11[4] = 0;
    curCmpSize = cmpSize;
    curTmpBufferPtr = 0;
    curTmpBufferLength = 0;
    iVar15 = curOutBufferPtr;
    if (0 < decmpSize_local_3c) {
        while (true) {
            if (curSplitBlockIndex == 0) {
                if (curTmpBufferPtr < curTmpBufferLength) {
                    flag = *(unsigned char*)((int)tempBuffer + curTmpBufferPtr);
                    curTmpBufferPtr = curTmpBufferPtr + 1;
                }
                else if (curCmpSize < 1) {
                    //flag = FUN_088373ac(streamPtr);
                }
                else {
                    curOutBufferPtr = curCmpSize;
                    if (0x800 < curCmpSize) {
                        curOutBufferPtr = 0x800;
                    }
                    fileRead((int*)streamPtr, (int)tempBuffer, curOutBufferPtr);
                    curCmpSize = curCmpSize - curOutBufferPtr;
                    curTmpBufferLength = curOutBufferPtr;
                    curTmpBufferPtr = 1;
                    flag = (unsigned char)tempBuffer[0];
                }
            }
            uVar8 = (unsigned int)flag;
            uVar18 = 1;
            uVar16 = curSplitBlockIndex & 0x1f;
            curSplitBlockIndex = curSplitBlockIndex + 1 & 7;
            curOutBufferPtr = iVar15;
            if (((int)uVar8 >> uVar16 & 1U) != 0) goto LAB_0884cb24;
            while (true) {
                if (curSplitBlockIndex == 0) {
                    if (curTmpBufferPtr < curTmpBufferLength) {
                        uVar8 = (unsigned int) * (unsigned char*)((int)tempBuffer + curTmpBufferPtr);
                        curTmpBufferPtr = curTmpBufferPtr + 1;
                    }
                    else if (curCmpSize < 1) {
                        //uVar8 = FUN_088373ac(streamPtr);
                        //uVar8 = uVar8 & 0xff;
                    }
                    else {
                        iVar19 = curCmpSize;
                        if (0x800 < curCmpSize) {
                            iVar19 = 0x800;
                        }
                        fileRead((int*)streamPtr, (int)tempBuffer, iVar19);
                        uVar8 = (unsigned int)(unsigned char)tempBuffer[0];
                        curCmpSize = curCmpSize - iVar19;
                        curTmpBufferLength = iVar19;
                        curTmpBufferPtr = 1;
                    }
                    flag = (unsigned char)uVar8;
                }
                uVar16 = curSplitBlockIndex & 0x1f;
                curSplitBlockIndex = curSplitBlockIndex + 1 & 7;
                if (((int)uVar8 >> uVar16 & 1U) == 0) break;
                if (curSplitBlockIndex == 0) {
                    if (curTmpBufferPtr < curTmpBufferLength) {
                        uVar8 = (unsigned int) * (unsigned char*)((int)tempBuffer + curTmpBufferPtr);
                        curTmpBufferPtr = curTmpBufferPtr + 1;
                    }
                    else {
                        iVar19 = curCmpSize;
                        if (0x800 < curCmpSize) {
                            iVar19 = 0x800;
                        }
                        if (curCmpSize < 1) {
                            //uVar8 = FUN_088373ac(streamPtr);
                            //uVar8 = uVar8 & 0xff;
                        }
                        else {
                            fileRead((int*)streamPtr, (int)tempBuffer, iVar19);
                            uVar8 = (unsigned int)(unsigned char)tempBuffer[0];
                            curCmpSize = curCmpSize - iVar19;
                            curTmpBufferLength = iVar19;
                            curTmpBufferPtr = 1;
                        }
                    }
                    flag = (unsigned char)uVar8;
                }
                uVar16 = curSplitBlockIndex & 0x1f;
                curSplitBlockIndex = curSplitBlockIndex + 1 & 7;
                uVar18 = (int)uVar8 >> uVar16 & 1U | uVar18 << 1;
            }
            if (curSplitBlockIndex == 0) {
                uVar16 = 0;
                pbVar17 = (unsigned char*)(outBufferBasePtr_local_40 + iVar15);
                if (uVar18 != 0) {
                    do {
                        if (curTmpBufferPtr < curTmpBufferLength) {
                            bVar3 = *(unsigned char*)((int)tempBuffer + curTmpBufferPtr);
                            curTmpBufferPtr = curTmpBufferPtr + 1;
                        }
                        else {
                            curOutBufferPtr = curCmpSize;
                            if (0x800 < curCmpSize) {
                                curOutBufferPtr = 0x800;
                            }
                            if (curCmpSize < 1) {
                                //bVar3 = FUN_088373ac(streamPtr, tempBuffer, curOutBufferPtr);
                            }
                            else {
                                fileRead((int*)streamPtr,(int)tempBuffer, curOutBufferPtr);
                                curTmpBufferLength = curOutBufferPtr;
                                curCmpSize = curCmpSize - curOutBufferPtr;
                                curTmpBufferPtr = 1;
                                bVar3 = (unsigned char)tempBuffer[0];
                            }
                        }
                        uVar16 = uVar16 + 1;
                        *pbVar17 = bVar3;
                        iVar15 = iVar15 + 1;
                        pbVar17 = pbVar17 + 1;
                        curOutBufferPtr = iVar15;
                    } while (uVar18 != uVar16);
                }
            }
            else {
                iVar19 = 0;
                pbVar17 = (unsigned char*)(iVar15 + outBufferBasePtr_local_40);
                if (uVar18 != 0) {
                    while (true) {
                        bVar3 = (unsigned char)((int)uVar8 >> (curSplitBlockIndex & 0x1f));
                        if (curTmpBufferPtr < curTmpBufferLength) {
                            flag = *(unsigned char*)((int)tempBuffer + curTmpBufferPtr);
                            curTmpBufferPtr = curTmpBufferPtr + 1;
                            *pbVar17 = flag << (8 - curSplitBlockIndex & 0x1f) | bVar3;
                        }
                        else {
                            curOutBufferPtr = curCmpSize;
                            if (0x800 < curCmpSize) {
                                curOutBufferPtr = 0x800;
                            }
                            if (curCmpSize < 1) {
                                //uVar16 = FUN_088373ac(streamPtr, tempBuffer, curOutBufferPtr);
                                //uVar16 = uVar16 & 0xff;
                            }
                            else {
                                fileRead((int*)streamPtr, (int)tempBuffer, curOutBufferPtr);
                                uVar16 = (unsigned int)(unsigned char)tempBuffer[0];
                                curTmpBufferLength = curOutBufferPtr;
                                curCmpSize = curCmpSize - curOutBufferPtr;
                                curTmpBufferPtr = 1;
                            }
                            flag = (unsigned char)uVar16;
                            *pbVar17 = (unsigned char)(uVar16 << (8 - curSplitBlockIndex & 0x1f)) | bVar3;
                        }
                        curOutBufferPtr = iVar15 + 1;
                        if (iVar19 + 1U == uVar18) break;
                        iVar15 = iVar15 + 1;
                        pbVar17 = pbVar17 + 1;
                        iVar19 = iVar19 + 1;
                        uVar8 = (unsigned int)flag;
                    }
                }
            }
            bVar2 = true;
            if (decmpSize_local_3c <= curOutBufferPtr) break;
            while (iVar15 = curOutBufferPtr, bVar2) {
            LAB_0884cb24:
                uVar16 = 1;
                while (true) {
                    if (curSplitBlockIndex == 0) {
                        if (curTmpBufferPtr < curTmpBufferLength) {
                            flag = *(unsigned char*)((int)tempBuffer + curTmpBufferPtr);
                            curTmpBufferPtr = curTmpBufferPtr + 1;
                        }
                        else if (curCmpSize < 1) {
                            //flag = FUN_088373ac(streamPtr);
                        }
                        else {
                            iVar15 = curCmpSize;
                            if (0x800 < curCmpSize) {
                                iVar15 = 0x800;
                            }
                            fileRead((int*)streamPtr, (int)tempBuffer, iVar15);
                            curCmpSize = curCmpSize - iVar15;
                            curTmpBufferLength = iVar15;
                            curTmpBufferPtr = 1;
                            flag = (unsigned char)tempBuffer[0];
                        }
                    }
                    uVar18 = (unsigned int)flag;
                    uVar8 = curSplitBlockIndex & 0x1f;
                    curSplitBlockIndex = curSplitBlockIndex + 1 & 7;
                    if (((int)uVar18 >> uVar8 & 1U) == 0) break;
                    if (curSplitBlockIndex == 0) {
                        if (curTmpBufferPtr < curTmpBufferLength) {
                            uVar18 = (unsigned int) * (unsigned char*)((int)tempBuffer + curTmpBufferPtr);
                            curTmpBufferPtr = curTmpBufferPtr + 1;
                        }
                        else if (curCmpSize < 1) {
                            //uVar18 = FUN_088373ac(streamPtr);
                            //uVar18 = uVar18 & 0xff;
                        }
                        else {
                            iVar15 = curCmpSize;
                            if (0x800 < curCmpSize) {
                                iVar15 = 0x800;
                            }
                            fileRead((int*)streamPtr, (int)tempBuffer, iVar15);
                            uVar18 = (unsigned int)(unsigned char)tempBuffer[0];
                            curCmpSize = curCmpSize - iVar15;
                            curTmpBufferLength = iVar15;
                            curTmpBufferPtr = 1;
                        }
                        flag = (unsigned char)uVar18;
                    }
                    uVar8 = curSplitBlockIndex & 0x1f;
                    curSplitBlockIndex = curSplitBlockIndex + 1 & 7;
                    uVar16 = (int)uVar18 >> uVar8 & 1U | uVar16 << 1;
                }
                iVar19 = uVar16 + 2;
                uVar16 = 0;
                uVar8 = 0;
                for (iVar15 = curOutBufferPtr + -1; 0x7f < iVar15; iVar15 = iVar15 >> 8) {
                    if (curSplitBlockIndex == 0) {
                        if (curTmpBufferPtr < curTmpBufferLength) {
                            uVar18 = (unsigned int) * (unsigned char*)((int)tempBuffer + curTmpBufferPtr);
                            curTmpBufferPtr = curTmpBufferPtr + 1;
                        }
                        else if (curCmpSize < 1) {
                            //uVar18 = FUN_088373ac(streamPtr);
                            //uVar18 = uVar18 & 0xff;
                        }
                        else {
                            iVar14 = curCmpSize;
                            if (0x800 < curCmpSize) {
                                iVar14 = 0x800;
                            }
                            fileRead((int*)streamPtr, (int)tempBuffer, iVar14);
                            uVar18 = (unsigned int)(unsigned char)tempBuffer[0];
                            curTmpBufferLength = iVar14;
                            curCmpSize = curCmpSize - iVar14;
                            curTmpBufferPtr = 1;
                        }
                    }
                    else {
                        uVar5 = (unsigned int)flag;
                        uVar18 = curSplitBlockIndex & 0x1f;
                        if (curTmpBufferPtr < curTmpBufferLength) {
                            uVar12 = (unsigned int) * (unsigned char*)((int)tempBuffer + curTmpBufferPtr);
                            curTmpBufferPtr = curTmpBufferPtr + 1;
                        }
                        else if (curCmpSize < 1) {
                            //uVar12 = FUN_088373ac(streamPtr);
                            //uVar12 = uVar12 & 0xff;
                        }
                        else {
                            iVar14 = curCmpSize;
                            if (0x800 < curCmpSize) {
                                iVar14 = 0x800;
                            }
                            fileRead((int*)streamPtr, (int)tempBuffer, iVar14);
                            uVar12 = (unsigned int)(unsigned char)tempBuffer[0];
                            curTmpBufferLength = iVar14;
                            curCmpSize = curCmpSize - iVar14;
                            curTmpBufferPtr = 1;
                        }
                        uVar18 = (int)uVar5 >> uVar18 | uVar12 << (8 - curSplitBlockIndex & 0x1f) & 0xff;
                        flag = (unsigned char)uVar12;
                    }
                    uVar16 = uVar16 | uVar18 << (uVar8 & 0x1f);
                    uVar8 = uVar8 + 8;
                }
                for (; iVar15 != 0; iVar15 = iVar15 >> 1) {
                    if (curSplitBlockIndex == 0) {
                        if (curTmpBufferPtr < curTmpBufferLength) {
                            flag = *(unsigned char*)((int)tempBuffer + curTmpBufferPtr);
                            curTmpBufferPtr = curTmpBufferPtr + 1;
                        }
                        else {
                            iVar14 = curCmpSize;
                            if (0x800 < curCmpSize) {
                                iVar14 = 0x800;
                            }
                            if (curCmpSize < 1) {
                                //flag = FUN_088373ac(streamPtr, tempBuffer, iVar14);
                            }
                            else {
                                fileRead((int*)streamPtr, (int)tempBuffer, iVar14);
                                curCmpSize = curCmpSize - iVar14;
                                curTmpBufferLength = iVar14;
                                curTmpBufferPtr = 1;
                                flag = (unsigned char)tempBuffer[0];
                            }
                        }
                    }
                    uVar18 = curSplitBlockIndex & 0x1f;
                    curSplitBlockIndex = curSplitBlockIndex + 1 & 7;
                    uVar16 = uVar16 | ((int)(unsigned int)flag >> uVar18 & 1U) << (uVar8 & 0x1f);
                    uVar8 = uVar8 + 1;
                }
                if (0 < iVar19) {
                    iVar15 = 0;
                    puVar9 = (char*)(outBufferBasePtr_local_40 + uVar16);
                    puVar6 = (char*)(outBufferBasePtr_local_40 + curOutBufferPtr);
                    do {
                        uVar1 = *puVar9;
                        iVar15 = iVar15 + 1;
                        puVar9 = puVar9 + 1;
                        *puVar6 = uVar1;
                        puVar6 = puVar6 + 1;
                    } while (iVar19 != iVar15);
                }
                curOutBufferPtr = curOutBufferPtr + iVar19;
                bVar2 = false;
                if (decmpSize_local_3c <= curOutBufferPtr) {
                    return curOutBufferPtr;
                }
            }
        }
    }
    return curOutBufferPtr;
}

