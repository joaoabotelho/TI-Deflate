/*Author: Rui Pedro Paiva
  Teoria da Informação, LEI, 2007/2008*/

#include <cstdlib>

#include "gzip.h"

void getBin(HuffmanTree *ht, block_p head, int size);

//função principal, a qual gere todo o processo de descompactação
int main(int argc, char** argv)
{
    //--- Gzip file management variables
    FILE *gzFile;  //ponteiro para o ficheiro a abrir
    long fileSize;
    long origFileSize;
    int numBlocks = 0;
    gzipHeader gzh;
    unsigned char byte;  //variável temporária para armazenar um byte lido directamente do ficheiro
    unsigned int rb = 0;  //último byte lido (poderá ter mais que 8 bits, se tiverem sobrado alguns de leituras anteriores)
    char needBits = 0, availBits = 0;

    lengths = (data_info_p)malloc(28 * sizeof(Data_info));
    lengths[0].bits = 0;
    lengths[0].shift = 3;
    lengths[1].bits = 0;
    lengths[1].shift = 4;
    lengths[2].bits = 0;
    lengths[2].shift = 5;
    lengths[3].bits = 0;
    lengths[3].shift = 6;
    lengths[4].bits = 0;
    lengths[4].shift = 7;
    lengths[5].bits = 0;
    lengths[5].shift = 8;
    lengths[6].bits = 0;
    lengths[6].shift = 9;
    lengths[7].bits = 0;
    lengths[7].shift = 10;
    lengths[8].bits = 1;
    lengths[8].shift = 11;
    lengths[9].bits = 1;
    lengths[9].shift = 13;
    lengths[10].bits = 1;
    lengths[10].shift = 15;
    lengths[11].bits = 1;
    lengths[11].shift = 17;
    lengths[12].bits = 2;
    lengths[12].shift = 19;
    lengths[13].bits = 2;
    lengths[13].shift = 23;
    lengths[14].bits = 2;
    lengths[14].shift = 27;
    lengths[15].bits = 2;
    lengths[15].shift = 31;
    lengths[16].bits = 3;
    lengths[16].shift = 35;
    lengths[17].bits = 3;
    lengths[17].shift = 43;
    lengths[18].bits = 3;
    lengths[18].shift = 51;
    lengths[19].bits = 3;
    lengths[19].shift = 59;
    lengths[20].bits = 4;
    lengths[20].shift = 67;
    lengths[21].bits = 4;
    lengths[21].shift = 83;
    lengths[22].bits = 4;
    lengths[22].shift = 99;
    lengths[23].bits = 4;
    lengths[23].shift = 115;
    lengths[24].bits = 5;
    lengths[24].shift = 131;
    lengths[25].bits = 5;
    lengths[25].shift = 163;
    lengths[26].bits = 5;
    lengths[26].shift = 195;
    lengths[27].bits = 5;
    lengths[27].shift = 227;
    lengths[28].bits = 0;
    lengths[28].shift = 258;

    dists = (data_info_p)malloc(sizeof(Data_info) * 30);
    dists[0].bits = 0;
    dists[0].shift = 1;
    dists[1].bits = 0;
    dists[1].shift = 2;
    dists[2].bits = 0;
    dists[2].shift = 3;
    dists[3].bits = 0;
    dists[3].shift = 4;
    dists[4].bits = 1;
    dists[4].shift = 5;
    dists[5].bits = 1;
    dists[5].shift = 7;
    dists[6].bits = 2;
    dists[6].shift = 9;
    dists[7].bits = 2;
    dists[7].shift = 13;
    dists[8].bits = 3;
    dists[8].shift = 17;
    dists[9].bits = 3;
    dists[9].shift = 25;
    dists[10].bits = 4;
    dists[10].shift = 33;
    dists[11].bits = 4;
    dists[11].shift = 49;
    dists[12].bits = 5;
    dists[12].shift = 65;
    dists[13].bits = 5;
    dists[13].shift = 97;
    dists[14].bits = 6;
    dists[14].shift = 129;
    dists[15].bits = 6;
    dists[15].shift = 193;
    dists[16].bits = 7;
    dists[16].shift = 257;
    dists[17].bits = 7;
    dists[17].shift = 385;
    dists[18].bits = 8;
    dists[18].shift = 513;
    dists[19].bits = 8;
    dists[19].shift = 769;
    dists[20].bits = 9;
    dists[20].shift = 1025;
    dists[21].bits = 9;
    dists[21].shift = 1537;
    dists[22].bits = 10;
    dists[22].shift = 2049;
    dists[23].bits = 10;
    dists[23].shift = 3073;
    dists[24].bits = 11;
    dists[24].shift = 4097;
    dists[25].bits = 11;
    dists[25].shift = 6145;
    dists[26].bits = 12;
    dists[26].shift = 8193;
    dists[27].bits = 12;
    dists[27].shift = 12289;
    dists[28].bits = 13;
    dists[28].shift = 16385;
    dists[29].bits = 13;
    dists[29].shift = 24577;

    //--- obter ficheiro a descompactar
    //char fileName[] = "FAQ.txt.gz";

    if (argc != 2)
    {
        printf("Linha de comando invalida!!!");
        return -1;
    }
    char * fileName = argv[1];

    //--- processar ficheiro
    gzFile = fopen(fileName, "rb");
    fseek(gzFile, 0L, SEEK_END);
    fileSize = ftell(gzFile);
    fseek(gzFile, 0L, SEEK_SET);

    //ler tamanho do ficheiro original (acrescentar: e definir Vector com símbolos
    origFileSize = getOrigFileSize(gzFile);


    //--- ler cabeçalho
    int erro = getHeader(gzFile, &gzh);
    if (erro != 0)
    {
        printf ("Formato invalido!!!");
        return -1;
    }

    //--- Para todos os blocos encontrados
    char BFINAL;

    do
    {
        //--- ler o block header: primeiro byte depois do cabeçalho do ficheiro
        needBits = 3;
        if (availBits < needBits)
        {
            fread(&byte, 1, 1, gzFile);
            rb = (byte << availBits) | rb;
            availBits += 8;
        }

        //obter BFINAL
        //ver se é o último bloco
        BFINAL = rb & 0x01; //primeiro bit é o menos significativo
        printf("BFINAL = %d\n", BFINAL);
        rb = rb >> 1; //descartar o bit correspondente ao BFINAL
        availBits -=1;

        //analisar block header e ver se é huffman dinâmico
        if (!isDynamicHuffman(rb))  //ignorar bloco se não for Huffman dinâmico
            continue;
        rb = rb >> 2; //descartar os 2 bits correspondentes ao BTYPE
        availBits -= 2;

        //mine//

        char HLIT = getBits(gzFile, &availBits, 5, &rb);
        printf("HLIT -> %d\n", HLIT);

        char HDIST = getBits(gzFile, &availBits, 5, &rb);
        printf("HDIST -> %d\n", HDIST);

        char HCLEN = getBits(gzFile, &availBits, 4, &rb);
        printf("HCLEN -> %d\n", HCLEN);

        block_p first_block = (block_p)calloc(19, sizeof(Block));

        int reading_sequence[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
        //availBits = getCodeLen(reading_sequence, gzFile, availBits, &rb, HCLEN, first_block);
        getBlock(gzFile, &availBits, 3, &rb, HCLEN + 4, first_block, reading_sequence);

        HuffmanTree *first_ht = createHFTree();
        getHuffmanCodes(first_ht, first_block, 19);
        //getBin(ht, first_block, 19);

        printf("\n############## FIRST BLOCK ##############\n\n");
        print_blocks(first_block, 19);

        block_p second_block = (block_p)calloc(HLIT + 257, sizeof(Block));
        getBlockHuffmanCode(gzFile, second_block, first_ht, &availBits, &rb, HLIT + 257);

        HuffmanTree *second_ht = createHFTree();
        getHuffmanCodes(second_ht, second_block, HLIT + 257);

        printf("\n############## SECOND BLOCK ##############\n\n");
        print_blocks(second_block, HLIT + 257);

        block_p third_block = (block_p)calloc(HDIST + 1, sizeof(Block));
        getBlockHuffmanCode(gzFile, third_block, first_ht, &availBits, &rb, HDIST + 1);

        HuffmanTree *third_ht = createHFTree();
        getHuffmanCodes(third_ht, third_block, HDIST + 1);

        printf("\n############## THIRD BLOCK ##############\n\n");
        print_blocks(third_block, HDIST + 1);

        read_data(gzFile, second_ht, third_ht, &availBits, &rb);

        //int i;
        //for(i = 0; i < 257 + HLIT; i++){
        //printf("%d -> %d\n", i, hlit_cod_comp[i]);
        //}
        //--- Se chegou aqui --> compactado com Huffman dinâmico --> descompactar
        //**************************************************
        //****** ADICIONAR PROGRAMA... *********************
        //**************************************************

        //actualizar número de blocos analisados
        numBlocks++;
    }while(BFINAL == 0);


    //terminações
    fclose(gzFile);
    printf("End: %d bloco(s) analisado(s).\n", numBlocks);


    //teste da função bits2String: RETIRAR antes de criar o executável final
    char str[9];
    bits2String(str, 0x03, 8);
    printf("%s\n", str);


    //RETIRAR antes de criar o executável final
    system("PAUSE");
    return EXIT_SUCCESS;
}


char getBits(FILE *f, char *availBits, char needBits, unsigned int *rb){
    char result;
    unsigned char byte;
    while(*availBits < needBits){
        fread(&byte, 1, 1, f);
        *rb = (byte << (*availBits)) | *rb;
        *availBits += 8;
    }

    result = *rb & mask(needBits);
    *rb = *rb >> needBits;
    *availBits -= needBits;
    return result;
}

int mask(int numBits){
    //int mask = 1;
    //mask = mask << numBits;
    //return mask - 1;
    return (1 << numBits) - 1;
}

void getBlock(FILE *f, char *availBits, char needBits, unsigned int *rb, int iter, block_p head, int* sequence){
    int i;

    for(i = 0; i < iter; i++){
        head[sequence[i]].len = getBits(f, availBits, 3, rb);
    }
}

void getHuffmanCodes(HuffmanTree *ht, block_p head, int size){
    int i, j, k, err;
    int unique_vals_size = 0;
    int *unique_vals;
    int counter = 0;
    int last;
    block_p head_cp = head;

    unique_vals = getUniqueValues(head, size, &unique_vals_size);

    last = unique_vals[0];
    for(i = 0; i < unique_vals_size; i++){
        for(k = 0; k < unique_vals[i] - last - 1; k++)
            counter*=2;
        for(j = 0; j < size; j++){
            if(head[j].len == unique_vals[i]){
                head[j].bin = (char*)malloc(head[j].len * sizeof(char));
                head[j].dec = counter++;
                bits2String(head[j].bin, head[j].dec, head[j].len);
                if((err = addNode(ht, head[j].bin, j, 0)) < 0)
                    printf("error: %d\n", err);
            }
        }
        counter *= 2;
        last = unique_vals[i];
    }
    return;
}

int *getUniqueValues(block_p head, int size, int *unique_vals_size){
    int i, j, flag;
    int *unique = (int*)malloc(sizeof(int) * size);
    int *resized;

    for(i = 0; i < size; i++){
        if(head[i].len == 0)
            continue;
        flag = 0;
        for(j = 0; j < *unique_vals_size; j++){
            if(head[i].len == unique[j]){
                flag = 1;
                break;
            }
        }
        if(!flag)
            unique[(*unique_vals_size)++] = head[i].len;
    }

    resized = (int*)malloc(sizeof(int) * *unique_vals_size);
    memcpy(resized, unique, *unique_vals_size * sizeof(int));
    free(unique);
    qsort(resized, *unique_vals_size, sizeof(int), cmpfunc);
    return resized;
}

void getBlockHuffmanCode(FILE *f, block_p head, HuffmanTree *ht, char *availBits, unsigned int *rb, int iter){
    int i, j, n, val, n_repetitions;
    char bit;

    for(i = 0; i < iter;){
        bit = getBits(f, availBits, 1, rb) + '0';
        n = nextNode(ht, bit);

        if(n >= 0){
            val = 0;
            switch(n){
                case 16:
                    val = head[i-1].len;
                    n_repetitions = getBits(f, availBits, 2, rb) + 3;
                    break;

                case 17:
                    n_repetitions = getBits(f, availBits, 3, rb) + 3;
                    break;

                case 18:
                    n_repetitions = getBits(f, availBits, 7, rb) + 11;
                    break;

                default:
                    n_repetitions = 1;
                    val = n;
                    break;
            }
            for(j = 0; j < n_repetitions; j++, i++)
                head[i].len = val;
            resetCurNode(ht);
        }
    }

    resetCurNode(ht);
    return;
}

void read_data(FILE *f, HuffmanTree *comp_lit, HuffmanTree *distances, char *availBits, unsigned int *rb){
    int n, i, j, k, lenght, backwards_distance;
    char bit;
    unsigned char arr[getOrigFileSize(f)];

    i = 0;
    while(1){
        bit = getBits(f, availBits, 1, rb) + '0';
        n = nextNode(comp_lit, bit);

        if(n >= 0){
            if(n == 256){
                printf("%s", arr);
                printf("\nEND\n");
                return;
            }

            if(n < 257)
                arr[i++] = (char)n;

            else{
                lenght = getBits(f, availBits, lengths[n-257].bits, rb) + lengths[n-257].shift;

                do{
                    bit = getBits(f, availBits, 1, rb) + '0';
                    n = nextNode(distances, bit);
                }while(n < 0);

                backwards_distance = getBits(f, availBits, dists[n].bits, rb) + dists[n].shift;

                k = i;
                for(j = k - backwards_distance; j < k - backwards_distance + lenght; j++){
                    arr[i++] = arr[j];
                }
            }
            resetCurNode(distances);
            resetCurNode(comp_lit);
            //getchar();
        }
    }
    return;
}

int cmpfunc (const void * a, const void * b) {
    return ( *(int*)a - *(int*)b );
}

void print_blocks(block_p head, int size){
    int i;

    for(i = 0; i < size; i++){
        printf("%d -> %d %d %s\n", i, head[i].len, head[i].dec, head[i].bin);
    }
}
//---------------------------------------------------------------
//Lê o cabeçalho do ficheiro gzip: devolve erro (-1) se o formato for inválidodevolve, ou 0 se ok
int getHeader(FILE *gzFile, gzipHeader *gzh) //obtém cabeçalho
{
    unsigned char byte;

    //Identicação 1 e 2: valores fixos
    fread(&byte, 1, 1, gzFile);
    (*gzh).ID1 = byte;
    if ((*gzh).ID1 != 0x1f) return -1; //erro no cabeçalho

    fread(&byte, 1, 1, gzFile);
    (*gzh).ID2 = byte;
    if ((*gzh).ID2 != 0x8b) return -1; //erro no cabeçalho

    //Método de compressão (deve ser 8 para denotar o deflate)
    fread(&byte, 1, 1, gzFile);
    (*gzh).CM = byte;
    if ((*gzh).CM != 0x08) return -1; //erro no cabeçalho

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
        //1º byte: LSB, 2º: MSB
        char lenXLEN = 2;

        fread(&byte, 1, 1, gzFile);
        (*gzh).xlen = byte;
        fread(&byte, 1, 1, gzFile);
        (*gzh).xlen = (byte << 8) + (*gzh).xlen;

        (*gzh).extraField = new unsigned char[(*gzh).xlen];

        //ler extra field (deixado como está, i.e., não processado...)
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
            if (i <= 1023)  //guarda no máximo 1024 caracteres no array
                (*gzh).fName[i] = byte;
            i++;
        }while(byte != 0);
        if (i > 1023)
            (*gzh).fName[1023] = 0;  //apesar de nome incompleto, garantir que o array termina em 0
    }
    else
        (*gzh).fName = 0;

    //FLG_FCOMMENT: ler comentário
    if ((*gzh).FLG_FCOMMENT == 1)
    {
        (*gzh).fComment = new char[1024];
        unsigned int i = 0;
        do
        {
            fread(&byte, 1, 1, gzFile);
            if (i <= 1023)  //guarda no máximo 1024 caracteres no array
                (*gzh).fComment[i] = byte;
            i++;
        }while(byte != 0);
        if (i > 1023)
            (*gzh).fComment[1023] = 0;  //apesar de comentário incompleto, garantir que o array termina em 0
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
//Analisa block header e vê se é huffman dinâmico
int isDynamicHuffman(unsigned char rb)
{
    unsigned char BTYPE = rb & 0x03;

    if (BTYPE == 0) //--> sem compressão
    {
        printf("Ignorando bloco: sem compactacao!!!\n");
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
//Obtém tamanho do ficheiro original
long getOrigFileSize(FILE * gzFile)
{
    //salvaguarda posição actual do ficheiro
    long fp = ftell(gzFile);

    //últimos 4 bytes = ISIZE;
    fseek(gzFile, -4, SEEK_END);

    //determina ISIZE (só correcto se cabe em 32 bits)
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
void bits2String(char *strBits, int byte, int size)
{
    char mask = 0x01;  //get LSbit

    strBits[size] = '\0';
    for (char bit, i = size - 1; i >= 0; i--)
    {
        bit = byte & mask;
        strBits[i] = bit + 48; //converter valor numérico para o caracter alfanumérico correspondente
        byte = byte >> 1;
    }
}
