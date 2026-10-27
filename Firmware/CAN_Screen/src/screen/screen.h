#pragma once

#include "Arduino.h"
#include "./custom_char.h"
#include "./vfd.h"
#include "../board/enums.h"

#define VFD_NBCHARS					20						// Nombre de caractères affichables de l'écran.
#define VFD_MAXCHARS					(VFD_NBCHARS * 2)				// Nombre de caractères en mémoire de l'écran.
#define VFD_NBLINES					2						// Nombre de lignes de l'écran.

#define VFD_NBPAGESHORIZ				3						// Nombre de pages horizontales ( scroll ) à afficher à l'écran.
#define VFD_NBPAGESVERT					7						// Nombre de pages verticales ( lignes ) à afficher à l'écran.
#define VFD_ALLCHARS					(VFD_NBCHARS * VFD_NBPAGESHORIZ)

#define VFD_POSITION(pos,P)				(pos + (VFD_NBCHARS * (P - 1)))

#define CALENDAR_PAGE					1
#define HOURSPOS_LITTLE					VFD_POSITION( 6, CALENDAR_PAGE)			// Format:
#define MINUTESPOS_LITTLE				VFD_POSITION( 9, CALENDAR_PAGE)
#define SECONDSPOS_LITTLE				VFD_POSITION(12, CALENDAR_PAGE)			// ▒▒▒▒▒▒__:__:__▒▒▒▒▒▒
#define DAYSPOS_LITTLE					VFD_POSITION( 5, CALENDAR_PAGE)			// ▒▒▒▒▒__/__/____▒▒▒▒▒
#define MONTHSPOS_LITTLE				VFD_POSITION( 8, CALENDAR_PAGE)
#define YEARSPOS_LITTLE					VFD_POSITION(11, CALENDAR_PAGE)
#define HOURSPOS_BIG					VFD_POSITION( 1, CALENDAR_PAGE)			// Format:
#define MINUTESPOS_BIG					VFD_POSITION( 5, CALENDAR_PAGE)
#define SECONDSPOS_BIG					VFD_POSITION( 9, CALENDAR_PAGE)			// ▒__.▒__.▒__▒@@@@@@@@
#define DAYSPOS_BIG					VFD_POSITION(15, CALENDAR_PAGE)			// ▒__.▒__.▒__▒▒▒▒__/__
#define MONTHSPOS_BIG					VFD_POSITION(18, CALENDAR_PAGE)
#define DAYSSTRPOS_BIG					VFD_POSITION(12, CALENDAR_PAGE)
#define CALENDAR_RELATIVE(pos)				((CALENDAR_PAGE * VFD_NBCHARS) - pos)

#define INFORMATION_PAGE				2
#define NAME_POS					VFD_POSITION( 0, INFORMATION_PAGE)
#define TEMP_INT_POS					VFD_POSITION( 7, INFORMATION_PAGE)		// Format:
#define TEMP_FRAC_POS					VFD_POSITION(10, INFORMATION_PAGE)
#define TEMP_TYPE_POS					VFD_POSITION(12, INFORMATION_PAGE)		// ▒▒▒▒▒DUE▒Rhéobus▒▒▒▒
#define FAN_POS						VFD_POSITION(14, INFORMATION_PAGE)		// @@@@@@-__,__°C▒☼▒####
#define SPEED_POS					VFD_POSITION(16, INFORMATION_PAGE)

#define LEVEL_PAGE					3

#define SCROLLINGSPEEDMS 100000										// Vitesse en mS de déplacement des caractères.

typedef struct {
	uint8_t BigClock: 1;					// Affichage de l'heure sur 2 lignes
	uint8_t Use2Digits: 1;					// Pour les chiffres inférieurs à 10, ajoute un 0 devant ( ex: 05 )
} clock_view_t;

