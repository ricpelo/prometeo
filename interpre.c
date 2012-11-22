
# line 2 "interpre.y"
#include "Sistema.H"
#include "Bandera.H"
#include "Local.H"
#include "Acciones.H"
#include "T_Resp.H"
#include "Runtime.H"
#include "Input.H"
#include "Interpre.H"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <alloc.h>
#include "Memoria.H"
#include "Vocab.H"
int yylex(void);
int yyerror(char *s);
void EjecutaSentencia(void);
int BuscaRespuesta(int verb, int nomb);
void BanderasObjeto(void);
#define VERBO 257
#define NOMBRE 258
#define ADJETIVO 259
#define ADVERBIO 260
#define PREPOSICION 261
#define CONJUNCION 262
#define PRONOMBRE 263
#define NOMBRE_VERBO 264
#define DIRECCION 265
#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval, yyval;
#define YYERRCODE 256

# line 121 "interpre.y"


int yyerror(char *s)
{
	if (s != NULL && !tiempo) {	/* Si ha habido un tiempo muerto, nada */
		Emite(sistema[6]); 				/* "No te entiendo" */
		syntaxError = 1;
		return 0;
	}

	return 1;
}

void EjecutaSentencia(void)
{
	CONEXION *co;
	int verb, nomb;
	int oscuro = bandera[0] != 0;
	int luzAusente = DarActualObjeto(DarObjeto(0)) != bandera[38];
	int sent = 0;

	bandera[31]++;				/* Incrementamos el n§ de turnos */

	if (bandera[5] > 0)
		bandera[5]--;
	if (bandera[6] > 0)
		bandera[6]--;
	if (bandera[7] > 0)
		bandera[7]--;
	if (bandera[8] > 0)
		bandera[8]--;

	if (oscuro) {
		if (bandera[9] > 0)
			bandera[9]--;
		if (bandera[10] > 0 && luzAusente)
			bandera[10]--;
	}

	verb = bandera[33];
	nomb = bandera[34];
	BanderasObjeto();
	co = ConexionLocalidad(bandera[38]);	/* 38 = Localidad actual */

	/* Respuestas de mayor a menor prioridad */
	sent = BuscaRespuesta(-1, -1) || sent; 	 	/* Respuestas "* *" */
	if (nomb >= 0)
		sent = BuscaRespuesta(-1, nomb) || sent;  /* Respuestas "* NOMBRE" */
	if (verb >= 0)
		sent = BuscaRespuesta(verb, -1) || sent; 	/* Respuestas "VERBO *" */
	if (verb >= 0 && nomb >= 0)
		sent = BuscaRespuesta(verb, nomb) || sent; /* Respuestas "VERBO NOMBRE" */
	if (verb >= 0)
		sent = BuscaRespuesta(verb, -2) || sent;  	/* Respuestas "VERBO _" */
	if (nomb >= 0)
		sent = BuscaRespuesta(-2, nomb) || sent;   /* Respuestas "_ NOMBRE" */
	sent = BuscaRespuesta(-2, -2) || sent;	   		/* Respuestas "_ _" */

	if (hecho != 1) {
		/* Comprobar si se ha introducido una acci¢n de movimiento */
		while (co != NULL && co->direc != verb)
			co = co->siguiente;

		if (co != NULL) {  				/* La conexion existe */
			if (!ExisteLocalidad(co->destino))
				EmiteError("ERROR: La localidad no existe");
			else {
				bandera[38] = co->destino;	/* Mueve el jugador al destino */
				DescribeSituacion();		/* Describe nueva localidad */
			}
		} else {
			if (!sent) {
				if (DarTipoPalabra(PrimeraVocab(verb)) == T_DIRECCION)
					Emite(sistema[21]);		/* "No puedo ir en esa direcci¢n" */
				else
					Emite(sistema[8]);	/* "No puedes hacer eso" */
			}
		}
	}

}


