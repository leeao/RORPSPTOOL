#pragma once

struct decmpHeader {
	unsigned int stream;//0x0
	unsigned int flag1;//0x4
	unsigned int curSplitBlockIndex;//0x8 当前分卷数量计数？最多7个分卷
	char tempBuffer[2048];//0xC
	unsigned int	curTmpBufferPtr;//0x80c 
	unsigned int	curTmpBufferLength;//0x810
	unsigned int	cmpSize;//0x814
	unsigned int	unk5;//0x818
	unsigned int	unk6;//0x81C
	unsigned int	outBufferBasePtr;//0x820
	unsigned int	decmpSize;//0x824
	unsigned int	unk7;//0x828
	unsigned int	unk8;//0x82C
};

typedef struct {
	unsigned int streamPtr;
	unsigned int basePos;
	unsigned int curPos;
}RORStream;




void fileRead(int* stream, int tempBuffer,int numBytes);
int DecompressPACK(int* stream, unsigned int outBufferBasePtr, int decmpSize, int cmpSize);