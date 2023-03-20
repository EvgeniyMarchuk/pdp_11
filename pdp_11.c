#include <stdio.h>
#include <assert.h>
#include <string.h>

#define MEMSIZE (64*1024)

typedef unsigned char byte;
typedef unsigned short int word;
typedef word address;

word mem[MEMSIZE];

void b_write (address adr, byte val);
byte b_read (address adr);

void w_write (address adr, word val);
word w_read (address adr);

void load_data();
void mem_dump(address adr, unsigned int size);

void load_file(char *name_file);

void test_mem();

int main(int argc, char *argv[])
{	
	if (argc == 1)
	{
		printf("Data will read from console\n");
		load_data();
	}
	else if (strcmp(argv[1],"-t") == 0)
		load_file(argv[argc-1]);
	else
		printf("Incorrect program launch");
	
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
	{	
		mem[adr-1] |= (((word)val) << 8);
		//printf("\n%0x \n", mem[adr-1]);
	}
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

void load_data()
{
	address adr_in_mem = 0;
	unsigned char N = 0;
	
	while (fscanf(stdin, "%hx %hhx", &adr_in_mem, &N) == 2)  
	{
		for (unsigned char i = 0; i < N; i++)
		{
			byte num;
			scanf("%hhx", &num);
			
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
		
		printf("%06o: %06o %04x\n", adr, res, res);
		adr += 2;
	}
}

void load_file(char *name_file)
{	
	FILE *data_file = fopen(name_file, "r");
	
	if (data_file == NULL)
		perror("Ошибка открытия файла");
	
	address adr_in_mem = 0;
	unsigned char N = 0;
	
	while (fscanf(data_file, "%hx %hhx", &adr_in_mem, &N) == 2)  
	{
		for (unsigned char i = 0; i < N; i++)
		{
			byte num;
			fscanf(data_file, "%hhx", &num);
			
			b_write(adr_in_mem, num);
			
			adr_in_mem++;
		}
	}
	
	fclose(data_file);
}

void test_mem()
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
}

/*void test_load(char *name_file)
{
	;
}*/