/*
	BuscaRespuesta():	Ejecuta las acciones correspondientes para una
						cierta respuesta
	Recibe:				El verbo y el nombre de la respuesta a buscar
	Devuelve:			1 si se ha encontrado una respuesta v lida; 0 en otro
						caso
*/
int BuscaRespuesta(int verb, int nomb)
{
	int salida = 0;
	RESPUESTA *resp;

	/* La respuesta s¢lo da lugar a acciones cuando el interruptor "hecho"
	   est  apagado */
	if ((resp = DarRespuesta(verb, nomb)) != NULL && !hecho) {
		salida = 1;
		EmpujaPila(&direcciones, pc);
		pc = resp->direccion;
		Traductor();
	}

	return salida;
}


/*
	BanderasObjeto():	Actualiza las banderas relacionadas con la
						informaci¢n del objeto actualmente referido
*/
void BanderasObjeto(void)
{
	int o = CodigoObjeto(bandera[34], bandera[35]);
	OBJETO *obj = DarObjeto(o);
	int tipoObj;
	int valor, nombre, adjetivo;

	if (obj == NULL) {
		valor = Siguiente(ENCIMA_LLEVADO, 0);
		DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
		while (valor != -1 && nombre != bandera[34]) {
			valor = Siguiente(ENCIMA_LLEVADO, 1);
			DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
		}

		if (valor == -1) {
			valor = Siguiente(bandera[38], 0);
			DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
			while (valor != -1 && nombre != bandera[34]) {
				valor = Siguiente(bandera[38], 1);
				DarNombreObjeto(DarObjeto(valor), &nombre, &adjetivo);
			}

			if (valor != -1)
				obj = DarObjeto(valor);

		} else
			obj = DarObjeto(valor);
	}

	tipoObj = DarTipoObjeto(obj);
	bandera[54] = DarActualObjeto(obj);	/* Localidad actual */
	bandera[55] = DarPesoObjeto(obj);	/* Peso del objeto */
	bandera[56] = tipoObj == T_CONTENEDOR;
	bandera[57] = tipoObj == T_ROPA;
	bandera[58] = DarCodigoObjeto(obj);
}


int Siguiente(int loc, int indic)
{
	static LOBJETO *sgteObjeto = NULL;
	static LOBJETO *inicial = NULL;
	LOBJETO *p, *q;

	if (indic == 0) {
		p = inicial;

		while (p != NULL) {
			q = p;
			p = p->siguiente;
			farfree(q);
		}

		sgteObjeto = inicial = SiguienteAux(localObj[loc]);

	} else if (sgteObjeto != NULL)
		sgteObjeto = sgteObjeto->siguiente;

	return (sgteObjeto == NULL) ? -1 : sgteObjeto->codigo;
}


LOBJETO *SiguienteAux(LOBJETO *org)
{
	LOBJETO *dest;

	if (org == NULL)
		return NULL;
	else {
		dest = (LOBJETO*) Memoria((long) sizeof (LOBJETO));
		dest->codigo = org->codigo;
		dest->siguiente = SiguienteAux(org->siguiente);
		return dest;
	}
}
FILE *yytfilep;
char *yytfilen;
int yytflag = 0;
int svdprd[2];
char svdnams[2][2];

int yyexca[] = {
  -1, 1,
  0, -1,
  -2, 0,
  0,
};

#define YYNPROD 59
#define YYLAST 233

int yyact[] = {
       6,       9,      25,      84,      35,      31,      10,       7,
       8,      36,      37,      38,      32,      33,      14,      52,
      13,      58,      59,      60,      53,      81,      82,      83,
      56,      78,      29,      16,      51,      15,      55,      24,
      43,      70,      44,       4,      34,      28,      77,      68,
      48,      47,      45,      88,      72,      80,      71,      22,
      57,      21,      26,      27,      20,      19,      18,      17,
      23,       5,      11,      30,      41,       3,      42,      39,
      40,       2,       1,       0,      46,       0,       0,      54,
      49,      50,       0,      62,       0,       0,       0,       0,
      65,      66,       0,      61,      67,      63,      64,      69,
       0,       0,       0,       0,       0,      73,       0,      74,
      75,       0,      76,       0,      79,       0,      85,      86,
      87,       0,       0,       0,       0,       0,       0,       0,
      89,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
      12,
};

