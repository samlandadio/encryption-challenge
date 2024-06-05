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

int writeBinFile(char* filename, int* input, size_t size) {
	FILE* filePtr;
	errno_t errorCode = fopen_s(&filePtr, filename, "wb");

	if (errorCode != 0) return 2; //Error Code

	size_t strSize = fwrite(input, sizeof(int), size, filePtr);
	
	if (strSize != size) return 2; //Error Code
	
	return 1; //Success Code
}

int readTextFile(const char *filename, char **str) {
	FILE* filePtr;
	errno_t errorCode = fopen_s(&filePtr, filename, "r");

	if (errorCode != 0) return 2;

	fseek(filePtr, 0, SEEK_END);
	long long size = ftell(filePtr);
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

void encrypt(char* filename, unsigned int** output, int* size, int key) {
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
		(*output)[i] = i + key + (unsigned int)input[i];
	}
}

void decrypt(int input[], char** output, int size, int key) {
	*output = (char*)malloc((size + 1) * sizeof(char));

	if (*output == NULL) {
		printf("Error...");
		return;
	}

	int i;
	for (i = 0; i < size; i++) {
		(*output)[i] = (char)input[i] - i - key;
	}
	(*output)[i] = '\0';
}

int main() {
	//Get user input
	const char *inputFile = "input.txt";
	const char* binFile = "output.bin";

	unsigned int *output = NULL;
	int size;
	encrypt(inputFile, &output, &size, dkey);

	int errorCode = writeBinFile(binFile, &output, size);
	if (errorCode == 2) printf("Bin error");

	//Old text file binary code
	//const char* filename2 = "encrypted.txt";
	//writeFile(filename2, output, (size_t)size);
	//printf("%s", "Encrypted output:\n");
	//for (size_t i = 0; i < size; i++) {
		//itob(output[i]);
		//printf("%d", output[i]);
	//}
	//printf("%s", "\n\n");

	//Decrypted message
	//Add size algorithm
	char *message = NULL;
	decrypt(output, &message, size, 9);
	printf("%s", "Decrypted output: \n");
	printArray(message);

	free(output);
	free(message);

	return 0;
}