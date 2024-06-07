#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <errno.h>
#include <crtdbg.h>

void printArray(const char* str) {
	while (*str != '\0') {
		printf("%c", *str);
		str++;
	}
	printf("\n");
}

int readBinFile(const char* filename, char** output, int key) {
	FILE *filePtr;
	errno_t errorCode = fopen_s(&filePtr, filename, "rb");

	if (errorCode != 0) return 2; //Error Code

	fseek(filePtr, 0, SEEK_END);
	long fsize = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);
	size_t num = fsize / sizeof(int);

	*output = (char *)malloc((num + 1) * sizeof(char));
	if (*output == NULL) {
		fclose(filePtr);
		return 2; //Error Code
	}

	//Reading
	int value;
	size_t i = 0;
	while (fread(&value, sizeof(int), 1, filePtr) == 1 && i < num ) {
		(*output)[i] = (char)value - i - key;
		if (((char)value - i - key) == '\n') num--;
		i++;
	}
	(*output)[num] = '\0';

	fclose(filePtr);
	return 1; //Success code
}

int writeBinFile(char* filename, int** input, size_t size) {
	FILE* filePtr;
	errno_t errorCode = fopen_s(&filePtr, filename, "wb");

	if (errorCode != 0) return 2; //Error Code

	size_t strSize = fwrite(*input, sizeof(int), size, filePtr);
	
	if (strSize != size) return 2; //Error Code
	
	fclose(filePtr);
	return 1; //Success Code
}

int readTextFile(const char *filename, char **str) {
	FILE* filePtr;
	errno_t errorCode = fopen_s(&filePtr, filename, "r");

	if (errorCode != 0) return 2;

	fseek(filePtr, 0, SEEK_END);
	long size = ftell(filePtr);
	if (size == -1) {
		fclose(filePtr);
		return 2; //Error Code
	}

	*str = (char *)malloc((size + 1) * sizeof(char));
	if (*str == NULL) {
		fclose(filePtr);
		return 2; //Error Code
	}
	fseek(filePtr, 0, SEEK_SET);

	size_t readSize = fread(*str, sizeof(char), size, filePtr);

	(*str)[size] = '\0';

	fclose(filePtr);
	return 1; //Success code
}

int writeTextFile(const char* filename, char* contents) {
	FILE* filePtr;
	errno_t errorCode = fopen_s(&filePtr, filename, "w");

	if (errorCode != 0) return 2;

	fprintf(filePtr, "%s", contents);

	fclose(filePtr);
	return 1;
}

void encrypt(char* filename, char* binFile, int key) {
	char* input = NULL;
	int size;
	int errorCode = readTextFile(filename, &input);
	if (errorCode == 2) {
		printf("Error opening text file...");
		free(input);
		return;
	}

	size = strlen(input);
	int* output = (int*)malloc(size * sizeof(int));

	if (output == NULL) {
		printf("Error...");
		free(input);
		free(output);
		return;
	}

	int i;
	for (i = 0; i < size; i++) {
		output[i] = i + key + (int)input[i]; //Encryption
	}

	errorCode = writeBinFile(binFile, &output, (size_t)(size));
	if (errorCode == 2) {
		printf("Bin write error...");
		free(input);
		free(output);
		return;
	}
	free(input);
	free(output);
}

void decrypt(char* binFile, const char* outputFile, const char* flag, int key) {
	//Open Bin
	char* input = NULL;
	int errorCode = readBinFile(binFile, &input, key);
	if (errorCode == 2) printf("Bin read error...");

	if (flag == "-c") {
		printArray(input);
	}
	else if (flag == "-f") {
		errorCode = writeTextFile(outputFile, input);
		if (errorCode == 2) {
			printf("Error writing to text file...");
			free(input);
			return;
		}
	}
	else {
		printf("Flag invalid...");
	}
	free(input);
}

int main(int argc, char* argv[]) {
	//Check for memory leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	int key;
	if (argc != 5) {
		printf("Error: Incorrect number of arguments...");
		return 1;
	}
	else if (sscanf_s(argv[4], "%d", &key) != 1) {
		printf("Error: Invalid key...");
		return 2;
	}
	else if (strcmp(argv[2], "-e") == 0) {
		encrypt(argv[1], argv[3], key);
	}
	else if (strcmp(argv[2], "-d") == 0) {
		decrypt(argv[3], argv[1], "-f", key);
	}
	else {
		printf("Error: Unexpected flag...");
		return 3;
	}
	return 0;
}
