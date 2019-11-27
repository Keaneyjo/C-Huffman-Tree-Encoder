// code for a huffman coder


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "huff.h"
#include "bitfile.h"

#include <string.h>
#include <limits.h>

const char * READ = "r";
const char * WRITE = "w";

// create a new huffcoder structure
struct huffcoder *  huffcoder_new()
{
	struct huffcoder * result = calloc(1, sizeof(struct huffcoder));
	memset(result->freqs, 0, sizeof(result->freqs));
	memset(result->code_lengths, 0, sizeof(result->code_lengths));
	memset(result->codes, 0, sizeof(result->codes));
	//result->tree = malloc(sizeof(struct huffchar)*NUM_CHARS);
	//result->tree;
	return result;
}

// count the frequency of characters in a file; set chars with zero
// frequency to one
void huffcoder_count(struct huffcoder * this, char * filename)
{
	unsigned char c;  // we need the character to be
		          // unsigned to use it as an index
	FILE * file = fopen(filename, "r");
	assert( file != NULL );
	c = fgetc(file);	// attempt to read a byte
	

	while( !feof(file) )
	{
		this->freqs[c] = this->freqs[c] + 1;
		c = fgetc(file);  // Moves to next char	  
	}
	fclose(file);

	// sets those with a frequency of 0 to 1
	for(int i = 0; i < NUM_CHARS; i++)
	{
		
		if(this->freqs[i] == 0)
		{
			this->freqs[i] = 1;
		}
	}
	return;
	
}

int oneRemaining(struct huffchar ** buffer)
{
	int coderCount = 0;
	for(int i = 0; i < NUM_CHARS; i++)
	{
		if(buffer[i] != NULL)
		{
			coderCount++;
		}
	}
	if(coderCount > 1)
		return 0;
	else
		return 1;
}






// using the Huffman tree, build a table of the Huffman codes
// with the huffcoder object

void huffcoder_tree2table(struct huffcoder * this)
{
	recursive_tree_2table(this, this->tree, 0, 0);
	return;
}

void recursive_tree_2table(struct huffcoder * this, struct huffchar * node, unsigned long long curr_code, int len)
{
	if(node->is_compound == 1)
	{
		curr_code = curr_code << 1;
		len++;
		recursive_tree_2table(this, node->u.compound.left, curr_code, len);
		curr_code++;
		recursive_tree_2table(this, node->u.compound.right, curr_code, len);
	}
	else
	{
		
		this->code_lengths[node->seqno] = len;
	  	this->codes[node->seqno] = reverse(curr_code, len);
	}
	return;
}

unsigned long long reverse(unsigned long long curr_code, int code_length)
{	
	unsigned long long result = 0;
	for(int i = 0; i < code_length; i++)  
        { 
            result <<= 1;
            if ((int)(curr_code & 1) == 1) 
                result ^= 1; 
  
            curr_code >>= 1; 
        } 
	return result;
}


// print the Huffman codes for each character in order
void huffcoder_print_codes(struct huffcoder * this)
{
  int i, j;
  char buffer[NUM_CHARS];

  for ( i = 0; i < NUM_CHARS; i++ ) {
    // put the code into a string
    assert(this->code_lengths[i] < NUM_CHARS);
    for ( j = this->code_lengths[i]-1; j >= 0; j--) {
      buffer[j] = ((this->codes[i] >> j) & 1) + '0';
    }
    // don't forget to add a zero to end of string
    buffer[this->code_lengths[i]] = '\0';

    // print the code
    printf("char: %d, freq: %d, code: %s\n", i, this->freqs[i], buffer);;
  }
}

