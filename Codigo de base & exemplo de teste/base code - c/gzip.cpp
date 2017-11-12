/*Author: Rui Pedro Paiva
Teoria da Informa��o, LEI, 2007/2008*/

#include <cstdlib>

#include "gzip.h"

//fun��o principal, a qual gere todo o processo de descompacta��o
int main(int argc, char** argv)
{
	//--- Gzip file management variables
	FILE *gzFile;  //ponteiro para o ficheiro a abrir
	long fileSize;
	long origFileSize;
	int numBlocks = 0;	
	gzipHeader gzh;
	unsigned char byte;  //vari�vel tempor�ria para armazenar um byte lido directamente do ficheiro
	unsigned int rb = 0;  //�ltimo byte lido (poder� ter mais que 8 bits, se tiverem sobrado alguns de leituras anteriores)
	char needBits = 0, availBits = 0;		



	//--- obter ficheiro a descompactar
	//char fileName[] = "FAQ.txt.gz";

	if (argc != 2)
	{
		printf("Linha de comando inv�lida!!!");
		return -1;
	}
	char * fileName = argv[1];

	//--- processar ficheiro
	gzFile = fopen(fileName, "r");
	fseek(gzFile, 0L, SEEK_END);
	fileSize = ftell(gzFile);
	fseek(gzFile, 0L, SEEK_SET);

	//ler tamanho do ficheiro original (acrescentar: e definir Vector com s�mbolos
	origFileSize = getOrigFileSize(gzFile);


	//--- ler cabe�alho
	int erro = getHeader(gzFile, &gzh);
	if (erro != 0)
	{
		printf ("Formato inv�lido!!!");
		return -1;
	}

	//--- Para todos os blocos encontrados
	char BFINAL;	
	
	do
	{				
		//--- ler o block header: primeiro byte depois do cabe�alho do ficheiro
		needBits = 3;
		if (availBits < needBits)
		{
			fread(&byte, 1, 1, gzFile);
			rb = (byte << availBits) | rb;
			availBits += 8;
		}
		
		//obter BFINAL
		//ver se � o �ltimo bloco
		BFINAL = rb & 0x01; //primeiro bit � o menos significativo
		printf("BFINAL = %d\n", BFINAL);
		rb = rb >> 1; //descartar o bit correspondente ao BFINAL
		availBits -=1;
						
		//analisar block header e ver se � huffman din�mico					
		if (!isDynamicHuffman(rb))  //ignorar bloco se n�o for Huffman din�mico
			continue;				
		rb = rb >> 2; //descartar os 2 bits correspondentes ao BTYPE
		availBits -= 2;

		char HLIT;
		availBits = updateBitBuffer(gzFile, availBits, 5, &rb);
		HLIT = rb & 0x1F;
		rb = rb >> 5;
		availBits -= 5;
		printf("HLIT -> %d\n", HLIT);
								
		char HDIST;
		availBits = updateBitBuffer(gzFile, availBits, 5, &rb);
		HDIST = rb & 0x1F;
		rb = rb >> 5;
		availBits -= 5;
		printf("HDIST -> %d\n", HDIST);
		
		char HCLEN;
		availBits = updateBitBuffer(gzFile, availBits, 4, &rb);
		HCLEN = rb & 0xF;
		rb = rb >> 4;
		availBits -= 4;
		printf("HCLEN -> %d\n", HCLEN);

		block_p head = NULL;

		getCodeLen(gzFile, availBits, &rb, HCLEN, &head);
		
		print_blocks(head);
		//--- Se chegou aqui --> compactado com Huffman din�mico --> descompactar
		//**************************************************
		//****** ADICIONAR PROGRAMA... *********************
		//**************************************************
        
        																																										
		//actualizar n�mero de blocos analisados
		numBlocks++;
	}while(BFINAL == 0);


	//termina��es			
	fclose(gzFile);
	printf("End: %d bloco(s) analisado(s).\n", numBlocks);	


    //teste da fun��o bits2String: RETIRAR antes de criar o execut�vel final
	char str[9];
	bits2String(str, 0x03);
	printf("%s\n", str);


    //RETIRAR antes de criar o execut�vel final
    system("PAUSE");
    return EXIT_SUCCESS;
}

