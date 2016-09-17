#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

typedef struct Pixel{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} Pixel;

FILE *input_fp, *output_fp;
int height, width, depth;

void p6_to_p3(FILE *input, FILE *output);
void p3_to_p6(FILE *input, FILE *output);
FILE* escape_comments(FILE *input, int c);

int main(int argc, char** argv){
	
	int size;
	int c;
	
	if(argc < 4){
		printf("Not enough args...\n");
	}
	/*if(strcmp(argv[1], "3")){
		printf("Is 3\n");
	}
	else if(strcmp(argv[1], "6")){
		printf("Is 6\n");
	}*/
	
	input_fp = fopen("upascii.ppm", "rb");
	if(input_fp == NULL){
		perror("File was not found");
	}
	output_fp = fopen("something.ppm", "wb+");
	if(output_fp == NULL){
		perror("Output file failed in creation");
	}
	
	fseek(input_fp, 0, SEEK_END);
	size = ftell(input_fp);
	printf("Size of file: %i\n", size);
	fseek(input_fp, 0, SEEK_SET);
	
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
		p3_to_p6(input_fp, output_fp);
	}
	if(c == 54){
		//send fp to conversion function
		p6_to_p3(input_fp, output_fp);
	}
	
	return 0;
}

void p6_to_p3(FILE *input, FILE *output){
	int c;
	c = fgetc(input);
	if(c != '\n'){
		fprintf(stderr, "Incorrect .PPM file type\n");
		exit(1);
	}
	
	// placing P3 header info into new file
	c = fgetc(input);
	fprintf(output, "P3\n");
	input = escape_comments(input, c);
	
	// transfering image dimension header info to new file
	fscanf(input, "\n%d %d\n%d\n", &width, &height, &depth);
	if(depth > 255){
		printf("Error: Image not an 8 bit channel\n");
		exit(1);
	}
	fprintf(output, "%d %d\n%d\n", width, height, depth);
	
	// generating reading and writing pixels
	Pixel new;
	Pixel *buffer = malloc(sizeof(Pixel)*width*height);
	int count = 0;
	while(!feof(input)){
		fread(&new.red, 1, 1, input);
		fread(&new.green, 1, 1, input);
		fread(&new.blue, 1, 1, input);
		buffer[count] = new;
		count++;
		fprintf(output, "%i %i %i ", new.red, new.green, new.blue);
	}
}

void p3_to_p6(FILE *input, FILE *output){
	int c;
	c = fgetc(input);
	if(c != '\n'){
		fprintf(stderr, "Incorrect .PPM file type\n");
		exit(1);
	}
	
	// placing P6 header info into new file
	c = fgetc(input);
	fprintf(output, "P6\n");
	input = escape_comments(input, c);
	
	// transfering image dimension header info to new file
	fscanf(input, "\n%d %d\n%d\n", &width, &height, &depth);
	if(depth > 255){
		printf("Error: Image not an 8 bit channel\n");
		exit(1);
	}
	fprintf(output, "%d %d\n%d\n", width, height, depth);
	
	// reading and writing pixels
	Pixel new;
	Pixel *buffer = malloc(sizeof(Pixel)*width*height);
	printf("size of pixel buffer: %li\n", sizeof(Pixel)*width*height);
	int count = 0;
	while(!feof(input)){
		fscanf(input, "%hhu ", &new.red);
		fscanf(input, "%hhu ", &new.green);
		fscanf(input, "%hhu ", &new.blue);
		buffer[count] = new;
		count++;
	}
	fwrite(buffer, sizeof(Pixel), count, output);
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
