#include "Fundido.H"

void Fundido(void)
{
	while (Humo())
		;
}


int Humo(void)
{
	register char far *p;
	char far *mem_video = (char far *) 0xb8000000;
	int bandera = 0;

	for (p = mem_video; p < mem_video + 80 * 25 * 2; p++, p++)
		if ((*p != 0) && (*p != 32)) {
			(*p)++;
			bandera = 1;
		}

	return bandera;
}
