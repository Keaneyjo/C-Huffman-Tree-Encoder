// C code file for  a file ADT where we can read a single bit at a
// time, or write a single bit at a time

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "bitfile.h"
#define BIT_MAX 7
#define BIT_MIN 0

// open a bit file in "r" (read) mode or "w" (write) mode
//readmode 1=read, 0=write
struct bitfile *bitfile_open(char *filename, char *mode)
{
    struct bitfile *theFile = malloc(sizeof(struct bitfile)); //make some space
    theFile->file = fopen(filename, mode);                    //open the file
    if (!strcmp("r", mode))                                   //make sure the read type is valid
    {
        theFile->buffer = fgetc(theFile->file); //get the first character
        theFile->index = BIT_MIN;               //set index
    }
    else if (!strcmp("w", mode)) //make sure write type is valid
    {
        theFile->index = BIT_MAX; //set index
        theFile->buffer = 0;      //clear buffer
    }
    else
    {
        fprintf(stderr, "ERROR! Mode must be 'r' or 'w'"); //tell the user they messed up
        assert(1 == 0);                                    //Crash
    }
    theFile->is_read_mode = (!(*mode & 1)); //set boolean (r and w end with different bits)

    return theFile;
}

// write a bit to a file; the file must have been opened in write mode
void bitfile_write_bit(struct bitfile *this, int bit)
{
    assert(!this->is_read_mode);                        //check to make sure this is safe
    this->buffer |= (bit << (BIT_MAX - this->index--)); //add the bit to the buffer
    if (this->index < BIT_MIN)                          //if at the end of the buffer, write buffer to file and reset
    {
        this->index = BIT_MAX;
        fputc(this->buffer, this->file);
        this->buffer = 0;
    }
}

// read a bit from a file; the file must have been opened in read mode
int bitfile_read_bit(struct bitfile *this)
{
    assert(this->is_read_mode); //check this is a valid operation
    if (this->index > BIT_MAX)  //if at the end of the buffer, reset and get next char
    {
        this->buffer = 0;
        this->buffer = fgetc(this->file);
        this->index = BIT_MIN;
    }
    return ((this->buffer & (1 << this->index)) >> (this->index)++); //return the bit
}

// close a bitfile; flush any partially-filled buffer if file is open in write mode
void bitfile_close(struct bitfile *this)
{
    if (!this->is_read_mode && this->index != BIT_MAX)
        fputc(this->buffer, this->file);
    fclose(this->file);
    free(this);
}

// check for end of file, didn't actually use this
int bitfile_end_of_file(struct bitfile *this)
{
    if (this->is_read_mode && (int)this->buffer == EOF)
        return 1;
    return 0;
}
