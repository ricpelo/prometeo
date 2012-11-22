#include <mem.h>
#include <conio.h>
#include <stdio.h>
#include <dos.h>

int main(void)
{
	int salida = 0;
	char far *p;

	p = (char *) getvect(0xF5);
	p += 0x145C;

	if (*p   == '<')
	if (*(p+1) == ':')
	if (*(p+2) == 'G')
	if (*(p+3) == 'A')
	if (*(p+4) == 'C')
	if (*(p+5) == ':')
	if (*(p+6) == '>')
		salida = 1;

	if (salida)
		printf("El programa est  residente en memoria\n");

	return 0;
}
