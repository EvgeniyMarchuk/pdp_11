#include <stdio.h>
#include <assert.h>

#define MEMSIZE (64*1024)

typedef unsigned char byte;
typedef unsigned short int word;
typedef word address;

word mem[MEMSIZE];

void b_write (address adr, byte val);
byte b_read (address adr);

void w_write (address adr, word val);
word w_read (address adr);

void test_mem();

int main()
{	
	test_mem();
	
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
	
	
	
    // пишем 2 байта, читаем 1 слово
    fprintf(stderr, "Пишем 2 байта, читаем слово\n");
    a = 3;        // другой адрес
    w = 0xa1b2;
    // little-endian, младшие разряды по меньшему адресу
    b0 = 0xb2;
    b1 = 0xa1;
    b_write(a, b0);
    b_write(a+1, b1);
    wres = w_read(a);
    // тут полезно написать отладочную печать a, w, wres
    fprintf(stderr, "a=%06o b1=%02hhx b0=%02hhx wres=%04x \n", a, b1, b0, wres);
    assert(w == wres);
	
	
    // еще тесты
}
