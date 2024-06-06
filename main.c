#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <errno.h>

int dkey = 9;

void printArray(const char* str) {
	while (*str != '\0') {
		printf("%c", *str);
		str++;
	}
	printf("\n");
}

int readBinFile(const char* filename, unsigned int** output, int* size) {
	FILE* filePtr;
	errno_t errorCode = fopen_s(&filePtr, filename, "rb");

	if (errorCode != 0) return 2; //Error Code

	//Error cases
	long fsize = ftell(filePtr);
	size_t num = fsize / sizeof(unsigned int);
	if (fseek(filePtr, 0, SEEK_END) != 0 || fsize == -1L || fseek(filePtr, 0, SEEK_SET) != 0) {
		fclose(filePtr);
		return 2; //Error Code
	}

	*output = (unsigned int *)malloc(num * sizeof(unsigned int));
	if (*output == NULL) {
		fclose(filePtr);
		return 2; //Error Code
	}

	size_t bytes = fread(*output, sizeof(unsigned int), 2, filePtr);
	if (bytes != num) {
		//(filePtr);
		//return 2; //Error Code
	}
	printf("%c\n", (*output[0]));
	fclose(filePtr);
	return 1; //Success code
}

int writeBinFile(char* filename, int** input, size_t size) {
	FILE* filePtr;
	errno_t errorCode = fopen_s(&filePtr, filename, "wb");

	if (errorCode != 0) return 2; //Error Code

	size_t strSize = fwrite(*input, sizeof(unsigned int), size, filePtr);
	
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

//For writing binary to a text file
char* itob(unsigned int num) {
	char* binary = (char*)malloc(33 * sizeof(char));

	if (binary == NULL) {
		return NULL;
	}

	for (int i = 31; i > -1; i--) {
		if (num >= pow(2, i)) {
			binary[i] = '1';
			printf("1");
			num -= pow(2, i);
		}
		else {
			printf("0");
			binary[i] = '0';
		}
	}
	binary[32] = '\0';
	return *binary;
}

//For printing in binary
void itob2(unsigned int num) {
	bool binary[32];
	for (int i = 31; i > -1; i--) {
		if (num >= pow(2, i)) {
			binary[i] = true;
			num -= pow(2, i);
			printf("1");
		}
		else {
			binary[i] = false;
			printf("0");
		}
	}
}

void encrypt(char* filename, unsigned int** output, int *size, int key) {
	char* input = NULL;
	int errorCode = readTextFile(filename, &input);
	if (errorCode == 2) {
		printf("Error opening text file...");
		return 2;
	}

	*size = strlen(input);
	*output = (unsigned int*)malloc(*size * sizeof(unsigned int));

	if (*output == NULL) {
		printf("Error...");
		return;
	}

	int i;
	for (i = 0; i < *size; i++) {
		(*output)[i] = /**i + key + */(unsigned int)input[i]; //Encryption
	}
}

void decrypt(unsigned int* input[], char** output, int size, int key) {
	*output = (char*)malloc((size + 1) * sizeof(char));

	if (*output == NULL) {
		printf("Error...");
		return;
	}

	int i;
	int j = 0;
	for (i = 0; i < size - j; i++) {
		(*output)[i] = (char)input[i]; //- i - key;
		if ((output)[i] == '\n') j++;
		printf("%c", (*output)[i]);
	}
	(*output)[i] = '\0';
}

int main() {
	//Get user input
	const char *inputFile = "input.txt";
	const char *binFile   = "output.bin";

	unsigned int *output = NULL;
	int size;
	encrypt(inputFile, &output, &size, dkey);

	int errorCode = writeBinFile(binFile, &output, (size_t)size);
	if (errorCode == 2) printf("Bin write error");

	//Decrypted message
	//TODO: Add size algorithm

	//Open Bin
	char *input2 = NULL;
	errorCode = readBinFile(binFile, &input2, size);
	if (errorCode == 2) printf("Bin read error");

	char *message = NULL;
	decrypt(output, &message, size, dkey);
	//printf("%s", "Decrypted output: \n");
	printArray(message);

	//free(input2);
	//free(output);
	//free(message);

	return 0;
}
