vocabulario {
	direccion NORTE = { N };
	direccion SUR = { S };
	direccion ESTE = { E };
	direccion OESTE = { O };
	direccion ARRIBA = { SUBIR };
	direccion ABAJO = { BAJAR };
	direccion NORESTE = { NE };
	direccion SURESTE = { SE };
	direccion ENTRA = { ENTRAR };
	direccion SAL = { SALIR };
	verbo IR;
	verbo INVENTARIO = { INVEN, I };
	verbo COGE = { COGER };
	verbo DEJA = { DEJAR, SOLTAR };
	verbo FIN = { ACABA, ACABAR, TERMINAR };
	verbo MIRA = { M, MIRAR };
	verbo EXAMINA = { EX, EXAM };
	verbo PON = { PONTE, PONER };
	verbo QUITA = { QUITATE, QUITAR };
	verbo METE = { METER };
	verbo SACA = { SACAR };
	verbo DIAGNOSTICO = { DIAG };
	verbo DI = { HABLA };
	verbo ESPERA = { AGUARDA };
	verbo ABRIR = { ABRE };
	verbo CERRAR = { CIERRA };
	verbo MUEVE = { MOVER, EMPUJAR, APARTAR };
	verbo SALIDAS = { X };
	verbo SALVAR = { SAVE, SALVA, GRABAR };
	verbo CARGAR = { LOAD, CARGA, RECUPERA };
	nombre PUERTA = { PORTON, PORTAL };
	nombre TABLONES = { VIGA, VIGAS, MADERA };
	nombre TODO;
	nombre PERRO, GATO, MANZANA, PERA, BOLSA;
	pronombre ELLO, ELLA, ELLOS, ELLAS, ESO, ESA;
	conjuncion Y, ENTONCES;
}
				  
				  
localidades {
	0 : "";

	1 :
"Est s en medio de la plaza de la ciudad. Hay un enorme bullicio de gente
yendo de un lado para otro, ocupados en sus asuntos. Ante ti puedes observar
la puerta de entrada a la alcald¡a."
	 { NORTE : 2, SUR : 3, ESTE : 5, OESTE : 4, ENTRA : 10 };

	2 :
"Una estrecha escalera de piedra conduce hasta la cornisa del castillo."
	  imagen "INTRO.FLI"
	 { ARRIBA : 15, SUR : 1 };

	3 :
"Te encuentras rodeado de un amasijo de peque¤as y destartaladas casas,
formando un intrincado laberinto de callejuelas."
	 { NORTE : 1, SUR : 11 };

	4 :
"El mercado, un calvero donde multitud de comerciantes de todos los lugares
venden sus mercanc¡as."
	 { SUR : 6, ESTE : 1, OESTE : 7 };

	5 :
"Est s frente a la taberna de la plaza. Un cartel cuelga por encima de la
puerta. Del interior llegan gritos, risas y m£sica."
	 { SUR : 8, ESTE : 9, OESTE : 1, ENTRA : 16 };

	6 :
"El mercado acaba aqu¡, junto a una de las murallas de la ciudad."
	 { NORTE : 4 };

	7 :
"Aqu¡ est  la entrada a otra de las almenas de la ciudad. Al parecer, la
entrada ha sido bloqueada por grandes vigas de madera."
	 { ESTE : 4 };

	8 :
"Una explanada frente a la muralla sirve de almac‚n para una enorme
catapulta."
	 { NORTE : 5 };

	9 :
"La entrada a la almena del ala Este de la ciudad."
	 { SUBIR : 17, OESTE : 5 };

	10 :
"En el despacho del alcade una gran mesa ocupa el centro de la habitaci¢n.
Hay un gran desorden de papeles, libros y otros objetos de escaso valor."
	 { SALIR : 1 };

	11 :
"El maloliente laberinto de calles que forman las viviendas."
	 { NORTE : 11, OESTE : 11, SUR : 11, ESTE : 12 };

	12 :
"Las calles entre las viviendas forman un maloliente laberinto."
	 { NORTE : 11, OESTE : 13, ESTE : 11, SUR : 12 };

	13 :
"Las viviendas forman un maloliente laberinto de calles."
	 { NORTE : 13, SUR : 13, ENTRAR : 14, OESTE : 11, ESTE : 12 };

	14 :
"En el interior de la casa del alquimista: gran cantidad de extra¤os
artilugios sin ninguna utilidad aparente, el suelo est  repleto de libros
abiertos. Un fuerte olor a orina domina el ambiente en la habitaci¢n."
	 { SALIR : 13 };

	15 :
"Has subido a la almena, y desde aqu¡ puedes observar c¢mo el ej‚rcito
enemigo rodea las murallas de la ciudad."
	 { BAJAR : 2 };

	16 :
"Dentro de la taberna: el ambiente est  cargado, el olor a eructos de cerveza
ocurren por doquier."
	 { SALIR : 5 };

	17 :
"Desde aqu¡ puedes observar las afueras de la ciudad en direcci¢n Este."
	 { BAJAR : 9 };

}


objetos {
	PERRO  "el perro" peso 1 inicial 1;
	GATO  "el gato" peso 1 inicial 1;
	MANZANA  "la manzana" peso 1 inicial 1;
	PERA  "la pera" peso 1 inicial 1;
	BOLSA  "la bolsa" peso 1 inicial 1 contenedor;
}