int yypact[] = {
   -1000,   -1000,    -257,     -30,   -1000,    -230,   -1000,   -1000,
   -1000,   -1000,   -1000,    -257,   -1000,   -1000,   -1000,   -1000,
   -1000,    -259,    -259,    -259,    -233,    -233,   -1000,    -252,
    -254,   -1000,    -254,    -254,    -259,   -1000,    -259,   -1000,
   -1000,   -1000,       0,   -1000,    -259,   -1000,   -1000,       0,
       0,    -243,    -243,    -236,    -240,    -233,    -243,    -233,
    -233,    -236,    -236,       0,   -1000,   -1000,       0,   -1000,
   -1000,      -1,   -1000,   -1000,   -1000,    -259,   -1000,    -259,
    -259,   -1000,   -1000,    -236,    -234,    -236,   -1000,    -237,
    -256,    -243,    -243,    -243,   -1000,   -1000,   -1000,   -1000,
   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,
    -256,   -1000,
};

int yypgo[] = {
       0,      66,      65,      61,      58,      35,      57,      56,
      55,      31,      36,      32,      30,      54,      53,      52,
      37,      28,      49,      48,      46,      45,      44,      23,
      43,      42,      41,      40,      39,      38,
};

int yyr1[] = {
       0,       2,       1,       3,       3,       4,       4,       4,
       4,       5,       5,       7,       7,       7,       7,       8,
       6,      13,       6,      14,       6,      15,       6,      18,
       6,      11,      11,      20,      19,      22,      19,      19,
      19,      24,      21,      21,      21,      23,      23,       9,
       9,      25,      10,      10,      10,      26,      10,      27,
      10,      16,      16,      28,      17,      17,      17,      29,
      29,      12,      12,
};

int yyr2[] = {
       0,       0,       2,       3,       1,       1,       1,       1,
       1,       1,       3,       2,       2,       2,       0,       0,
       6,       0,       6,       0,       6,       0,       7,       0,
       7,       3,       0,       0,       3,       0,       3,       1,
       0,       0,       3,       1,       0,       1,       0,       1,
       0,       0,       5,       3,       0,       0,       5,       0,
       5,       1,       0,       0,       3,       1,       0,       1,
       0,       1,       0,
};

int yychk[] = {
   -1000,      -1,      -2,      -3,      -5,      -6,     257,     264,
     265,     258,     263,      -4,     262,      46,      44,      59,
     257,      -8,     -13,     -14,     -15,     -18,      -5,      -7,
      -9,     261,      -9,      -9,     -16,     259,     -16,     257,
     264,     265,     -10,     258,     263,     264,     265,     -10,
     -10,      -9,      -9,     -11,      34,     -25,      -9,     -26,
     -27,     -11,     -11,     -17,     258,     263,     -17,     -12,
     260,     -19,     257,     258,     259,     -16,     -17,     -16,
     -16,     -12,     -12,     -11,     -28,     -11,      34,     -20,
     -22,      -9,      -9,      -9,     -12,     -29,     259,     -12,
     -21,     258,     259,     -23,     259,     -17,     -17,     -17,
     -24,     -23,
};

int yydef[] = {
       1,      -2,       0,       2,       4,       9,      15,      17,
      19,      21,      23,       0,       5,       6,       7,       8,
      14,      40,      40,      40,      50,      50,       3,      10,
      44,      39,      44,      44,      40,      49,      40,      11,
      12,      13,      26,      41,      40,      45,      47,      26,
      26,      54,      54,      58,      32,      50,      54,      50,
      50,      58,      58,      26,      51,      53,      26,      16,
      57,       0,      27,      29,      31,      40,      43,      40,
      40,      18,      20,      58,      56,      58,      25,      36,
      38,      54,      54,      54,      22,      52,      55,      24,
      28,      33,      35,      30,      37,      42,      46,      48,
      38,      34,
};

int *yyxi;
/*****************************************************************/
/* PCYACC LALR parser driver routine -- a table driven procedure */
/* for recognizing sentences of a language defined by the        */
/* grammar that PCYACC analyzes. An LALR parsing table is then   */
/* constructed for the grammar and the skeletal parser uses the  */
/* table when performing syntactical analysis on input source    */
/* programs. The actions associated with grammar rules are       */
/* inserted into a switch statement for execution.               */
/*****************************************************************/


