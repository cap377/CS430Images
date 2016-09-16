#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct Pixel{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} Pixel;

FILE *input_fp, *output_fp;
int height, width, depth;

void p6_to_p3(FILE *input, FILE *output, Pixel *buffer);
FILE* escape_comments(FILE *input, int c);

int main(int argc, char** argv){
	
	int size;
	int c;
	
	input_fp = fopen("upraw.ppm", "rb");
	if(input_fp == NULL){
		perror("File was not found");
	}
	output_fp = fopen("something.ppm", "wb");
	if(output_fp == NULL){
		perror("Output file failed in creation");
	}
	
	fseek(input_fp, 0, SEEK_END);
	size = ftell(input_fp);
	printf("Size of file: %i\n", size);
	fseek(input_fp, 0, SEEK_SET);
	
	Pixel *buffer = malloc(size+1);
	
	c = fgetc(input_fp);
	if(c != 80){
		printf("Incorrect file type: Expected .PPM file\n");
		return 1;
	}
	
	//getting magic number
	c = fgetc(input_fp);
	if(c != 51 && c != 54){
		printf("Incorrect file type: Expected P3 or P6 file\n");
		return 1;
	}
	
	if(c == 51){
		//send fp to conversion function
	}
	if(c == 54){
		p6_to_p3(input_fp, output_fp, buffer);
	}
	
	return 0;
}

void p6_to_p3(FILE *input, FILE *output, Pixel *buffer){
	int c;
	c = fgetc(input);
	if(c != '\n'){
		printf("Incorrect .PPM file type\n");
		EXIT_FAILURE;
	}
	
	// placing P3 header info into new file
	c = fgetc(input);
	fprintf(output, "P3\n");
	input = escape_comments(input, c);
	
	// transfering image dimension header info to new file
	fscanf(input, "\n%d %d\n%d\n", &width, &height, &depth);
	fprintf(output, "%d %d\n%d\n", width, height, depth);
	
	// generating pixel buffer
	Pixel new;
	while(!feof(input)){
		fread(&new.red, 1, 1, input);
		fread(&new.green, 1, 1, input);
		fread(&new.blue, 1, 1, input);
		fprintf(output, "%i %i %i ", new.red, new.green, new.blue);
	}
}

FILE* escape_comments(FILE *input, int c){
	if(c == '#'){
		printf("Traversing comments...\n");
		while(c != '\n'){
			c = fgetc(input);
		}
		printf("escaped comments\n");
		//c = fgetc(input);
		input = escape_comments(input, c);
	}
	return input;
}
