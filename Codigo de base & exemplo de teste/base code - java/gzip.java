/* Author: Rui Pedro Paiva
Teoria da Informa��o, LEI, 2006/2007*/

import java.io.*;
import java.util.*;

//class principal para leitura de um ficheiro gzip
//M�todos:
//gzip(String fileName) throws IOException --> construtor
//int getHeader() throws IOException --> l� o cabe�alho do ficheiro para um objecto da class gzipHeader
//void closeFiles() throws IOException --> fecha os ficheiros
//String bits2String(byte b) --> converte um byte para uma string com a sua representa��o bin�ria
public class gzip
{
	static gzipHeader gzh;
	static String gzFile;
	static long fileSize;
	static long origFileSize;
	static int numBlocks = 0;
	static RandomAccessFile is;
	static int rb = 0, needBits = 0, availBits = 0;		

	//fun��o principal, a qual gere todo o processo de descompacta��o
	public static void main (String args[])
	{			
		//--- obter ficheiro a descompactar
		String fileName = "FAQ.txt.gz";
		/*if (args.length != 1)
		{
			System.out.println("Linha de comando inv�lida!!!");
			return;
		}
		String fileName = args[0];*/			
				
		//--- processar ficheiro
		try
		{
			gzip gz = new gzip(fileName);
			//System.out.println(fileSize);
			
			//ler tamanho do ficheiro original e definir Vector com s�mbolos
			origFileSize = getOrigFileSize();
			System.out.println(origFileSize);
			
			//--- ler cabe�alho
			int erro = getHeader();
			if (erro != 0)
			{
				System.out.println ("Formato inv�lido!!!");
				return;
			}
			//else				
			//	System.out.println(gzh.fName);
			
			
			//--- Para todos os blocos encontrados
			int BFINAL;	
			
			do
			{				
				//--- ler o block header: primeiro byte depois do cabe�alho do ficheiro
				needBits = 3;
				if (availBits < needBits)
				{
					rb = is.readUnsignedByte() << availBits | rb;
					availBits += 8;
				}
				
				//obter BFINAL
				//ver se � o �ltimo bloco
				BFINAL = (byte) (rb & 0x01); //primeiro bit � o menos significativo
				System.out.println("BFINAL = " + BFINAL);
								
				//analisar block header e ver se � huffman din�mico					
				if (!isDynamicHuffman(rb))  //ignorar bloco se n�o for Huffman din�mico
					continue;
				
				//descartar os 3 bits correspondentes ao tipo de bloco
				rb = rb >> 3;
				availBits -= 3;
										
				//--- Se chegou aqui --> compactado com Huffman din�mico --> descompactar
				//adicionar programa...				
																																												
				//actualizar n�mero de blocos analisados
				numBlocks++;				
			}while(BFINAL == 0);
						

			//termina��es			
			is.close();	
			System.out.println("End: " + numBlocks + " bloco(s) analisado(s).");
		}
		catch (IOException erro)
		{
			System.out.println("Erro ao usar o ficheiro!!!");
			System.out.println(erro);
		}
	}

	
	//Construtor: recebe nome do ficheiro a descompactar e cria File Streams
	gzip(String fileName) throws IOException
	{
		gzFile = fileName;
		is = new RandomAccessFile(fileName, "r");
		fileSize = is.length();
	}
	
	
	//Obt�m tamanho do ficheiro original
	public static long getOrigFileSize() throws IOException
	{
		//salvaguarda posi��o actual do ficheiro
		long fp = is.getFilePointer();
		
		//�ltimos 4 bytes = ISIZE;
		is.seek(fileSize-4);
		
		//determina ISIZE (s� correcto se cabe em 32 bits)
		long sz = 0;
		sz = is.readUnsignedByte();
		for (int i = 0; i <= 2; i++)
			sz = (is.readUnsignedByte() << 8*(i+1)) + sz;			
		
		//restaura file pointer
		is.seek(fp);
		
		return sz;		
	}
		

	//L� o cabe�alho do ficheiro gzip: devolve erro se o formato for inv�lido
	public static int getHeader() throws IOException  //obt�m cabe�alho
	{
		gzh = new gzipHeader();
		
		int erro = gzh.read(is);
		if (erro != 0) return erro; //formato inv�lido		
		
		return 0;
	}
		
	
	//Analisa block header e v� se � huffman din�mico
	public static boolean isDynamicHuffman(int k)
	{				
		byte BTYPE = (byte) ((k & 0x06) >> 1);
						
		if (BTYPE == 0) //--> sem compress�o
		{
			System.out.println("Ignorando bloco: sem compacta��o!!!");
			return false;
		}
		else if (BTYPE == 1)
		{
			System.out.println("Ignorando bloco: compactado com Huffman fixo!!!");
			return false;
		}
		else if (BTYPE == 3)
		{
			System.out.println("Ignorando bloco: BTYPE = reservado!!!");
			return false;
		}
		else
			return true;
		
	}
	
	
	//Converte um byte para uma string com a sua representa��o bin�ria
	public static String bits2String(byte b)
	{
		String strBits = "";
		byte mask = 0x01;  //get LSbit
		
		for (byte bit, i = 1; i <= 8; i++)
		{
			bit = (byte)(b & mask);
			strBits = bit + strBits; //add bit to the left, since LSb first
			b >>= 1;
		}
		return strBits;		
	}
}