#ifndef YYMAXDEPTH
#define YYMAXDEPTH 200
#endif
#ifndef YYREDMAX
#define YYREDMAX 1000
#endif
#define PCYYFLAG -1000
#define WAS0ERR 0
#define WAS1ERR 1
#define WAS2ERR 2
#define WAS3ERR 3
#define yyclearin pcyytoken = -1
#define yyerrok   pcyyerrfl = 0
YYSTYPE yyv[YYMAXDEPTH];     /* value stack */
int pcyyerrct = 0;           /* error count */
int pcyyerrfl = 0;           /* error flag */
int redseq[YYREDMAX];
int redcnt = 0;
int pcyytoken = -1;          /* input token */


yyparse()
{
  int statestack[YYMAXDEPTH]; /* state stack */
  int      j, m;              /* working index */
  YYSTYPE *yypvt;
  int      tmpstate, tmptoken, *yyps, n;
  YYSTYPE *yypv;


  tmpstate = 0;
  pcyytoken = -1;
#ifdef YYDEBUG
  tmptoken = -1;
#endif
  pcyyerrct = 0;
  pcyyerrfl = 0;
  yyps = &statestack[-1];
  yypv = &yyv[-1];


  enstack:    /* push stack */
#ifdef YYDEBUG
    printf("at state %d, next token %d\n", tmpstate, tmptoken);
#endif
    if (++yyps - &statestack[YYMAXDEPTH] > 0) {
      yyerror("pcyacc internal stack overflow");
      return(1);
    }
    *yyps = tmpstate;
    ++yypv;
    *yypv = yyval;


  newstate:
    n = yypact[tmpstate];
    if (n <= PCYYFLAG) goto defaultact; /*  a simple state */


    if (pcyytoken < 0) if ((pcyytoken=yylex()) < 0) pcyytoken = 0;
    if ((n += pcyytoken) < 0 || n >= YYLAST) goto defaultact;


    if (yychk[n=yyact[n]] == pcyytoken) { /* a shift */
#ifdef YYDEBUG
      tmptoken  = pcyytoken;
#endif
      pcyytoken = -1;
      yyval = yylval;
      tmpstate = n;
      if (pcyyerrfl > 0) --pcyyerrfl;
      goto enstack;
    }


  defaultact:


    if ((n=yydef[tmpstate]) == -2) {
      if (pcyytoken < 0) if ((pcyytoken=yylex())<0) pcyytoken = 0;
      for (yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=tmpstate); yyxi += 2);
      while (*(yyxi+=2) >= 0) if (*yyxi == pcyytoken) break;
      if ((n=yyxi[1]) < 0) { /* an accept action */
        if (yytflag) {
          int ti; int tj;
          yytfilep = fopen(yytfilen, "w");
          if (yytfilep == NULL) {
            fprintf(stderr, "Can't open t file: %s\n", yytfilen);
            return(0);          }
          for (ti=redcnt-1; ti>=0; ti--) {
            tj = svdprd[redseq[ti]];
            while (strcmp(svdnams[tj], "$EOP"))
              fprintf(yytfilep, "%s ", svdnams[tj++]);
            fprintf(yytfilep, "\n");
          }
          fclose(yytfilep);
        }
        return (0);
      }
    }


    if (n == 0) {        /* error situation */
      switch (pcyyerrfl) {
        case WAS0ERR:          /* an error just occurred */
          yyerror("syntax error");
          yyerrlab:
            ++pcyyerrct;
        case WAS1ERR:
        case WAS2ERR:           /* try again */
          pcyyerrfl = 3;
	   /* find a state for a legal shift action */
          while (yyps >= statestack) {
	     n = yypact[*yyps] + YYERRCODE;
	     if (n >= 0 && n < YYLAST && yychk[yyact[n]] == YYERRCODE) {
	       tmpstate = yyact[n];  /* simulate a shift of "error" */
	       goto enstack;
            }
	     n = yypact[*yyps];


	     /* the current yyps has no shift on "error", pop stack */
#ifdef YYDEBUG
            printf("error: pop state %d, recover state %d\n", *yyps, yyps[-1]);
#endif
	     --yyps;
	     --yypv;
	   }


	   yyabort:
            if (yytflag) {
              int ti; int tj;
              yytfilep = fopen(yytfilen, "w");
              if (yytfilep == NULL) {
                fprintf(stderr, "Can't open t file: %s\n", yytfilen);
                return(1);              }
              for (ti=1; ti<redcnt; ti++) {
                tj = svdprd[redseq[ti]];
                while (strcmp(svdnams[tj], "$EOP"))
                  fprintf(yytfilep, "%s ", svdnams[tj++]);
                fprintf(yytfilep, "\n");
              }
              fclose(yytfilep);
            }
	     return(1);


	 case WAS3ERR:  /* clobber input char */
#ifdef YYDEBUG
          printf("error: discard token %d\n", pcyytoken);
#endif
          if (pcyytoken == 0) goto yyabort; /* quit */
	   pcyytoken = -1;
	   goto newstate;      } /* switch */
    } /* if */


    /* reduction, given a production n */
