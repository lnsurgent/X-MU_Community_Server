#include "stdafx.h"

typedef unsigned long ULONG;

void InitCrcTable(void);
ULONG Reflect(ULONG , char);
int Get_CRC(unsigned char*, ULONG);
long FileSize(FILE*);


ULONG crc32_table[256];
ULONG ulPolynomial = 0x04c11db7;

int GetCheckSumFromMain()
{
	FILE *fs = fopen("GXP.dll", "rb");   //open file for reading 

	int crc;
	long bufsize = FileSize(fs), result;
	unsigned char *buffer = new unsigned char[bufsize];

	// copy the file into the buffer:
	result = fread (buffer,1,bufsize,fs);
	fclose(fs);

	InitCrcTable(); 
	crc = Get_CRC(buffer, bufsize);
	
	//printf("\nCRC: 0x%X\n",crc);
	delete [] buffer;
	return crc;
}


///////////////////////////////////////////////////////////////////

void InitCrcTable()
{

	// 256 values representing ASCII character codes.
	for(int i = 0; i <= 0xFF; i++)
	{
		crc32_table[i]=Reflect(i, 8) << 24;
		for (int j = 0; j < 8; j++)
			crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
		crc32_table[i] = Reflect(crc32_table[i], 32);
	}

}

//////////////////////////////////////////////////////////////
// Reflection is a requirement for the official CRC-32 standard.
// You can create CRCs without it, but they won't conform to the standard.
//////////////////////////////////////////////////////////////////////////

ULONG Reflect(ULONG ref, char ch)
{                                 // Used only by Init_CRC32_Table()

	ULONG value(0);

	// Swap bit 0 for bit 7
	// bit 1 for bit 6, etc.
	for(int i = 1; i < (ch + 1); i++)
	{
		if(ref & 1)
			value |= 1 << (ch - i);
		ref >>= 1;
	}
	return value;
}

///////////////////////////////////////////////////////////////

int Get_CRC(unsigned char* buffer, ULONG bufsize)
{

	ULONG  crc(0xffffffff);
	int len;
	len = bufsize;
	// Save the text in the buffer.

	// Perform the algorithm on each character
	// in the string, using the lookup table values.

	for(int i = 0; i < len; i++)
		crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ buffer[i]];


	// Exclusive OR the result with the beginning value.
	return crc^0xffffffff;

}

////////////////////////////////////////////////////////////

long FileSize(FILE *input)
{

	long fileSizeBytes;
	fseek(input, 0, SEEK_END);
	fileSizeBytes = ftell(input);
	fseek(input, 0, SEEK_SET);

	return fileSizeBytes;
}