#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE * file;
char memory_block [512];
int bytes_ocupados = 0;

char record_buffer [510];
char text_buffer [200];
char file_name [205];

char separa_campo [2] = {13, '\0'};
char termina_registro [2] = {10, '\0'};

void append(char *, char *);
void grava_registro(int);
void remove_last_char(char *);
int retorna_registro(int);
int retorna_campo(int);

int main(void)
{
	//Recebe o nome do arquivo
	//fprintf(stderr,"Qual o nome do arquivo a ser gravado (sem extensão): ");
	fgets(text_buffer, 198, stdin);
	remove_last_char(text_buffer);
	
	//Adiciona a extensão .dat
	strcpy(file_name, text_buffer);
	append(file_name, ".dat");

	//Zeramos o bloco de memoria
	memory_block[0] = '\0';

	//Abrimos o arquivo
	file = fopen(file_name, "w");

	{
		int i;
		fprintf(stderr, "Insira quantos registros serao inseridos : ");
		fgets(text_buffer, 198, stdin);
		i = atoi(text_buffer);
		for(; i != 0; i--)
		{
			//Zeramos o record buffer
			{
				int j;
				for(j=0; j< 510; j++) record_buffer[j] = 0;
			}
			//Recebemos o RA e enviamos ao buffer;
			fprintf(stderr, "Insira o RA : ");
			text_buffer[0] = '\0';
			fgets(text_buffer, 198, stdin);
			remove_last_char(text_buffer);
			append(record_buffer, text_buffer);
			append(record_buffer, separa_campo);

			//Recebemos o nome e enviamos ao buffer;
			//Tem de usar fgets pois pode haver espaços no nome
			fprintf(stderr, "Insira o nome : ");
			text_buffer[0] = '\0';
			fgets(text_buffer, 198, stdin);
			remove_last_char(text_buffer);
			append(record_buffer, text_buffer);
			append(record_buffer, separa_campo);

			//Recebemos o curso e mandamos ao buffer
			fprintf(stderr, "Insira a sigla do curso : ");
			text_buffer[0] = '\0';
			fgets(text_buffer, 198, stdin);
			remove_last_char(text_buffer);
			append(record_buffer, text_buffer);
			append(record_buffer, separa_campo);

			//Recebemos o ano e mandamos ao buffer
			fprintf(stderr, "Insira o ano : ");
			text_buffer[0] = '\0';
			fgets(text_buffer, 198, stdin);
			remove_last_char(text_buffer);
			append(record_buffer, text_buffer);
			append(record_buffer, separa_campo);

			//Inserimos o final de arquivo
			append(record_buffer, termina_registro);
			//Grava o registro no bloco de memoria
			grava_registro(0);
		}
	}
	if(memory_block[0] != '\0') grava_registro(1);
	fclose(file);

	//Agora começamos a parte da pesquisa
	
	file = fopen(file_name, "r");

	{
		
		char ra [200];
		while(ra[0] != '0')
		{

			fprintf(stderr, "Qual RA voce quer pesquisar : ");
			fgets(ra, 198, stdin);
			remove_last_char(ra);
			int block_offset=0, inner_offset=0, bytes_lidos, guardiao_achou=0;
			do
			{
				
				inner_offset = 0;
				bytes_lidos = fread(memory_block, 512, 1, file);
				do
				{
					inner_offset = retorna_registro(inner_offset);
					inner_offset = retorna_campo(inner_offset);
					if(strcmp(text_buffer, ra) == 0)
					{
						guardiao_achou=1;
						fprintf(stdout, "%s:", text_buffer);
						inner_offset = retorna_campo(inner_offset);
						fprintf(stdout, "%s:", text_buffer);
						inner_offset = retorna_campo(inner_offset);
						fprintf(stdout, "%s:", text_buffer);
						inner_offset = retorna_campo(inner_offset);
						fprintf(stdout, "%s\n", text_buffer);
						inner_offset = retorna_campo(inner_offset);
					}
				}while(record_buffer[0] != 0 && guardiao_achou == 0);
				if(guardiao_achou==1) 
				{
					rewind(file);
				}
			}while(bytes_lidos == 512 && guardiao_achou == 0);
			if(guardiao_achou == 0)
			{
				fprintf(stdout, "*\n");
			}
		}

	}

	fclose(file);

	return 0;
}

void append(char * dest, char * src)
{
	int zero, i;
	for(zero = 0; dest[zero] != '\0'; zero++);
	for(i = 0; src[i] != '\0'; i++)
	{
		dest[zero+i] = src[i];
	}
	dest[zero+i+1] = '\0';
}

void grava_registro(int force)
{
	//calcula o tamanho do registro
	int len = strlen(record_buffer);

	//Verifica se o registro cabe no bloco;
	if(len > 510 - bytes_ocupados || force == 1)
	{
		//printf("Shit\n");
		//Prepara o bloco para gravação
		int i = 0;
		for(; memory_block[i] != '\0'; i++);
		for(; i < 510; i++)
		{
			memory_block[i] = '\0';
		}
		//Grava a quantide de bytes uteis em little endian
		memory_block[510] = bytes_ocupados & 0xFF;
		memory_block[511] = (bytes_ocupados >> 8) & 0xFF;
		//Grava a pagina no arquivo
		//printf("woof\n");
		fwrite(memory_block , 512 , 1, file);
		//printf("woof\n");
		//Reseta o bloco de memoria
		{
			int j;
			for(j=0; j< 510; j++) memory_block[j]=0;
		};
		bytes_ocupados = 0;
	}
	if(force != 1)
	{
		append(memory_block, record_buffer);
		bytes_ocupados += len;
	}
		
}

void remove_last_char(char * stream)
{
	int i = 0;
	for(; stream[i] != '\0'; i++);
	if(i > 0) stream[i-1] = '\0';
}

int retorna_registro(int offset)
{
	{
		int i;
		for(i = 0; i < 512; i++) record_buffer[i] = 0;
	}
	for(; (memory_block[offset] != '#' && offset < 512) && offset !=0; offset++){fprintf(stderr,".");}
	int counter;
	if(offset != 512)
	{
		for(counter = 0; memory_block[offset+counter+1] != '#' &&						memory_block[offset+counter+1] != '\0' &&
					offset + counter < 510; counter++)
		{
			record_buffer[counter] = memory_block[offset+counter+1]; 
		}
	}
	return counter;
}

int retorna_campo(int offset)
{
	{
		int i;
		for(i = 0; i < 200; i++) text_buffer[i] = 0;
	}
	int counter;
	for(; ((record_buffer[offset] != '|' || record_buffer[offset] != '#') && offset < 512) && offset != 0; offset++);
	if(offset != 512)
	{
		for(counter = 0; record_buffer[offset+counter+1] != '|'; counter++)
		{
			fprintf(stderr,"%c",record_buffer[offset+counter+1]);
			text_buffer[counter] = record_buffer[offset+counter+1]; 
		}
	}
	return offset+counter+1;
}
