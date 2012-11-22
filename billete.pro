//
// EL BILLETE: Aventura desarrollada por Ricardo P‚rez L¢pez
// ----------- como demostraci¢n para PROMETEO 2.2.1
//

vocabulario {
	direccion NORTE = { N };
	direccion SUR = { S };
	direccion ESTE = { E };
    direccion OESTE = { O };
	direccion ARRIBA = { SUBIR };
    direccion ABAJO = { BAJAR };
	direccion NOROESTE = { NO };
	direccion SUROESTE = { SO };
	direccion NORESTE = { NE };
	direccion SURESTE = { SE };
	verbo INVENTARIO = { INVEN, I };
	verbo COGE = { COGER };
	verbo DEJA = { DEJAR };
	verbo FIN = { ACABA, ACABAR };
	verbo MIRA = { MIRAR, M };
	verbo EXAMINA = { EX, EXAM };
	verbo PON = { PONER, PONTE };
	verbo QUITA = { QUITAR, QUITATE };
	verbo METE = { METER };
	verbo SACA = { SACAR };
	verbo DIAGNOSTICO = { DIAG };
	conjuncion Y, ENTONCES, DESPUES;
	nombre ANTORCHA, BOLSA, EMPAREDADO, ANORAK, MANZANA;
	nombre PIEDRA, BILLETE, CORREA;
	nombre BANCO, AUTOBUS, PERRO, PAJARO = { PAJARITO };
	adjetivo ENCENDIDA, APAGADA;
	verbo DI = { HABLA, DECIR, HABLAR };
	verbo ATA = { ATAR };
	verbo DESATA;
	verbo VEN, QUEDATE = { SIENTATE };
	verbo ENCIENDE;
	verbo APAGA = { APAGAR };
	verbo ESPERA, SALVA = { SALVAR, GRABAR, GRABA };
	verbo CARGAR = { CARGA, RECUPERA };
    verbo X;
        verbo LASER;
        nombre TODO;
}

LASER * {
        sonido("LASER");
}

localidades {
    0 : "\n\t\t\t\t\t\t\t\tEL BILLETE\n\nMientras esperaba el autob£s, una
         r faga de viento se llev¢ mi billete. ¨Puede Vd. ayudarme a
         encontrarlo?";

    2 : "Estoy en la parada del autob£s en una calle en direcci¢n Norte -
         Sur. Al Oeste queda un parque cuya verja de hierro est  abierta."
        { OESTE : 4 };

    3 : "La hierba sobre la cual camino est  muy bien cuidada. Hacia el Norte
         hay un c¢modo banco y hacia el Este queda un estanque."
        { NORTE : 4, ESTE : 6, NOROESTE : 7 };

    4 : "Estoy en un camino de grava que va de Este a Oeste, muy cercano a un
         c¢modo banco. Hacia el Sur hay un cuidado c‚sped y hacia el Norte
         hay un pabell¢n de m£sica."
        { NORTE : 5, ESTE : 2, SUR : 3, SUROESTE : 6, OESTE : 7 };

	5 : "Estoy en el pabell¢n de m£sica. Al Sur hay un camino de grava."
        { SUR : 4, SUROESTE : 7 };

    6 : "El sol crea bellos efectos de luz en la superficie del estanque,
         mecida por una suave brisa. Hay un camino al Norte que termina en un
         lloroso sauce. Al Este queda un cuidado c‚sped."
        { NORTE : 7, NORESTE : 4, ESTE : 3 };

    7 : "Estoy al lado de un sauce llor¢n. Al sur queda el estanque."
        { ARRIBA : 8, NORESTE : 5, ESTE : 4, SURESTE : 3, SUR : 6 };

    8 : "Estoy sentado en una rama del sauce, con una visi¢n panor mica del
         parque. Lejos al Este, m s all  de la verja del parque, puedo ver la
         parada del autob£s."
        { ABAJO : 7 };
}

objetos {
	ANTORCHA ENCENDIDA "la antorcha encendida"
			peso 1 inicial no_creado;
	BOLSA "la bolsa"
			peso 3 inicial 2 contenedor;
	EMPAREDADO "el emparedado"
			peso 1 inicial llevado;
	MANZANA "la manzana"
			peso 1 inicial llevado;
	BILLETE "el billete de autob£s"
			peso 1 inicial no_creado;
	PIEDRA "la piedra"
			peso 1 inicial 3;
	ANORAK "el anorak"
			peso 3 inicial encima ropa;
	ANTORCHA APAGADA "la antorcha apagada"
			peso 1 inicial llevado;
	CORREA "la correa de perro"
			peso 1 inicial llevado;
}


FIN * {
	si (acabar?) acabar;
	hecho;
}


X * {
    salidas;
}

