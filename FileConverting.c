#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char** argv){
	
	FILE *fp;
	int i, size;
	char c;
	
	fp = fopen("test.txt", "rb");
	if(fp == NULL){
		perror("File was not found");
	}
	
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	printf("Size of file: %i\n", size);
	fseek(fp, 0, SEEK_SET);
	
	char *buffer = malloc(size+1);
	
	/*c = fgetc(fp);
	if(c != 80){
		printf("Incorrect file type: Expected .PPM file\n");
		return 1;
	}
	fseek(fp, 0, SEEK_SET);*/
	
	for(i = 0; i < size; i++){
		c = fgetc(fp);
		buffer[i] = c;
	}
	fwrite(buffer, size, 1, stdout);
	
	return 0;
}
