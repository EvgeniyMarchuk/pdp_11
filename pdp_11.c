#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

#define MEMSIZE (64*1024)
#define SIZE 100

typedef unsigned char byte;
typedef unsigned short int word;
typedef word address;

enum level{
	DEBUG,
	TRACE,
	TESTS,
	INFO,
	ERROR
};

unsigned int log_level = TESTS;

word mem[MEMSIZE];

word reg[8];    


void b_write (address adr, byte val);
byte b_read (address adr);

void w_write (address adr, word val);
word w_read (address adr);

void load_data(char *name_file);
void mem_dump(address adr, unsigned int size);

//void load_file(char *name_file);

void Log(unsigned int level, char *str, ...);
unsigned int set_log_level(unsigned int level);

int is_hexnumber(unsigned char num);

int test_mem();
int test_file();





int main(int argc, char *argv[])
{	
	if (argc == 1)
	{
		Log(INFO, "\nData will read from console, if you want read from file, please, write -t and name of file after ./a.out\n");
		load_data("console");
	}
	else if (strcmp(argv[1],"-t") == 0)
		load_data(argv[argc-1]);
	else
		Log(INFO, "\nIncorrect program launch\n");
	
	if (TESTS >= log_level)
		Log(TESTS, "Test!", test_mem());
	
    return 0;
}





byte b_read(address adr)
{
	byte b_res;
	
	if ((adr & 1) == 0)
		b_res = (byte)(mem[adr]);
	else
		b_res = (byte)(mem[adr-1] >> 8);
	
	return b_res;
}

void b_write(address adr, byte val)
{
	if ((adr & 1) == 0)
		mem[adr] = (word)val;
	else
		mem[adr-1] |= (((word)val) << 8);
}

void w_write(address adr, word val)
{
	assert((adr & 1) == 0);
	mem[adr] = val;	
}

word w_read(address adr)
{
	assert((adr & 1) == 0);
	return mem[adr];
}





void load_data(char *name_file)
{
	address adr_in_mem = 0;
	unsigned char N = 0;
	
	FILE *data_file = NULL;
	
	if (strcmp(name_file, "console") == 0)
		data_file = stdin;
	else
	{
		data_file = fopen(name_file, "r");
		
		if (data_file == NULL)
		{
			perror("\nError, can't open the file\n");
			exit(1);
		}
	}
	
	while (fscanf(data_file, "%hx %hhx", &adr_in_mem, &N) == 2)  
	{
		for (unsigned char i = 0; i < N; i++)
		{
			byte num;
			
			if (1 != fscanf(data_file, "%hhx", &num))
			{
				Log(ERROR, "Error type reading\n");
				exit(3);
			}	
			
			b_write(adr_in_mem, num);
			
			adr_in_mem++;
		}
	}
}

void mem_dump(address adr, unsigned int size)
{
	for (unsigned int i = 0; i < size/2; i++)
	{
		word res = w_read(adr);
		
		Log(INFO, "%06o: %06o %04x\n", adr, res, res);
		adr += 2;
	}
}






void Log(unsigned int level, char *str, ...)
{
	if (level >= log_level)
	{
		va_list ap;
		va_start(ap, str);
		
		vprintf(str, ap);	
		
		va_end(ap);
	}
}

unsigned int set_log_level(unsigned int level)
{
	unsigned int old_log = log_level;
	
	log_level = level;
	
	return old_log;
}




int is_hexnumber(unsigned char num)               //НАДО СДЕЛАТЬ НОРМАЛЬНОЙ
{
	FILE *p_file = fopen("is_hex.txt", "w");
	
	fprintf(p_file, "%hhx", num);
	
	fclose(p_file);
	
	return 1;
}





int test_mem()
{
    address a;
    byte b0, b1, bres;
    word w, wres;

	
    // пишем байт, читаем байт
    fprintf(stderr, "\n Пишем и читаем байт по четному адресу\n");
    a = 0;
    b0 = 0x12;
    b_write(a, b0);
    bres = b_read(a);
	
	//отладочная печать
	printf("a = %hhx  |||  b0 = %hhx  |||  bres = %hhx \n", a, b0, bres);
	
    fprintf(stderr, "a=%06o b0=%hhx bres=%hhx\n", a, b0, bres);
    assert(b0 == bres);
    // аналогично стоит проверить чтение и запись по нечетному адресу

	
    // пишем слово, читаем слово
    fprintf(stderr, "Пишем и читаем слово\n");
    a = 2;        // другой адрес
    w = 0x3456;
    w_write(a, w);
    wres = w_read(a);
    //отладочная печать
	printf("a = %x  |||  w = %x  |||  wres = %x \n", a, w, wres);
	
    fprintf(stderr, "a=%06o w=%04x wres=%04x\n", a, w, wres);
    assert(w == wres);
	
	
	// работа с нечетным адресом 
    // пишем 2 байта, читаем 1 слово
    fprintf(stderr, "Пишем 2 байта, читаем слово\n");
    a = 3;        // другой адрес
    w = 0xa1b2;
    // little-endian, младшие разряды по меньшему адресу
    b0 = 0xb2;
    b1 = 0xa1;
    b_write(a-1, b0);
	//wres = w_read(a-1);
	//printf("\n%d ||||||||", wres);
    b_write(a, b1);
    wres = w_read(a-1);
    // тут полезно написать отладочную печать a, w, wres
    fprintf(stderr, "a=%06o b1=%02hhx b0=%02hhx wres=%04x \n", a, b1, b0, wres);
    assert(w == wres);
	
	
	//пишем слово, читаем 2 байта
	a = 4;
	w = 0x1234;
	
	byte bres0 = 0x34;
	byte bres1 = 0x12;
	
	w_write(a, w);
	b0 = b_read(a);
	b1 = b_read(a+1);
	Log(TESTS, "пишем слово, чиатем 2 байта\n");
	Log(TESTS, "a=%06o b1=%02hhx b0=%02hhx wres=%04x \n", a, b1, b0, w);
	assert(b0 == bres0);
	assert(b1 == bres1);
	
	return 0;
}

int test_file()
{
	load_data("TryBreakPDP.txt");
	mem_dump(64, 4);
	
	return 0;
}
