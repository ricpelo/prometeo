#include <process.h>
#include <conio.h>
#include <alloc.h>
#include "Memoria.H"

void *Memoria(unsigned long bytes)
{
	void *ptr;

	if ((ptr = farmalloc(bytes)) == NULL) {
		cprintf("ERROR: Memoria insuficiente\r\n");
		exit(1);
	}

	return ptr;
}
