#include "Alfa.H"
#include <ctype.h>

int EsAlfa(int c)
{
	return isalpha(c) ||
		   c == 164 ||	/* ¤ */
		   c == 165 ||	/* ¥ */
		   c == 160 ||	/*   */
		   c == 130 ||	/* ‚ */
		   c == 161 ||	/* ¡ */
		   c == 162 ||	/* ¢ */
		   c == 163;  	/* £ */
}


int EsAlfanum(int c)
{
	return EsAlfa(c) || isalnum(c);
}


int Mayus(int c)
{
	return (c == '¤') ? '¥' : toupper(c);
}