#pragma once

//BEGIN - Caractère personnalisé représentant un processeur.
	const byte __cpu_char1__ [ 8 ] = {
		0b00010,
		0b00111,
		0b11100,
		0b00101,
		0b00101,
		0b11100,
		0b00111,
		0b00010
	};

	const byte __cpu_char2__ [ 8 ] = {
		0b01000,
		0b11100,
		0b00111,
		0b10100,
		0b10100,
		0b00111,
		0b11100,
		0b01000
	};

	// Assemblage...
	const char __cpu_char__ [ 2 ] = { 0x00, 0x01 };
//END

//BEGIN - Caractère personnalisé pour afficher des °K.
	const byte __char_kelvin__ [ 8 ] = {
		0b11000,
		0b11000,
		0b00000,
		0b01001,
		0b01010,
		0b01100,
		0b01010,
		0b00000
	};
//END

//BEGIN - Caractères personnalisés animés.
	const byte __fan_char_frame1__ [ 8 ] = {
		0b00000,
		0b01000,
		0b01011,
		0b00100,
		0b11010,
		0b00010,
		0b00000,
		0b00000
	};

	const byte __fan_char_frame2__ [ 8 ] = {
		0b00000,
		0b00010,
		0b10100,
		0b01110,
		0b00101,
		0b01000,
		0b00000,
		0b00000
	};
//END

//BEGIN - Caractères personnalisés, heure sur deux lignes.
	const byte __number0_char__ [ 8 ] = {
		0b00000,
		0b00000,
		0b00001,
		0b00001,
		0b00001,
		0b00001,
		0b00001,
		0b00001
	};

	const byte __number1_char__ [ 8 ] = {
		0b00000,
		0b00000,
		0b10000,
		0b10001,
		0b10001,
		0b10001,
		0b10001,
		0b10001
	};

	const byte __number2_char__ [ 8 ] = {
		0b01110,
		0b00001,
		0b00001,
		0b00001,
		0b00001,
		0b00001,
		0b00001,
		0b00000
	};

	const byte __number3_char__ [ 8 ] = {
		0b00000,
		0b00000,
		0b01110,
		0b10000,
		0b10000,
		0b10000,
		0b10000,
		0b10000
	};

	const byte __number4_char__ [ 8 ] = {
		0b01110,
		0b10000,
		0b10000,
		0b10000,
		0b10000,
		0b10000,
		0b01110,
		0b00000
	};

	const byte __number5_char__ [ 8 ] = {
		0b01110,
		0b00001,
		0b00001,
		0b00001,
		0b00001,
		0b00001,
		0b01110,
		0b00000
	};

	const byte __number6_char__ [ 8 ] = {
		0b00000,
		0b00000,
		0b01110,
		0b10001,
		0b10001,
		0b10001,
		0b10001,
		0b10001
	};

	const byte __number7_char__ [ 8 ] = {
		0b00000,
		0b00000,
		0b01110,
		0b00001,
		0b00001,
		0b00001,
		0b00001,
		0b00001
	};

	// Assemblage de caractères pour former un chiffre sur 2 x ( 5 x 8 pixels ). Chiffres dans l'ordre 0 à 9 !
	const char __numbers__ [ 10 ] [ 2 ] = {	{ 0x6, 'U' },		// Chiffre 0.
						{ 0x0, 0x18 },		// Chiffre 1.
						{ 0x7, 0x4 },		// Chiffre 2.
						{ 0x7, 0x5 },		// Chiffre 3.
						{ 0x1, 0x2 },		// Chiffre 4.
						{ 0x3, 0x5 },		// Chiffre 5.
						{ 0x3, 'O' },		// Chiffre 6.
						{ 0x7, 0x18 },		// Chiffre 7.
						{ 0x6, 'O' },		// Chiffre 8.
						{ 0x6, 0x5 }		// Chiffre 9.
	};
//END

//BEGIN - Caractères personnalisés, Vu-mètre vertical.
	const byte __level0_char__ [ 8 ] = {
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b11111
	};

	const byte __level1_char__ [ 8 ] = {
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b11111,
		0b11111
	};

	const byte __level2_char__ [ 8 ] = {
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b11111,
		0b11111,
		0b11111
	};

	const byte __level3_char__ [ 8 ] = {
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b11111,
		0b11111,
		0b11111,
		0b11111
	};

	const byte __level4_char__ [ 8 ] = {
		0b00000,
		0b00000,
		0b00000,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111
	};

	const byte __level5_char__ [ 8 ] = {
		0b00000,
		0b00000,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111
	};

	const byte __level6_char__ [ 8 ] = {
		0b00000,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111
	};

	const byte __level7_char__ [ 8 ] = {
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111
	};

	const char __level__ [ 17 ] [ 2 ] = {	{ ' ', ' ' },			//   0,00 %
						{ ' ', 0x0 },			//   6,25 %
						{ ' ', 0x1 },			//  12,50 %
						{ ' ', 0x2 },			//  18,75 %
						{ ' ', 0x3 },			//  25,00 %
						{ ' ', 0x4 },			//  31,25 %
						{ ' ', 0x5 },			//  37,50 %
						{ ' ', 0x6 },			//  43,75 %
						{ ' ', 0x7 },			//  50,00 %
						{ 0x0, 0x7 },			//  56,25 %
						{ 0x1, 0x7 },			//  62,50 %
						{ 0x2, 0x7 },			//  68,75 %
						{ 0x3, 0x7 },			//  75,00 %
						{ 0x4, 0x7 },			//  81,25 %
						{ 0x5, 0x7 },			//  87,50 %
						{ 0x6, 0x7 },			//  93,75 %
						{ 0x7, 0x7 }			// 100,00 %
	};
	// caractère supérieur = SI(OU(pourcentage="";pourcentage<50);"";SI(pourcentage>99;"7";ENT((pourcentage-50)/7,14)+1))
	// caractère inférieur = SI(OU(pourcentage="";pourcentage=0);"";SI(pourcentage>=50;"7";ENT(pourcentage/7,14)+1))
//END