char updateBitBuffer(FILE *f, char availBits, char needBits, unsigned int *rb){
	
	unsigned char byte;
	if (availBits < needBits)
	{
		fread(&byte, 1, 1, f);
		*rb = (byte << availBits) | *rb;
		availBits += 8;
	}
	return availBits;
}

int getCodeLen(FILE *f, char availBits, unsigned int *rb, char HCLEN, block_p *head){
	int i;	
	char len;
	int symbols[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

	block_p last_pos = *head;

	for(i = 0; i < 19; i++){
		block_p node = (block_p)malloc(sizeof(Block));
		node -> symbol = symbols[i];
		node -> len = 0;
		node -> next = NULL;

		!*head ? *head = node : (last_pos -> next) = node;
		last_pos = node;
	}

	last_pos = *head;
	for(i = 0; i < HCLEN + 4; i++){
		availBits = updateBitBuffer(f, availBits, 3, rb);
		last_pos -> len = *rb & 0x7;
		last_pos = last_pos -> next;
		*rb = *rb >> 3;
		availBits -= 3;
	}	

	return availBits;
}

void print_blocks(block_p head){
	while(head){
		printf("%d -> %d\n", head -> symbol, head -> len);
		head = head -> next;
	}
}
//---------------------------------------------------------------
//L� o cabe�alho do ficheiro gzip: devolve erro (-1) se o formato for inv�lidodevolve, ou 0 se ok
int getHeader(FILE *gzFile, gzipHeader *gzh) //obt�m cabe�alho
{
	unsigned char byte;

	//Identica��o 1 e 2: valores fixos
	fread(&byte, 1, 1, gzFile);
	(*gzh).ID1 = byte;
	if ((*gzh).ID1 != 0x1f) return -1; //erro no cabe�alho
	
	fread(&byte, 1, 1, gzFile);
	(*gzh).ID2 = byte;
	if ((*gzh).ID2 != 0x8b) return -1; //erro no cabe�alho
	
	//M�todo de compress�o (deve ser 8 para denotar o deflate)
	fread(&byte, 1, 1, gzFile);
	(*gzh).CM = byte;
	if ((*gzh).CM != 0x08) return -1; //erro no cabe�alho
				
	//Flags
	fread(&byte, 1, 1, gzFile);
	unsigned char FLG = byte;
	
	//MTIME
	char lenMTIME = 4;	
	fread(&byte, 1, 1, gzFile);
	(*gzh).MTIME = byte;
	for (int i = 1; i <= lenMTIME - 1; i++)
	{
		fread(&byte, 1, 1, gzFile);
		(*gzh).MTIME = (byte << 8) + (*gzh).MTIME;				
	}
					
	//XFL (not processed...)
	fread(&byte, 1, 1, gzFile);
	(*gzh).XFL = byte;
	
	//OS (not processed...)
	fread(&byte, 1, 1, gzFile);
	(*gzh).OS = byte;
	
	//--- Check Flags
	(*gzh).FLG_FTEXT = (char)(FLG & 0x01);
	(*gzh).FLG_FHCRC = (char)((FLG & 0x02) >> 1);
	(*gzh).FLG_FEXTRA = (char)((FLG & 0x04) >> 2);
	(*gzh).FLG_FNAME = (char)((FLG & 0x08) >> 3);
	(*gzh).FLG_FCOMMENT = (char)((FLG & 0x10) >> 4);
				
	//FLG_EXTRA
	if ((*gzh).FLG_FEXTRA == 1)
	{
		//ler 2 bytes XLEN + XLEN bytes de extra field
		//1� byte: LSB, 2�: MSB
		char lenXLEN = 2;

		fread(&byte, 1, 1, gzFile);
		(*gzh).xlen = byte;
		fread(&byte, 1, 1, gzFile);
		(*gzh).xlen = (byte << 8) + (*gzh).xlen;
		
		(*gzh).extraField = new unsigned char[(*gzh).xlen];
		
		//ler extra field (deixado como est�, i.e., n�o processado...)
		for (int i = 0; i <= (*gzh).xlen - 1; i++)
		{
			fread(&byte, 1, 1, gzFile);
			(*gzh).extraField[i] = byte;
		}
	}
	else
	{
		(*gzh).xlen = 0;
		(*gzh).extraField = 0;
	}
	
	//FLG_FNAME: ler nome original	
	if ((*gzh).FLG_FNAME == 1)
	{			
		(*gzh).fName = new char[1024];
		unsigned int i = 0;
		do
		{
			fread(&byte, 1, 1, gzFile);
			if (i <= 1023)  //guarda no m�ximo 1024 caracteres no array
				(*gzh).fName[i] = byte;
			i++;
		}while(byte != 0);
		if (i > 1023)
			(*gzh).fName[1023] = 0;  //apesar de nome incompleto, garantir que o array termina em 0
	}
	else
		(*gzh).fName = 0;
	
	//FLG_FCOMMENT: ler coment�rio
	if ((*gzh).FLG_FCOMMENT == 1)
	{			
		(*gzh).fComment = new char[1024];
		unsigned int i = 0;
		do
		{
			fread(&byte, 1, 1, gzFile);
			if (i <= 1023)  //guarda no m�ximo 1024 caracteres no array
				(*gzh).fComment[i] = byte;
			i++;
		}while(byte != 0);
		if (i > 1023)
			(*gzh).fComment[1023] = 0;  //apesar de coment�rio incompleto, garantir que o array termina em 0
	}
	else
		(*gzh).fComment = 0;

	
	//FLG_FHCRC (not processed...)
	if ((*gzh).FLG_FHCRC == 1)
	{			
		(*gzh).HCRC = new unsigned char[2];
		fread(&byte, 1, 1, gzFile);
		(*gzh).HCRC[0] = byte;
		fread(&byte, 1, 1, gzFile);
		(*gzh).HCRC[1] = byte;		
	}
	else
		(*gzh).HCRC = 0;
	
	return 0;
}


//---------------------------------------------------------------
//Analisa block header e v� se � huffman din�mico
int isDynamicHuffman(unsigned char rb)
{				
	unsigned char BTYPE = rb & 0x03;
					
	if (BTYPE == 0) //--> sem compress�o
	{
		printf("Ignorando bloco: sem compacta��o!!!\n");
		return 0;
	}
	else if (BTYPE == 1)
	{
		printf("Ignorando bloco: compactado com Huffman fixo!!!\n");
		return 0;
	}
	else if (BTYPE == 3)
	{
		printf("Ignorando bloco: BTYPE = reservado!!!\n");
		return 0;
	}
	else
		return 1;		
}


//---------------------------------------------------------------
//Obt�m tamanho do ficheiro original
long getOrigFileSize(FILE * gzFile)
{
	//salvaguarda posi��o actual do ficheiro
	long fp = ftell(gzFile);
	
	//�ltimos 4 bytes = ISIZE;
	fseek(gzFile, -4, SEEK_END);
	
	//determina ISIZE (s� correcto se cabe em 32 bits)
	unsigned long sz = 0;
	unsigned char byte;
	fread(&byte, 1, 1, gzFile);
	sz = byte;
	for (int i = 0; i <= 2; i++)
	{
		fread(&byte, 1, 1, gzFile);
		sz = (byte << 8*(i+1)) + sz;
	}

	
	//restaura file pointer
	fseek(gzFile, fp, SEEK_SET);
	
	return sz;		
}


//---------------------------------------------------------------
void bits2String(char *strBits, unsigned char byte)
{
	char mask = 0x01;  //get LSbit
	
	strBits[8] = 0;
	for (char bit, i = 7; i >= 0; i--)
	{		
		bit = byte & mask;
		strBits[i] = bit +48; //converter valor num�rico para o caracter alfanum�rico correspondente
		byte = byte >> 1;
	}	
}
