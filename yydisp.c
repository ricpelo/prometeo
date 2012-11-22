#include "yydisp.h"
#include "common.h"
#include "palabra.h"   // Hay que incluir estos tres ficheros de cabecera
#include "objeto.h"    // para que al incluir el fichero "yytab.h" no se
#include "local.h"     // produzcan errores.
#include "yytab.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define YYERRCODE 256

char *yydisplay(int tok, int sw) {
	const char *toknames[] = {
	#include "yytab.tok"
	NULL };
	const char *result;
	static char buf[BUFSIZ];

	if (tok == 0)
		result = "EOF";
	else if (tok == YYERRCODE)
		result = "error";
	else if (isascii(tok) && isprint(tok))
		sprintf(buf, "%c", tok), result = buf;
	else if (tok > 0 && tok < YYERRCODE)
		sprintf(buf, "\'x%X\'", tok), result = buf;
	else if (tok > YYERRCODE && tok < YYERRCODE + DIM(toknames)) {
		if (tok == ID && sw)
			sprintf(buf, "%s \"%s\"", toknames[tok - (YYERRCODE + 1)],
					yylval.lex), result = buf;
		else
			result = toknames[tok - (YYERRCODE + 1)];
	} else
		result = "-desconocido-";

	return DupString(result);
}
