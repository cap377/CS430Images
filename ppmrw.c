#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

// creating pixel structure
typedef struct Pixel{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} Pixel;

// global variables
FILE *input_fp, *output_fp;
int height, width, depth;

// prototype declarations
void p6_to_p3(FILE *input, FILE *output);
void p6_to_p6(FILE *input, FILE *output);
void p3_to_p6(FILE *input, FILE *output);
void p3_to_p3(FILE *input, FILE *output);
FILE* escape_comments(FILE *input, int c);


int main(int argc, char **argv){
	
	// initializing variables
	int size;
	int c;
	
	// checking for the correct number of args
	if(argc !=  4){
		printf("Not enough args...\n");
		//return 1;
	}
	// checking if convertion request is valid
	if(strcmp(argv[1], "3") == 0){
		printf("Converting to %s\n", argv[1]);
	}
	else if(strcmp(argv[1], "6") == 0){
		printf("Converting to %s\n", argv[1]);
	}
	else{
		printf("Error: Can only convert to P3 or P6\n");
		return 1;
	}
	
	// opening input and output files
	input_fp = fopen(argv[2], "rb");
	if(input_fp == NULL){
		printf("Error: File was not found\n");
		return 1;
	}
	output_fp = fopen(argv[3], "wb+");
	if(output_fp == NULL){
		printf("Error: Output file failed in creation\n");
		return 1;
	}
	
	// checking file size for my own curiosity
	fseek(input_fp, 0, SEEK_END);
	size = ftell(input_fp);
	printf("Size of file: %i\n", size);
	fseek(input_fp, 0, SEEK_SET);
	
	// checking what the current file type is
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
	
	// calling the correct function depending on the current file type and desired file output type
	if(c == 51){
		if(strcmp(argv[1], "6") == 0){
			//send fp to conversion function
			p3_to_p6(input_fp, output_fp);
		}
		else if(strcmp(argv[1], "3") == 0){
			p3_to_p3(input_fp, output_fp);
		}
	}
	if(c == 54){
		if(strcmp(argv[1], "3") == 0){
			//send fp to conversion function
			p6_to_p3(input_fp, output_fp);
		}
		else if(strcmp(argv[1], "6") == 0){
			p6_to_p6(input_fp, output_fp);
		}
	}
	
	// closing my files
	fclose(input_fp);
	fclose(output_fp);
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

void p6_to_p6(FILE *input, FILE *output){
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
	}
	fwrite(buffer, sizeof(Pixel), count, output);
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
	fscanf(input, "%d %d\n%d\n", &width, &height, &depth);
	if(depth > 255){
		printf("Error: Image not an 8 bit channel\n");
		exit(1);
	}
	fprintf(output, "%d %d\n%d\n", width, height, depth);
	
	// reading and writing pixels
	Pixel new;
	Pixel *buffer = malloc(sizeof(Pixel)*width*height);
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

void p3_to_p3(FILE *input, FILE *output){
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
	fscanf(input, "%d %d\n%d\n", &width, &height, &depth);
	if(depth > 255){
		printf("Error: Image not an 8 bit channel\n");
		exit(1);
	}
	fprintf(output, "%d %d\n%d\n", width, height, depth);
	
	// reading and writing pixels
	Pixel new;
	Pixel *buffer = malloc(sizeof(Pixel)*width*height);
	int count = 0;
	while(!feof(input)){
		fscanf(input, "%hhu ", &new.red);
		fscanf(input, "%hhu ", &new.green);
		fscanf(input, "%hhu ", &new.blue);
		buffer[count] = new;
		fprintf(output, "%i %i %i ", new.red, new.green, new.blue);
		count++;
	}
}

// recursive comment excluding file
FILE* escape_comments(FILE *input, int c){
	if(c == '#'){
		printf("Traversing comments...\n");
		while(c != '\n'){
			c = fgetc(input);
		}
		printf("Escaped comments\n");
		c = fgetc(input);
		input = escape_comments(input, c);
	}
	else{
		fseek(input, -1, SEEK_CUR);
	}
	return input;
}