// using the character frequencies build the tree of compound
// and simple characters that are used to compute the Huffman codes
void huffcoder_build_tree(struct huffcoder * this)
{
	//struct huffchar * l = p->u.compound.left;
	//struct huffchar * r = p->u.compound.right;
	//char value = p->u.c;

	struct huffchar * buffer[NUM_CHARS];
	for(int i = 0; i < NUM_CHARS; i++)
	{
		buffer[i] = malloc(sizeof(struct huffchar));
		buffer[i]->freq = this->freqs[i];
		buffer[i]->is_compound = 0;
		buffer[i]->seqno = i;
		buffer[i]->u.c = (char) i;
	}
	int seqCount = NUM_CHARS;

	while(oneRemaining(buffer) == 0)
	{
		int placeOne = 0;
		int placeTwo = 0;
		struct huffchar * lowestOne = malloc(sizeof(struct huffchar));
		struct huffchar * lowestTwo = malloc(sizeof(struct huffchar));
		lowestOne->freq = TMP_MAX;
		lowestTwo->freq = TMP_MAX;

		for(int i = 0; i < NUM_CHARS; i++)
		{
			if(buffer[i] != NULL)
			{
				if(buffer[i]->freq <= lowestOne->freq)
				{
					if(buffer[i]->freq == lowestOne->freq)
					{
						if(buffer[i]->seqno < lowestOne->seqno)
						{
							lowestOne = buffer[i];
							placeOne = i;
						}
					}	
					else
					{
						lowestOne = buffer[i];
						placeOne = i;
					}
				}
			}
		}
		for(int i = 0; i < NUM_CHARS; i++)
		{
			if(buffer[i] != NULL)
			{
				if((buffer[i]->freq <= lowestTwo->freq) && (buffer[i] != lowestOne))
				{	
					if(buffer[i]->freq == lowestTwo->freq)
					{
						if(buffer[i]->seqno < lowestTwo->seqno)
						{
							lowestTwo = buffer[i];
							placeTwo = i;
						}
					}	
					else
					{
						lowestTwo = buffer[i];
						placeTwo = i;
					}
				}

			}
		}


		//printf("l1 %i", lowestOne->freq);
		//printf("   l2  %i\n", lowestTwo->freq);

		struct huffchar * newCompound = malloc(sizeof(struct huffchar));
		newCompound->freq = lowestOne->freq + lowestTwo->freq;
		newCompound->is_compound = 1;
		newCompound->seqno = seqCount++;

		if(lowestOne->freq < lowestTwo->freq)
		{
			newCompound->u.compound.left = lowestOne;
			newCompound->u.compound.right = lowestTwo;
		}
		else if(lowestTwo->freq < lowestOne->freq)
		{
			newCompound->u.compound.left = lowestTwo;
			newCompound->u.compound.right = lowestOne;
		}
		else if(lowestTwo->freq == lowestOne->freq)
		{
			if(lowestOne->seqno < lowestTwo->seqno)
			{
				newCompound->u.compound.left = lowestOne;
				newCompound->u.compound.right = lowestTwo;
			}
			else
			{
				newCompound->u.compound.right = lowestOne;
				newCompound->u.compound.left = lowestTwo;
			}
		}
		buffer[placeOne] = newCompound;
		buffer[placeTwo] = NULL;
	}
	for(int i = 0; i < NUM_CHARS; i++)
	{
		if(buffer[i] != NULL)
		{
			this->tree = buffer[i];
		}
	}
	return;
	


}

// encode the input file and write the encoding to the output file
void huffcoder_encode(struct huffcoder * this, char * input_filename, char * output_filename)
{
	FILE * file =  fopen(input_filename, "r");
	assert(file != NULL);
	struct bitfile * output = bitfile_open(output_filename, "w");
	
	int c;
	int mask = 1;
	unsigned long long chuck;
	while(!feof(file))
	{
		c = fgetc(file);
		chuck =this->codes[c];
		for(int i = 0; i < this->code_lengths[c]; i++)
		{
			bitfile_write_bit(output, mask & chuck);
			chuck >>=1;
		}
	}
}

// decode the input file and write the decoding to the output file
void huffcoder_decode(struct huffcoder * this, char * input_filename, char * output_filename)
{
	FILE * file =  fopen(input_filename, "w");
	assert(file != NULL);
	struct bitfile * input = bitfile_open(output_filename, "r");
	
	struct huffchar * theNode = this->tree;
	while(!bitfile_end_of_file(input))
	{
		int c = bitfile_read_bit(input);
		if(c == 0)
		{
			theNode = theNode->u.compound.left;
		}
		else if(c == 1)
		{
			theNode = theNode->u.compound.right;
		}

		if(theNode->is_compound == 0)
		{
			fputc(theNode->u.c, file);
			theNode = this->tree;
		}
	}
}




/*
	//    int fputc(int c, FILE * file); // writes a single byte to file
	//    int fgetc (FILE * file);       // reads a single byte from a file
	FILE * in_file; = fopen(input_filename, "r");
	File * out_file = fopen(putput_filename, "w");
	assert(in_file != NULL );
	unsigned char c = fgetc(in_file);	// attempt to read a byte
	while( !feof(in_file) ) {
		c = fgetc(in_file);
		fputc(int c, FILE * out_file);
	}
	fclose(in_file);
*/