COGE TODO {
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

COGE BILLETE {
	si (b12 = b38 y estaen?(BILLETE, no_creado)) {
		b5 := 0;
		hecho;
	}

	retorno;
}


COGE PAJARITO {
	si (b12 = b38) {
		"El pajarito me ignora.\n";
	}

	hecho;
}


COGE _ {
	autoc;
	hecho;
}

DEJA TODO {
        b28 := primero(llevado);
        b27 := objeto(b44, b45);

        si (b28 = -1)
                "No hay nada que dejar.";
        sino
                mientras (b28 <> -1) {
                        si (b27 <> b28)
                                dejar(b28);

                        b28 := siguiente(llevado);

                        si (b28 <> -1 y b27 <> b28)
                                saltolinea;
                }
        hecho;
}


DEJA * {
	si (esta?(8)) {
		si (llevado?(b58)) {
			"_ cae al suelo al pie del  rbol.\n";
			colocar(b58, 7);
			hecho;
		}
	}

	retorno;
}


DEJA _ {
	autod;
	hecho;
}


PON CORREA {
	si (nombre2?(PERRO) y llevado?(CORREA) y b13 = b38) {
        b14 := 1;
		destruir(CORREA);
		"Confiadamente, el perro me deja ponerle la correa
		alrededor del cuello.\n";
		hecho;
	} sino
		no_hecho;
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
	si (nombre2?(BOLSA))
		autom(BOLSA);
	sino
		no_hecho;

	hecho;
}


SACA * {
	si (nombre2?(BOLSA))
		autos(BOLSA);
	sino
		no_hecho;

	hecho;
}


MIRA BOLSA {
	"En la bolsa tienes ";
	listade(BOLSA);
	hecho;
}


MIRA _ {
	describe;
	hecho;
}


EXAMINA MANZANA {
    si (presente?(MANZANA)) {
		"La manzana est  madurita y muy sabrosa.\n";
        hecho;
	} sino
		no_hecho;

}


EXAMINA EMPAREDADO {
	si (presente?(EMPAREDADO)) {
		"Es un emparedado delicioso de jam¢n y queso.\n";
		hecho;
	} sino
		no_hecho;

}


EXAMINA BILLETE {
	si (presente?(BILLETE)) {
        "El billete tiene un n£mero y el nombre de la compa¤¡a
        de transportes para aventureros impreso.\n";
		hecho;
	} sino
		no_hecho;

}


EXAMINA BANCO {
	si (esta?(4)) {
		"El banco est  anclado firmemente a una base de concreto\n";
		hecho;
	} sino
		no_hecho;

}


EXAMINA PAJARITO {
    si (b12 = b38) {
		imagen_ciclo("BIRDSHOW.FLI");
		hecho;
	} sino
		no_hecho;

}


EXAMINA PERRO {
	si (b13 = b38) {
		"Es un chucho vagabundo y un poco maloliente.\n";
		hecho;
	} sino
		no_hecho;
}


EXAMINA _ {
	si (presente?(objeto(b34, b35))) {
		"No veo nada especial en _.\n";
		hecho;
	} sino
        mensis(26);                   // "No hay uno de esos por aqu¡"
}


INVENTARIO * {
	inven;
	hecho;
}


DIAG * {
	diag;
	hecho;
}


ENCIENDE ANTORCHA {
	si (llevado?(ANTORCHA APAGADA)) {
		intercambiar(ANTORCHA APAGADA, ANTORCHA ENCENDIDA);
		"Enciendes la antorcha.\n";
	}

	hecho;
}


APAGA ANTORCHA {
	si (llevado?(ANTORCHA ENCENDIDA)) {
		intercambiar(ANTORCHA ENCENDIDA, ANTORCHA APAGADA);
		"Apagas la antorcha\n";
	}

	hecho;
}


DI PERRO {
	si (b13 = b38) {
		si (analiza?) {
			llama(Habla_Perro);
		} sino
			"\6El perro me mira con amor.\7\n"; // No es frase v lida

		hecho;
	}

	retorno;
}


DI PAJARITO {
	si (b12 = b38) {
		"El pajarito me ignora.\n";
		hecho;
	}

	retorno;
}


DI _ {
    "¨A qui‚n se lo digo?\n"; 
	hecho;
}


ATA PERRO {
	llama(Atar_Perro);
	hecho;
}


ATA CORREA {
	llama(Atar_Perro);
	hecho;
}


ATA _ {
	no_hecho;
}


DESATA PERRO {
	llama(Desatar_Perro);
	hecho;
}


DESATA CORREA {
	llama(Desatar_Perro);
	hecho;
}


DESATA _ {
	no_hecho;
}


ESPERA * {
	"El tiempo pasa...\n";
	hecho;
}


SALVAR * {
	salvar;
	hecho;
}


CARGAR * {
	cargar;
	hecho;
}

subrutina Process1 {
    si (esta?(0)) {
        fuente("FUENTES\THIN.FNT"); // Nuevo en versi¢n 2.2
		tecla;
		b12 := 8;               // La localidad del pajarito
		b13 := 2;               // El perro en la parada del autob£s
		b15 := 10;
		b37 := 5;               // N§ m ximo de objetos llevables
		b42 := 5;
//              b53 := 1;
                b11 := 3;
		mueve(2);
		// El comando describe hace una descripci¢n de la localidad actual,
		// lo que incluye tambi‚n una llamada a Process1. Esto implica una
		// llamada recursiva que debe estar suficientemente protegida para
		// que no produzca resultados indeseados
		describe;
	} sino {
		si ((b0 = 0 y ausente?(ANTORCHA ENCENDIDA)) o b0 <> 0)
			listaobj;

		si (b12 = b38) {
			"\2Un pajarito anda rondando por aqu¡.\7\n";

			si (estaen?(BILLETE, no_creado))
				"\2El pajarito tiene un billete en su pico.\7\n";
		}

		si (b13 = b38) {        // ¨El perro est  en la misma localidad?
			"\6El perro est  aqu¡.\7\n";    // S¡, d¡selo al jugador
			si (b14 = 1)        // ¨Con la correa?
				"\6El perro va arrastrando la correa.\7\n";
			sino si (b14 = 2)   // ¨Est  el perro amarrado al banco?
				"\6El perro est  atado al banco con una correa.\7\n";
			sino si (b14 > 2)
				"\6El perro se sienta tranquilo.\7\n";
		}

	}
	retorno;
}


subrutina Process2 {
	si (esta?(2) y llevado?(BILLETE)) {
        "El autob£s llega. Le doy el billete al conductor, quien sonr¡e
        y dice:\n\"Siento haber llegado tarde, supongo que no ha tenido
        que esperar mucho\".\n";
		turnos;
		acabar;
	}

	llama(Perro);
	llama(Pajarito);

	retorno;
}


subrutina Pajarito {
	si (local(BILLETE) = b12 y b5 = 0) {   // ¨Va a volar el p jaro?
		destruir(BILLETE);                  // El p jaro coge el billete

		si (b12 = b38)
			"El pajarito se lleva velozmente el billete.\n";
	}

	si (b12 = 8 y b5 = 0) {                 // ¨Est  el p jaro en la rama?
		b12 := 5;
		b5  := 3;

		si (esta?(8))
			"\2El pajarito se va.\7\n";
	}

	si (b12 = 5 y b5 = 0) {                 // ¨Es tiempo de volar?
		b12 := 8;
		b5  := 3;

		si (esta?(5))
			"\2El pajarito se va.\7\n";
	}

	si (b5 = 3 y b12 = b38) {

		si (esta?(5))
			"\2Un peque¤o pajarito se posa en la hierba.\7\n";

		si (esta?(8))
			"\2El pajarito est  ahora en la rama del  rbol.\7\n";

		si (estaen?(BILLETE, no_creado))
			"\2El pajarito tiene un billete en su pico.\7\n";
	}

	si (b12 = b13) {
		b12 := 8;
		b5  := 3;
		si (esta?(5))           // ¨Est  el jugador en el pabell¢n de m£sica?
			"\2El pajarito ve al perro y sale pitando.\7\n";
	}

	si (local(EMPAREDADO) = b12 y estaen?(BILLETE, no_creado)) {
		colocar(BILLETE, 5);

		si (b12 = b38)
			"El pajarito deja caer el billete para coger el emparedado.\n";
	}

	retorno;
}


subrutina Perro {
	si (b13 <> b38 y b14 < 2 y no esta?(8)) {
		b13 := b38;
		"\6El perro me sigue moviendo la cola.\7\n";
	}

	retorno;
}


subrutina Habla_Perro {
	imprime(b14);
	si (verbo?(SIENTATE) y (b14 = 0 o b14 = 1)) { // ¨El perro no est  amarrado?
		b14 := 255;                         // Ahora est  sentado, quieto
		"\6El perro se sienta tranquilo.\7\n";
	} sino si (verbo?(VEN) y b14 = 255) {   // ¨El perro est  sentado?
		b14 := 0;                           // Ahora est  normal
		"\6El perro me sigue moviendo la cola.\7\n";
	} sino
		"\6El perro me mira con amor.\7\n";    // Cualquier otra cosa

	retorno;
}


subrutina Atar_Perro {
	si (nombre2?(BANCO) y esta?(4) y b13 = b38 y b14 = 1) {
		b14 := b14 + 1;
		"He amarrado el perro al banco.\n";
	} sino
		no_hecho;

	retorno;
}


subrutina Desatar_Perro {
	si (esta?(4) y b14 = 2) {
		b14 := 0;
		"He desatado al perro del banco.\n";
		coger(CORREA);
	}

	retorno;
}


subrutina Timeout {
	"\nEl tiempo pasa...\n";
        hecho;
}
