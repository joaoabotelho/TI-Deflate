/* Author: Rui Pedro Paiva
   Teoria da Informação, LEI, 2007/2008*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "huffman.h"
#include <limits.h>

#define CHAR_SIZE 1024
// Gzip header variables
typedef struct header
{
  //elementos fixos
  unsigned char ID1, ID2, CM, XFL, OS;
  unsigned long MTIME;
  unsigned char FLG_FTEXT, FLG_FHCRC, FLG_FEXTRA, FLG_FNAME, FLG_FCOMMENT;   //bits 0, 1, 2, 3 e 4, respectivamente (restantes 3: reservados)

  // FLG_FTEXT --> ignorado deliberadamente (tipicamente igual a 0)
  //se FLG_FEXTRA == 1
  unsigned char xlen;
  unsigned char *extraField;

  //se FLG_FNAME == 1
  char *fName;  //terminada por um byte a 0

  //se FLG_FCOMMENT == 1
  char *fComment; //terminada por um byte a 0

  //se FLG_HCRC == 1
  unsigned char *HCRC;
} gzipHeader;

typedef struct block *block_p;
typedef struct block{
  char len;
  unsigned int dec;
  char *bin;
} Block;

typedef struct data_info *data_info_p;
typedef struct data_info{
    unsigned int bits;
    unsigned int shift;
}Data_info;

unsigned long data_read;
unsigned char *content;
data_info_p lengths;
data_info_p dists;
char *fileName;
long origFileSize;

int cmpfunc (const void * a, const void * b);
long getOrigFileSize(FILE *gzFile);
int getHeader(FILE *gzFile, gzipHeader *gzh);
int isDynamicHuffman(unsigned char rb);
void bits2String(char *strBits, int byte, int size);
char updateBitBuffer(FILE *f, char availBits, char needBits, unsigned int *rb);
int getCodeLen(int *symbols, FILE *f, char availBits, unsigned int *rb, char HCLEN, block_p head);
void print_blocks(block_p head, int size);
void getBin(HuffmanTree *ht, block_p head, int size);
void read_huffman_lit(int hlit_cod_comp[], HuffmanTree* Root_cod_comp, char* needBits,char * availBits, unsigned char* byte, unsigned int * rb, int num_codes, FILE* gzFile);
int mask(int numBits);
unsigned int getBits(FILE *f, char *availBits, unsigned int needBits, unsigned int *rb);
void getBlock(FILE *f, char *availBits, char needBits, unsigned int *rb, int iter, block_p head, int* sequence);
void getHuffmanCodes(HuffmanTree *ht, block_p head, int size);
int *getUniqueValues(block_p head, int size, int *unique_vals_size);
void getBlockHuffmanCode(FILE *f, block_p head, HuffmanTree *ht, char *availBits, unsigned int *rb, int iter);
void read_data(FILE *f, HuffmanTree *comp_lit, HuffmanTree *dist, char *availBits, unsigned int *rb);
void write_to_file(unsigned int size);
void free_tree(HFNode *node);
char *getOrigFileName();
