#include "diskinfo.h"

int main(int argc, char**argv)
{
	if (argc < 2) {
		printf("Usage: ./diskinfo [.img file]\n");
		return 0;
	}
	
	FILE *infile = fopen(argv[1], "r");
	/* Respective parts of the superblock */
	int BlockSize, FSCount, StartPtr, FATCount, RootPtr, RootCount, block, FreeBlocksCount,
	ReservedBlocksCount, AllocatedBlocksCount;
	AllocatedBlocksCount = 0; ReservedBlocksCount = 0; FreeBlocksCount = 0;
	int currentSegmentSize = 2;							/* how large the current superblock segment is */
	void *currentPtr = malloc(currentSegmentSize);		/* the current segment of the superblock */
	
	fseek(infile, 8, SEEK_CUR);			/* Skip past the name */
	
	/* block determines which part of the superblock to examine */
	for (block = 0;block < 6;block++) {
		switch(block) {
			case 0:
				currentSegmentSize = 2;
				currentPtr = malloc(currentSegmentSize);
				fread(currentPtr, 1, currentSegmentSize, infile);
				memcpy(&BlockSize, currentPtr, currentSegmentSize);
				BlockSize = ntohs(BlockSize);
				break;
			case 1:
				currentSegmentSize = 4;
				currentPtr = malloc(currentSegmentSize);
				fread(currentPtr, 1, currentSegmentSize, infile);
				memcpy(&FSCount, currentPtr, currentSegmentSize);
				FSCount = ntohl(FSCount);
				break;
			case 2:
				currentPtr = malloc(currentSegmentSize);
				fread(currentPtr, 1, currentSegmentSize, infile);
				memcpy(&StartPtr, currentPtr, currentSegmentSize);
				StartPtr = ntohl(StartPtr);
				break;
			case 3:
				currentPtr = malloc(currentSegmentSize);
				fread(currentPtr, 1, currentSegmentSize, infile);
				memcpy(&FATCount, currentPtr, currentSegmentSize);
				FATCount = ntohl(FATCount);
				break;
			case 4:
				currentPtr = malloc(currentSegmentSize);
				fread(currentPtr, 1, currentSegmentSize, infile);
				memcpy(&RootPtr, currentPtr, currentSegmentSize);
				RootPtr = ntohl(RootPtr);
				break;
			case 5:
				currentPtr = malloc(currentSegmentSize);
				fread(currentPtr, 1, currentSegmentSize, infile);
				memcpy(&RootCount, currentPtr, currentSegmentSize);
				RootCount = ntohl(RootCount);
				break;
		}
		free(currentPtr);
		currentPtr = NULL;
	}
	rewind(infile);
	fseek(infile, 32, SEEK_CUR);
	currentPtr = NULL;
	currentPtr = malloc(4);
	int hexVal;
	for (;;) { 
		int bytes = fread(currentPtr, 1, 4, infile);
		if (bytes == 0) break;
		memcpy(&hexVal, currentPtr, 4);
		hexVal = ntohl(hexVal);
		switch (hexVal) {
			case 0x00000000:
				FreeBlocksCount++;
				break;
			case 0x00000001:
				ReservedBlocksCount++;
				break;
			default:
				AllocatedBlocksCount++;
				break;
		}
		free(currentPtr);
		currentPtr = NULL;
		currentPtr = malloc(4);
		
	}
	
	/*
	currentPtr = malloc(1);
	int RootAddrAsDec = htoi(RootPtr);
	
	fseek(infile, (RootAddrAsDec*16), SEEK_CUR);
	DEntry root;
	fread(currentPtr, 1, 1, infile);
	memcpy(&root.status, currentPtr, 1);
	printf("Bit 0 of status byte: %d\n", CHECK_BIT(root.status, 0));
	*/	
	printf("Super block information:\n");
	printf("Block Size: %d\n", BlockSize);
	printf("Block Count: %d\n", FSCount);
	printf("FAT starts: %d\n", StartPtr);
	printf("FAT blocks: %d\n", FATCount);
	printf("Root directory start: %d\n", RootPtr);
	printf("Root directory blocks: %d\n", RootCount);
	printf("\nFAT information:\n");
	printf("Free Blocks: %d\n", FreeBlocksCount);
	printf("Reserved Blocks: %d\n", ReservedBlocksCount);
	printf("Allocated Blocks: %d\n", AllocatedBlocksCount);
	return 0;
}

/* Converts a input hex number and converts to decimal */
int htoi(int inputHex) {
	char s[7];
	sprintf(s, "%d", inputHex);
	return (int)strtol(s, NULL, 16);
}
