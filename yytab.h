
typedef union  {
	int valor;
	unsigned long linea;
	char *lex;
	TIPO_PALABRA tipoPal;
	TIPO_OBJETO tipoObj;
	CONEXION *conex;
} YYSTYPE;
extern YYSTYPE yylval;
#define MENSAJES 257
#define NUM 258
#define ID 259
#define CADENA 260
#define VOCABULARIO 261
#define VERBO 262
#define ADVERBIO 263
#define NOMBRE 264
#define ADJETIVO 265
#define PREPOSICION 266
#define CONJUNCION 267
#define PRONOMBRE 268
#define OBJETOS 269
#define DESC 270
#define CONTENEDOR 271
#define ROPA 272
#define VERBO1 273
#define ADVERBIO1 274
#define NOMBRE1 275
#define ADJETIVO1 276
#define VERB1 277
#define NOMB1 278
#define PREPOSICION1 279
#define CONJUNCION1 280
#define PRONOMBRE1 281
#define PESO 282
#define LOCALIDADES 283
#define INICIAL 284
#define IMAGEN 285
#define IMAGEN_CICLO 286
#define BANDERA 287
#define ASIGNA 288
#define MAY 289
#define MEN 290
#define MAI 291
#define MEI 292
#define EQU 293
#define NEQ 294
#define LLAMA 295
#define SUBRUTINA 296
#define RETORNO 297
#define MIENTRAS 298
#define NOT 299
#define AND 300
#define OR 301
#define TRUE 302
#define FALSE 303
#define SI 304
#define SINO 305
#define ENTONCES 306
#define SISTEMA 307
#define ESTA 308
#define ESTAMAY 309
#define ESTAMEN 310
#define PRESENTE 311
#define PUESTO1 312
#define ENCIMA1 313
#define LLEVADO1 314
#define ESTAEN 315
#define ADJET1 316
#define ADVERB 317
#define PREPOS 318
#define NOMB2 319
#define ADJET2 320
#define ACABAR1 321
#define COGER 322
#define DEJAR 323
#define PONER 324
#define QUITAR 325
#define CREAR 326
#define DESTRUIR 327
#define ACABAR 328
#define TURNOS 329
#define HECHO 330
#define COLOCAR 331
#define INTERCAMBIAR 332
#define METER 333
#define SACAR 334
#define AUTOC 335
#define AUTOD 336
#define AUTOP 337
#define AUTOQ 338
#define AUTOM 339
#define AUTOS 340
#define LISTAOBJ 341
#define LISTADE 342
#define INVEN 343
#define MENSIS 344
#define LLEVADO2 345
#define NO_CREADO1 346
#define TECLA 347
#define PAUSA 348
#define AUSENTE 349
#define DESCRIBE 350
#define MUEVE 351
#define LOCAL 352
#define NO_HECHO 353
#define DIAG 354
#define IMPRIME 355
#define ANALIZA 356
#define OBJETO 357
#define SALVAR 358
#define CARGAR 359
#define BORRAR 360
#define ALEATORIO 361
#define SALIDAS 362
#define PRIMERO 363
#define SIGUIENTE 364
#define CONTENEDOR1 365
#define ROPA1 366
#define CURSOR 367
#define POSX 368
#define POSY 369
#define SALTOLINEA 370
#define SONIDO 371
#define EVALUAR 372
#define CASO 373
#define DEFECTO 374
#define SILENCIO 375
#define SONIDO_CICLO 376
#define CAMINAR 377
#define VENTANA 378
#define COLOR 379
#define FONDO 380
#define LISTACONT 381
#define NOMBRE_VERBO 382
#define NOMBRE_VERBO1 383
#define DIRECCION 384
#define DIRECCION1 385
#define DIRECC1 386
#define FUENTE 387
#define MENOSU 388
