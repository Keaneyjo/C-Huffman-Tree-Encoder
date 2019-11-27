// C code file for  a file ADT where we can read a single bit at a
// time, or write a single bit at a time

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "bitfile.h"

/*
struct bitfile {
  FILE * file;
  unsigned char buffer;
  int index;
  int is_read_mode;
  int is_EOF;
};
*/

/*
file = fopen(argv[1], "r");
assert( file != NULL );
c = fgetc(file);	// attempt to read a byte
while( !feof(file) ) {
printf("%c", c);
c = fgetc(file);
}
fclose(file);
*/

	//    int fputc(int c, FILE * file); // writes a single byte to file
	//    int fgetc (FILE * file);       // reads a single byte from a file

// open a bit file in "r" (read) mode or "w" (write) mode
struct bitfile * bitfile_open(char * filename, char * mode)
{ 
	struct bitfile * result;
	result = calloc(1, sizeof(struct bitfile)); 
	result->file = fopen(filename, mode);
	result->buffer = 0;
	result->index = 0;
	if(atoi(mode) == (int) 'r')
		result->is_read_mode = 1;
	else
		result->is_read_mode = 0;
	result->is_EOF = 0;
	return result;
}

// write a bit to a file; the file must have been opened in write mode
void bitfile_write_bit(struct bitfile * this, int bit)
{
	if(this->is_EOF == 0)
	{
		if(this->is_read_mode == 0)
		{
			if(this->index < 8)
			{
				this->buffer = this->buffer + bit;
				this->buffer = this->buffer << 1;
				this->index++;
			}
			else
			{
				this->index = 0;
				fputc(this->buffer, this->file);
				this->buffer = 0;	
			}
		}
		else
		{
			printf("You got an error boi, write_bit");
		}
	}
}

// read a bit from a file; the file must have been opened in read mode
int bitfile_read_bit(struct bitfile * this)
{
	if(this->is_read_mode == 1)
	{

	}
	else
	{
		printf("You got an error boi; read_bit");
	}
	return 0;
}

// close a bitfile; flush any partially-filled buffer if file is open
// in write mode
void bitfile_close(struct bitfile * this) 
{
	//flush remaining bits
	fputc(this->buffer, this->file);
	fclose(this->file);
}

// check for end of file
int bitfile_end_of_file(struct bitfile * this)
{
	if(feof(this->file))
	{
		this->is_EOF = 1;
		return 1;
	}
	else return 0;
}
