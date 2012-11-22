%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tabla.H"
extern FILE *yyin;
extern FILE *yyout;
int yylex(void);
int loc = 0;
%}

%union {
	char *cadena;
}

%token DEFINE LOCAL FINLOCAL
%token <cadena> CADENA

%%

programa : programa linea
		 | linea
		 ;

linea : DEFINE CADENA CADENA { InsertarElemento($2, $3); }
	  | CADENA { char *i = DarInfo($1);
				 if (i == NULL) fprintf(yyout, "%s", $1);
				 else fprintf(yyout, "%s", i);
			   }
	  | LOCAL resto FINLOCAL
	  ;

resto : resto CADENA { char cad[2048];
					   InsertarElemento($2, itoa(loc++, cad, 10)); }
	  | CADENA { char cad[2048];
				 InsertarElemento($1, itoa(loc++, cad, 10)); }
	  ;

%%

void yyerror(char *s)
{
	printf("ERROR: %s\n", s);
	exit(1);
}