sistema {
	0 : "Esta muy oscuro para poder ver nada.";
	1 : " Tambi‚n veo ";
	2 : "Di algo coherente.";
	3 : "­Vamos! ¨A qu‚ esperas?";
	4 : "¨Qu‚ quieres que haga?";
	5 : "";
	6 : "No entiendo lo que quieres decirme.";
	8 : "No puedo hacer eso.";
	9 : "Llevas ";
	10 : " \5(encima)\7";
	11 : "nada en absoluto.";
	12 : "¨Estas seguro?";
	13 : "\n¨Quieres jugar otra vez?";
	16 : "\nPulsa una tecla para continuar...\n";
	17 : "Has hecho _ movida(s) v lida(s).";
	18 : "Las salidas posibles son ";
	19 : "Unicamente ";
	20 : "No hay ninguna salida.";
	23 : "No llevo puesto uno de esos.";
	24 : "No puedo. Ya llevo puesto _.";
	25 : "Ya tengo _.";
	26 : "No hay uno de esos por aqu¡.";
	27 : "Ya no puedo llevar m s cosas.";
	28 : "No tengo eso.";
	29 : "Ya llevo puesto _.";
	30 : "S";
	31 : "S";
	33 : "\n\14>>>\7 ";
	36 : "Ahora tengo _.";
	37 : "Ahora llevo puesto _.";
	38 : "Me he quitado _.";
	39 : "He dejado caer _.";
	40 : "No puedo ponerme _.";
	41 : "No me puedo quitar _.";
	42 : "No puedo quitarme _. Mis manos est n llenas.";
	43 : "_ pesa mucho para m¡.";
	44 : "_ est  en ";
	45 : "- ";
	46 : ", ";
	47 : " y ";
	48 : ".";
	49 : "No tengo _.";
	50 : "No llevo puesto _.";
	51 : ".";
	52 : "No hay uno de esos en ";
	53 : "nada de nada";
}

ENTRAR * {
	si (esta?(13) y b60 = 0)
		no_hecho;
	sino si (esta?(1) y b61 = 0) {
		"\2GUARDA\7: El alcalde est  reunido y no quiere que se le moleste.\"";
		hecho;
	}
}


SAVE * {
	salvar;
	hecho;
}


LOAD * {
	cargar;
	hecho;
}


SUBIR * {
	llama(Mueve_Tablones);
	si (esta?(7))
		hecho;
	sino
		mensis(21);
}


SALIDAS * {
	salidas;
	hecho;
}


MOVER TABLONES {
	llama(Mueve_Tablones);
	hecho;
}


ABRIR PUERTA {
	si (esta?(1) y b60 = 0) {
		"\2GUARDA\7: \"Est  prohibido entrar\".";
		hecho;
	}
}


ABRIR _ {
	"No puedes abrir eso.";
	hecho;
}


FIN * {
	si (acabar?) acabar;
	hecho;
}


COGER TODO {
	b28 := primero(b38);
	b27 := objeto(b44, b45);

	si (b28 = -1)
		"No hay nada que coger.";
	sino
		mientras (b28 <> -1) {
			si (b27 <> b28)
				coger(b28);

			b28 := siguiente(b38);

			si (b28 <> -1 y b27 <> b28)
				saltolinea;
		}

	hecho;
}


COGER _ {
	autoc;
	hecho;
}


DEJAR TODO {
	b28 := primero(llevado);

	si (b28 = -1)
		"No hay nada que dejar.";
	sino
		mientras (b28 <> -1) {
			dejar(b28);

			b28 := siguiente(llevado);

			si (b28 <> -1)
				saltolinea;
		}

	hecho;
}


DEJA _ {
	autod;
	hecho;
}


PON _ {
	autop;
	hecho;
}


QUITA * {
	autoq;
	hecho;
}


METE * {
	si (nombre2?(BOLSA)) {
		autom(BOLSA);
		hecho;
	} sino
		no_hecho;
}


SACA * {
	si (nombre2?(BOLSA)) {
		autos(BOLSA);
		hecho;
	} sino
		no_hecho;
}


MIRA _ {
	describe;
	hecho;
}


INVENTARIO * {
	inven;
	hecho;
}


DIAG * {
	diag;
	hecho;
}


DI _ {
	"¨A qui‚n se lo digo?";
	hecho;
}


ESPERA * {
	"El tiempo pasa...";
	hecho;
}


subrutina Process1 {
	si (esta?(0)) {
		tecla;
		b11 := 5;
		b42 := 5;
		b37 := 5;
		mueve(1);
		describe;
        fuente("THIN.FNT");
	} sino {
		si (b0 = 0 o presente?(0)) {
			si (esta?(7) y b62 = 7)
				" Ves las vigas ca¡das en el suelo, impidiendo el paso.";

			listaobj;
			ventana(posy() + 1, 24);
		}
	}

}


subrutina Process2 {
	si (b62 >= 2 y b62 < 7)
		b62 := b62 + 1;

	si (b62 = 6)
		si (esta?(7)) {
		   "Las pesadas vigas caen sobre ti, convirti‚ndote en una pulpa sanguinolienta de carne y huesos.";
		   turnos;
		   acabar;
		}
}


subrutina Mueve_Tablones {
	si (esta?(7)) {
		si (b62 = 0) {
			"Las vigas se tambalean peligrosamente.";
			b62 := 1;
			hecho;
		} sino si (b62 = 1) {
			"Se escuchan algunos crujidos... ­Parece que todo se va a venir abajo!";
			b62 := 2;
			hecho;
		}
	}
}


subrutina Timeout {
	"\n­Vamos! Esto es para hoy...";
	hecho;
}