#ifdef YYDEBUG
    printf("reduce with rule %d\n", n);
#endif
    if (yytflag && redcnt<YYREDMAX) redseq[redcnt++] = n;
    yyps -= yyr2[n];
    yypvt = yypv;
    yypv -= yyr2[n];
    yyval = yypv[1];
    m = n;
    /* find next state from goto table */
    n = yyr1[n];
    j = yypgo[n] + *yyps + 1;
    if (j>=YYLAST || yychk[ tmpstate = yyact[j] ] != -n) tmpstate = yyact[yypgo[n]];
    switch (m) { /* actions associated with grammar rules */
      
      case 1:
# line 28 "interpre.y"
      { syntaxError = terminal = hecho = 0; bandera[33] = bandera[34] =
      			bandera[35] = bandera[36] = bandera[43] = bandera[44] =
      			bandera[45] = bandera[48] = bandera[49] = bandera[50] = -1; } break;
      case 9:
# line 44 "interpre.y"
      { if (!syntaxError) EjecutaSentencia(); } break;
      case 10:
# line 45 "interpre.y"
      { syntaxError = 1; yyerror("syntax error"); } break;
      case 15:
# line 54 "interpre.y"
      { hecho = 0; bandera[33] = yypvt[-0]; } break;
      case 17:
# line 56 "interpre.y"
      { hecho = 0; bandera[33] = yypvt[-0]; } break;
      case 19:
# line 58 "interpre.y"
      { hecho = 0; bandera[33] = yypvt[-0]; } break;
      case 21:
# line 61 "interpre.y"
      { if (bandera[33] >= 0) {
      			  bandera[34] = yypvt[-0];
      			  hecho = 0;
      		  }
      		} break;
      case 23:
# line 67 "interpre.y"
      { if (bandera[33] >= 0) hecho = 0; } break;
      case 27:
# line 75 "interpre.y"
      { bandera[48] = yypvt[-0]; } break;
      case 29:
# line 76 "interpre.y"
      { bandera[49] = yypvt[-0]; } break;
      case 31:
# line 77 "interpre.y"
      { bandera[50] = yypvt[-0]; } break;
      case 33:
# line 81 "interpre.y"
      { bandera[49] = yypvt[-0]; } break;
      case 35:
# line 82 "interpre.y"
      { bandera[50] = yypvt[-0]; } break;
      case 37:
# line 86 "interpre.y"
      { bandera[50] = yypvt[-0]; } break;
      case 39:
# line 90 "interpre.y"
      { bandera[43] = yypvt[-0]; } break;
      case 41:
# line 94 "interpre.y"
      { bandera[34] = yypvt[-0]; } break;
      case 44:
# line 97 "interpre.y"
      { if (!terminal) bandera[34] = -1; } break;
      case 45:
# line 98 "interpre.y"
      { bandera[34] = yypvt[-0]; } break;
      case 47:
# line 100 "interpre.y"
      { bandera[34] = yypvt[-0]; } break;
      case 49:
# line 104 "interpre.y"
      { bandera[35] = yypvt[-0]; } break;
      case 51:
# line 108 "interpre.y"
      { bandera[44] = yypvt[-0]; } break;
      case 55:
# line 113 "interpre.y"
      { bandera[45] = yypvt[-0]; } break;
      case 57:
# line 117 "interpre.y"
      { bandera[36] = yypvt[-0]; } break;    }
    goto enstack;
}