class CLASS_SCREEN_ {
private:
	typedef struct {
		char UpperLine [ VFD_MAXCHARS ];		// Le contenu complet de la première ligne ( 40 caractères ! )
		char LowerLine [ VFD_MAXCHARS ];		// Le contenu complet de la seconde ligne ( 40 caractères ! )
		uint32_t UpdateTime;				// Temps ( en secondes ) entre 2 animations
		uint32_t UpdateLineTime;			// Temps ( en secondes ) avant changement de l'affichage des lignes.
		uint32_t ScrollTime;				// Temps ( en micro-secondes ) entre chaque pas d'une animation
		uint8_t ScrollPosition;				// Position de l'écran ( Entre 0 et 20. Au delà de 20, les caractères reviennent à la suite. ) Cylindrique !
		bool ViewSpecial;				// TRUE = Affichage des informations. FALSE = Affichage de l'heure
		bool ping_pong;					// Pour changer le sens de déplacement. Permet d'aller en négatif dans le déplacement et utiliser une caractéristique de l'écran !
		uint8_t Line_Number;				// Le nombre de lignes d'information qu'il est possible d'afficher.

		//bool View_Special ()			{ return ( this -> ScrollPosition == 0 ); }
		//bool View_Calendar ()			{ return ( this -> ScrollPosition == ( ( CALENDAR_PAGE - 1 ) * VFD_NBCHARS ) ); }
		//bool View_Informations ()		{ return ( this -> ScrollPosition == ( ( INFORMATION_PAGE - 1 ) * VFD_NBCHARS ) ); }
	} screen_view_t;
	typedef struct {
		typedef union {
			struct {
				uint8_t MCU: 1;			// Affichage de la température du µC
				uint8_t Water: 1;		// Affichage dela température d'eau		( Si connecté )
				uint8_t Flow: 1;		// Affichage du débit d'eau			( Si connecté )
				uint8_t Battery: 1;		// Affichage de la tension de la pile
				uint8_t Power: 1;		// Affichage de la tension +5V
				uint8_t Logic: 1;		// Affichage de la tension +3,3V
				uint8_t Box: 1;			// Affichage de la température du boîtier	( Si connecté )
				__I uint8_t : 1;
			} bit;
			uint8_t View;
		} view_t;

		view_t Lignes;
		uint8_t nb_Lignes () {				// Nombre de lignes réellement visibles ( Entre 4 et 7 );
			uint8_t _number = 0;
			for ( uint8_t _bit = 0; _bit < 8; _bit ++ ) {
				if ( bitRead ( this -> Lignes.View, _bit ) ) _number ++;
			}
			return _number;
		}
		void get_Rows ( uint8_t *_Rows_ ) {
			memset ( _Rows_, 255, 7 );
			uint8_t _number = 0;
			for ( uint8_t _bit = 0; _bit < 8; _bit ++ ) {
				if ( bitRead ( this -> Lignes.View, _bit ) ) {
					_Rows_ [ _number ] = _bit;
					_number ++;
				}
			}
		}
	} standAlone_view_t;

	CLASS_VFD_ friend_;				// Pour avoir accès à la classe CLASS_VFD_ et ses fonctions privées !
	clock_view_t clock_view_;
	screen_view_t screen_;
	board_state_t state_;
	vfd_brightness_t _brightness;

	bool _mustUpdateChars;
	bool _animate;
	//uint32_t _screenDisplayUpdateTime;

	const char _daysStr [ 7 ] [ 9 ] = { "  Samedi", "Dimanche", "   Lundi", "   Mardi", "Mercredi", "   Jeudi", "Vendredi" };
	const char _internalStr [ 7 ] [ 7 ] = { "MCU:  ", "Eau:  ", "Debit:", "Pile: ", "+5V:  ", "+3,3V ", "Box:  " };
	const char _timeStr_Little [ VFD_NBCHARS ]  = { ' ', ' ', ' ', ' ', ' ', ' ', '#', '#', ':', '#', '#', ':', '#', '#', ' ', ' ', ' ', ' ', ' ', ' ' };
	const char _dateStr_Little [ VFD_NBCHARS ]  = { ' ', ' ', ' ', ' ', ' ', '#', '#', '/', '#', '#', '/', '#', '#', '#', '#', ' ', ' ', ' ', ' ', ' ' };
	const char _timeDayStr_Big [ VFD_NBCHARS ]  = { ' ', '&', '&', 0x2E, ' ', '&', '&', 0x2E, ' ', '&', '&', ' ', '@', '@', '@', '@', '@', '@', '@', '@' };
	const char _timeDateStr_Big [ VFD_NBCHARS ] = { ' ', '&', '&', 0xA5, ' ', '&', '&', 0xA5, ' ', '&', '&', ' ', ' ', ' ', ' ', '#', '#', '/', '#', '#' };

	bool updateChars ();
	void updateTime ();
	void updateDate ();
	void updateInformations ();
	void writeBigInt ( uint8_t _position_, size_t _value_ , bool _use2Digits_ = true );
	void writeManyInt ( char *_string_, uint8_t _position_, size_t _value_, uint8_t _size_ = 1, bool _use2Digits_ = true );
	void writeTemperature ( char *_string_, int16_t _value_, uint8_t _type_ );
	void writeTempAndSpeed ( char *_string_, const char _name_ [ 7 ], int16_t _temperature_, uint8_t _type_, uint8_t _animated_char_, int16_t _speed_ = -1 );
	void writeOnlyTemp ( char *_string_, const char _name_ [ 7 ], int16_t _temperature_, uint8_t _type_ );
	void writeOnlySpeed ( char *_string_, const char _name_ [ 7 ], uint8_t _animated_char_, int16_t _speed_ = -1 );
	void scrollScreen ();
	void changeTimeFormat ();

public:
	CLASS_SCREEN_ ();
	void loading ();
	uint8_t loading ( uint8_t _percent_ );
	void begin ( vfd_brightness_t _brightness_ = VFD_Brightness100 );
	void loop ();
	void setBrightness ( vfd_brightness_t _brightness_ = VFD_Brightness100 );
	void refresh_HalfSecond ()									{ this -> _animate ^= 0x1; }
	clock_view_t Display () const									{ return this -> clock_view_;}
	//void wakeup ();
	//void shutdown ();
};

extern CLASS_SCREEN_ screen_;

