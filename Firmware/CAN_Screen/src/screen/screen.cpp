#include "./screen.h"
#include "../board/macros.h"
#include "../board/rtc.h"
#include "../sensors/flow.h"
#include "../sensors/monitor.h"
#include "../sensors/temperature.h"


CLASS_SCREEN_::CLASS_SCREEN_ () {
	this -> clock_view_.BigClock = true;
	this -> state_.Display = friend_.displayControl_.bit.Display;
	memset ( this -> screen_.UpperLine, ' ', sizeof ( this -> screen_.UpperLine ) );
	memset ( this -> screen_.LowerLine, ' ', sizeof ( this -> screen_.LowerLine ) );
}

/** @brief
 *  @details
 */
bool CLASS_SCREEN_::updateChars () {
	if ( this -> _mustUpdateChars ) {
		//if ( ( this -> screen_.ScrollPosition < CALENDAR_RELATIVE ( 6 ) ) && ( this -> clock_view_.BigClock ) ) {
		if ( ( this -> screen_.ScrollPosition < VFD_NBCHARS ) && ( this -> clock_view_.BigClock ) ) {
			//this -> screen_.LowerLine [ FAN_POS ] = ' ';
			friend_.createChar ( 0, ( uint8_t* ) __number0_char__ );
			friend_.createChar ( 1, ( uint8_t* ) __number1_char__ );
			friend_.createChar ( 2, ( uint8_t* ) __number2_char__ );
			friend_.createChar ( 3, ( uint8_t* ) __number3_char__ );
			friend_.createChar ( 4, ( uint8_t* ) __number4_char__ );
			friend_.createChar ( 5, ( uint8_t* ) __number5_char__ );
			friend_.createChar ( 6, ( uint8_t* ) __number6_char__ );
			friend_.createChar ( 7, ( uint8_t* ) __number7_char__ );
		}

		//if ( this -> screen_.ScrollPosition > CALENDAR_RELATIVE ( 10 ) ) {
		if ( this -> screen_.ScrollPosition > ( VFD_NBCHARS - 1 ) ) {
			friend_.createChar ( 0, ( uint8_t* ) __fan_char_frame1__ );
			friend_.createChar ( 1, ( uint8_t* ) __fan_char_frame2__ );
			friend_.createChar ( 7, ( uint8_t* ) __char_kelvin__ );
		}

		return false;
	}

	return true;
}

/** @brief
 *  @details
 */
void CLASS_SCREEN_::updateTime () {
	if ( rtc_.RTC_State ().State.bit.Configured ) {
		if ( clock_view_.BigClock ) {
			this -> writeBigInt ( HOURSPOS_BIG, rtc_.get_Hour (), this -> clock_view_.Use2Digits );
			this -> writeBigInt ( MINUTESPOS_BIG, rtc_.get_Minute () );
			this -> writeBigInt ( SECONDSPOS_BIG, rtc_.get_Second () );

			switch ( rtc_.am_pm () ) {							// Cas de l'affichage horaire sur 12h
				case hour_am:
					this -> screen_.UpperLine [ HOURSPOS_BIG - 1 ] = 'a';
					this -> screen_.LowerLine [ HOURSPOS_BIG - 1 ] = 'm';
					break;
				case hour_pm:
					this -> screen_.UpperLine [ HOURSPOS_BIG - 1 ] = 'p';
					this -> screen_.LowerLine [ HOURSPOS_BIG - 1 ] = 'm';
					break;
			}
		}
		else {
			this -> writeManyInt ( this -> screen_.UpperLine, HOURSPOS_LITTLE, rtc_.get_Hour (), 1, clock_view_.Use2Digits );
			this -> writeManyInt ( this -> screen_.UpperLine, MINUTESPOS_LITTLE, rtc_.get_Minute () );
			this -> writeManyInt ( this -> screen_.UpperLine, SECONDSPOS_LITTLE, rtc_.get_Second () );

			switch ( rtc_.am_pm () ) {							// Cas de l'affichage horaire sur 12h
				case hour_am:
					this -> screen_.UpperLine [ HOURSPOS_LITTLE - 2 ] = 'a';
					this -> screen_.UpperLine [ HOURSPOS_LITTLE - 1 ] = 'm';
					break;
				case hour_pm:
					this -> screen_.UpperLine [ HOURSPOS_LITTLE - 2 ] = 'p';
					this -> screen_.UpperLine [ HOURSPOS_LITTLE - 1 ] = 'm';
					break;
			}
		}
	}
}

/** @brief
 *  @details
 */
void CLASS_SCREEN_::updateDate () {
	if ( rtc_.RTC_State ().State.bit.Configured ) {
		if ( clock_view_.BigClock ) {
			memmove ( this -> screen_.UpperLine + DAYSSTRPOS_BIG, this -> _daysStr [ rtc_.get_DayOfWeek () ], 8 );
			this -> writeManyInt ( this -> screen_.LowerLine, DAYSPOS_BIG, rtc_.get_Day (), 1, clock_view_.Use2Digits );
			this -> writeManyInt ( this -> screen_.LowerLine, MONTHSPOS_BIG, rtc_.get_Month (), 1, clock_view_.Use2Digits );
		}
		else {
			this -> writeManyInt ( this -> screen_.LowerLine, DAYSPOS_LITTLE, rtc_.get_Day (), 1, clock_view_.Use2Digits );
			this -> writeManyInt ( this -> screen_.LowerLine, MONTHSPOS_LITTLE, rtc_.get_Month (), 1, clock_view_.Use2Digits );
			this -> writeManyInt ( this -> screen_.LowerLine, YEARSPOS_LITTLE, rtc_.get_Year (), 1, clock_view_.Use2Digits );
		}
	}
}

void CLASS_SCREEN_::updateInformations () {
	if ( this -> state_.is_StandAlone () ) {
		high_flow_t self_Flow_;						// Tampon pour le débimètre
		voltage_t self_Monitor_;					// Tampon pour les tensions
		temperatures_t self_Temperatures_;				// Tampon pour les températures
		standAlone_view_t visuel_;

		debimetre_.get ( &self_Flow_ );					// Réccupération du débit mesuré par la carte.
		monitor_.get ( &self_Monitor_ );				// Réccupération des tensions mesurées par la carte.
		temperatures_.get ( &self_Temperatures_ );			// Réccupération des températures mesurées par la carte.

		visuel_.Lignes.View = 0x39;					// Au minimum, température du µC, tension de pile, +3,3V et +5V !
		visuel_.Lignes.bit.Flow = ( self_Flow_.Flow != FLOW_DISCONNECTED );
		visuel_.Lignes.bit.Box = ( self_Temperatures_.Box != SENSOR_DISCONNECTED );
		visuel_.Lignes.bit.Water = ( self_Temperatures_.Water != SENSOR_DISCONNECTED );

		if ( ( time_.seconds - this -> screen_.UpdateLineTime ) > 5 ) {
			uint8_t _Rows [ 7 ];
			visuel_.get_Rows ( _Rows );				// Enregistrement des lignes actives et leur ordre !
			this -> screen_.Line_Number %= ( visuel_.nb_Lignes () );

			switch ( _Rows [ this -> screen_.Line_Number ] ) {
				case 0:
					this -> writeOnlyTemp ( this -> screen_.UpperLine, this -> _internalStr [ 0 ], self_Temperatures_.Convert ( self_Temperatures_.Chip ), self_Temperatures_._type );
					break;
				case 1:
					this -> writeOnlyTemp ( this -> screen_.UpperLine, this -> _internalStr [ 1 ], self_Temperatures_.Convert ( self_Temperatures_.Water ), self_Temperatures_._type );
					break;
				case 2:
					this -> writeOnlySpeed ( this -> screen_.UpperLine, this -> _internalStr [ 2 ], 0x04, self_Flow_.Flow );
					break;
				case 3:
					this -> writeOnlyTemp ( this -> screen_.UpperLine, this -> _internalStr [ 3 ], ( int16_t ) ( self_Monitor_.VBatt () / 10 ), 'V' );
					break;
				case 4:
					this -> writeOnlyTemp ( this -> screen_.UpperLine, this -> _internalStr [ 4 ], ( int16_t ) ( self_Monitor_.VPower () / 10 ), 'V' );
					break;
				case 5:
					this -> writeOnlyTemp ( this -> screen_.UpperLine, this -> _internalStr [ 5 ], ( int16_t ) ( self_Monitor_.VLogic () / 10 ), 'V' );
					break;
				case 6:
					this -> writeOnlyTemp ( this -> screen_.UpperLine, this -> _internalStr [ 6 ], self_Temperatures_.Convert ( self_Temperatures_.Box ), self_Temperatures_._type );
					break;
			}

			switch ( _Rows [ this -> screen_.Line_Number + 1 ] == 255 ? 0 : _Rows [ this -> screen_.Line_Number + 1 ] ) {
				case 0:
					this -> writeOnlyTemp ( this -> screen_.LowerLine, this -> _internalStr [ 0 ], self_Temperatures_.Convert ( self_Temperatures_.Chip ), self_Temperatures_._type );
					break;
				case 1:
					this -> writeOnlyTemp ( this -> screen_.LowerLine, this -> _internalStr [ 1 ], self_Temperatures_.Convert ( self_Temperatures_.Water ), self_Temperatures_._type );
					break;
				case 2:
					this -> writeOnlySpeed ( this -> screen_.LowerLine, this -> _internalStr [ 2 ], 0x04, self_Flow_.Flow );
					break;
				case 3:
					this -> writeOnlyTemp ( this -> screen_.LowerLine, this -> _internalStr [ 3 ], ( int16_t ) ( self_Monitor_.VBatt () / 10 ), 'V' );
					break;
				case 4:
					this -> writeOnlyTemp ( this -> screen_.LowerLine, this -> _internalStr [ 4 ], ( int16_t ) ( self_Monitor_.VPower () / 10 ), 'V' );
					break;
				case 5:
					this -> writeOnlyTemp ( this -> screen_.LowerLine, this -> _internalStr [ 5 ], ( int16_t ) ( self_Monitor_.VLogic () / 10 ), 'V' );
					break;
				case 6:
					this -> writeOnlyTemp ( this -> screen_.LowerLine, this -> _internalStr [ 6 ], self_Temperatures_.Convert ( self_Temperatures_.Box ), self_Temperatures_._type );
					break;
			}

			this -> screen_.Line_Number ++;
			this -> screen_.UpdateLineTime = time_.seconds;
		}
	}
	else {
	}
}

/** @brief
 *  @details
 *  @param _position_
 *  @param _value_
 */
void CLASS_SCREEN_::writeBigInt ( uint8_t _position_, size_t _value_, bool _use2Digits_ ) {
	if ( _position_ <= ( VFD_MAXCHARS - 2 ) ) {
		if ( _value_ < 10 ) {
			if ( _use2Digits_ ) {
				this -> screen_.UpperLine [ _position_ ] = __numbers__ [ 0 ] [ 0 ];
				this -> screen_.LowerLine [ _position_ ] = __numbers__ [ 0 ] [ 1 ];
			}
			else {
				this -> screen_.UpperLine [ _position_ ] = ' ';
				this -> screen_.LowerLine [ _position_ ] = ' ';
			}
			this -> screen_.UpperLine [ _position_ + 1 ] = __numbers__ [ _value_ ] [ 0 ];
			this -> screen_.LowerLine [ _position_ + 1 ] = __numbers__ [ _value_ ] [ 1 ];
		}

		if ( ( _value_ >= 10 ) && ( _value_ < 100 ) ) {
			this -> screen_.UpperLine [ _position_ ] = __numbers__ [ ( _value_ / 10 ) ] [ 0 ];
			this -> screen_.UpperLine [ _position_ + 1 ] = __numbers__ [ ( _value_ % 10 ) ] [ 0 ];
			this -> screen_.LowerLine [ _position_ ] = __numbers__ [ ( _value_ / 10 ) ] [ 1 ];
			this -> screen_.LowerLine [ _position_ + 1 ] = __numbers__ [ ( _value_ % 10 ) ] [ 1 ];
		}

		if ( _value_ >= 100 ) {										// Ne devrait pas arriver pour l'affichage des heures !
			memset ( this -> screen_.UpperLine + _position_, __numbers__ [ 9 ] [ 0 ], 2 );
			memset ( this -> screen_.LowerLine + _position_, __numbers__ [ 9 ] [ 1 ], 2 );
		}
	}
}

/** @brief
 *  @details
 *  @param *_string_		La chaine de texte dans laquelle on insère la valeur.
 *  @param _position_		Position dans la chaine de texte ou l'on insère la valeur.
 *  @param _value_		La valeur à insérer !
 *  @param _size_		Nombre de bytes. Equivalent à sizeof ( _value_ )
 *  @param _use2Digits_		Format de la valeur. ( ' #' ou '##' ) !
 */
void CLASS_SCREEN_::writeManyInt ( char *_string_, uint8_t _position_, size_t _value_, uint8_t _size_, bool _use2Digits_ ) {
	if ( ( _size_ == 1 ) && ( _position_ < ( VFD_MAXCHARS - 2 ) ) ) {
		if ( _value_ < 10 ) {
			_string_ [ _position_ ] = ( _use2Digits_ ? '0' : ' ' );
			_string_ [ _position_ + 1 ] = _value_ | 0x30;
		}
		if ( ( _value_ >= 10 ) && ( _value_ < 100 ) ) {
			_string_ [ _position_ ] = ( ( uint8_t ) ( _value_ / 10 ) ) | 0x30;
			_string_ [ _position_ + 1 ] = ( ( uint8_t ) ( _value_ % 10 ) ) | 0x30;
		}
		if ( _value_ >= 100 ) memset ( _string_ + _position_, '9', 2 );
	}

	if ( ( _size_ == 2 ) && ( _position_ < ( VFD_MAXCHARS - 3 ) ) ) {									// Pour écrire les vitesses... Ou les années !
		if ( ( _value_ == 0 ) || ( bitRead ( _value_, 15 ) ) ) {									// Valeur nulle ou négative !
			memset ( _string_ + _position_, ' ', 4 );										// Inutile d'avoir un affichage !
		}
		else {
			char _tempChar [ 4 ];
			_tempChar [ 0 ] = ( ( uint8_t ) ( _value_ / 1000 ) ) | 0x30;								// uint8_t 0 - 10 -> ascii 0 - 10. Conversion d'un chiffre en ascii.
			_tempChar [ 1 ] = ( ( ( uint8_t ) ( _value_ / 100 ) ) % 10 ) | 0x30;
			_tempChar [ 2 ] = ( ( ( uint8_t ) ( _value_ / 10 ) ) % 10 ) | 0x30;
			_tempChar [ 3 ] = ( ( uint8_t ) ( _value_ % 10 ) ) | 0x30;

			while ( _tempChar [ 0 ] == '0' ) {											// Tant que le chiffre débute par un zéro,
				memmove ( _tempChar, _tempChar + 1, 3 );									// On décale.
				_tempChar [ 3 ] = ' ';												// Écriture d'un ESPACE à la fin.
			}

			memcpy ( _string_ + _position_, _tempChar, 4 );
		}
	}
}

/** @brief
 *  @details
 *  @param *_string_
 *  @param _value_
 */
void CLASS_SCREEN_::writeTemperature ( char *_string_, int16_t _value_, uint8_t _type_ ) {
	uint16_t _intPartTemp;
	uint8_t _fraqPartTemp;

	if ( ! bitRead ( _value_, 15 ) ) {							// La valeur est strictement positive ( 0x8000 ) signe le nombre... )
		_intPartTemp = _value_ / 100;							// Réccupération de la partie entière, ...
		_fraqPartTemp = _value_ % 100;							// ... et de la partie fractionelle.
		_string_ [ TEMP_INT_POS - 1 ] = ' ';
	}
	else {											// La valeur est négative
		int16_t _negativeTemp = MASK_16BITS - _value_;
		_intPartTemp = _negativeTemp / 100;
		_fraqPartTemp = _negativeTemp % 100;
		_string_ [ TEMP_INT_POS - 1 ] = '-';
	}

	this -> writeManyInt ( _string_, TEMP_INT_POS, _intPartTemp, 1, false );		// Écriture de la partie entière
	_string_ [ TEMP_FRAC_POS - 1 ] = ',';							// Écriture de la virgule de séparation
	this -> writeManyInt ( _string_, TEMP_FRAC_POS, _fraqPartTemp, 1, true );		// Écriture de la partie fractionelle ( Avec 2 digits -> 0# )
	//_string_ [ TEMP_TYPE_POS ] = _type_;							// Écriture de l'unité affichée ( °C, °F, °K )
	_string_ [ TEMP_TYPE_POS ] = ( ( _type_ == deg_Kelvin ) && ( this -> screen_.ScrollPosition < VFD_NBCHARS ) ) ? ' ' : _type_;
}

/** @brief
 *  @details
 *  @param *_string_		La chaine de texte dans laquelle on insère la valeur.
 *  @param _name_ [ 7 ]		Le texte d'introduction. Maximum 6 caractère !		( ex: "Box:  " )
 *  @param _temperature_	La température à afficher
 *  @param _type_		Le type de température ( °C, °K, °F. Voir degree_t qui retourne le numéro mémoire du caractère à afficher )
 *  @param _animated_char_	L'adresse mémoire de la première frame du caractère animé à afficher pour simuler la vitesse/débit.
 *  @param _speed_		La vitesse ou le débit à afficher. Si < à 1 aucun affichage !
 */
void CLASS_SCREEN_::writeTempAndSpeed ( char *_string_, const char _name_ [ 7 ], int16_t _temperature_, uint8_t _type_, uint8_t _animated_char_, int16_t _speed_ ) {
	memset ( _string_ + NAME_POS, ' ', VFD_NBCHARS );					// Mise en blanc de la zone.
	memcpy ( _string_ + NAME_POS, _name_, 6 );						// Copie de l'intitulé.		Juste 6 caractères, car le dernier est un caractère d'échappement non affichable ( String )
	this -> writeTemperature ( _string_, _temperature_, _type_ );
	if ( ( _speed_ > 0 ) && ( this -> screen_.ScrollPosition < VFD_NBCHARS ) ) {		// Si il y as une vitesse...
		bitWrite ( _animated_char_,0 , this -> _animate );
		_string_ [ FAN_POS ] = _animated_char_;						// ... affichage de l'animation
	}
	this -> writeManyInt ( _string_, SPEED_POS, _speed_, 2, false );			// Affichage de la vitesse ou du débit.
}

/** @brief
 *  @details
 *  @param *_string_		La chaine de texte dans laquelle on insère la valeur.
 *  @param _name_ [ 7 ]		Le texte d'introduction. Maximum 6 caractère !		( ex: "Box:  " )
 *  @param _temperature_	La température à afficher
 *  @param _type_		Le type de température ( °C, °K, °F. Voir degree_t qui retourne le numéro mémoire du caractère à afficher )
 */
void CLASS_SCREEN_::writeOnlyTemp ( char *_string_, const char _name_ [ 7 ], int16_t _temperature_, uint8_t _type_ ) {
	memset ( _string_ + NAME_POS, ' ', VFD_NBCHARS );					// Mise en blanc de la zone.
	memcpy ( _string_ + NAME_POS, _name_, 6 );						// Copie de l'intitulé.		Juste 6 caractères, car le dernier est un caractère d'échappement non affichable ( String )
	this -> writeTemperature ( _string_, _temperature_, _type_ );
}

/** @brief
 *  @details
 *  @param *_string_		La chaine de texte dans laquelle on insère la valeur.
 *  @param _name_ [ 7 ]		Le texte d'introduction. Maximum 6 caractère !		( ex: "Box:  " )
 *  @param _animated_char_	L'adresse mémoire de la première frame du caractère animé à afficher pour simuler la vitesse/débit.
 *  @param _speed_		La vitesse ou le débit à afficher. Si < à 1 aucun affichage !
 */
void CLASS_SCREEN_::writeOnlySpeed ( char *_string_, const char _name_ [ 7 ], uint8_t _animated_char_, int16_t _speed_ ) {
	memset ( _string_ + NAME_POS, ' ', VFD_NBCHARS );					// Mise en blanc de la zone.
	memcpy ( _string_ + NAME_POS, _name_, 6 );						// Copie de l'intitulé.
	if ( ( _speed_ > 0 ) && ( this -> screen_.ScrollPosition < VFD_NBCHARS ) ) {		// Si il y as une vitesse...
		bitWrite ( _animated_char_,0 , this -> _animate );
		_string_ [ FAN_POS ] = _animated_char_;						// ... affichage de l'animation
	}
	this -> writeManyInt ( _string_, SPEED_POS, _speed_, 2, false );			// Affichage de la vitesse.
}

/** @brief
 *  @details
 */
void CLASS_SCREEN_::scrollScreen () {
	if ( ( rtc_.RTC_State ().State.bit.Configured ) && ( ( time_.seconds - this -> screen_.UpdateTime ) > 13 ) ) {
		if ( ( micros () - this -> screen_.ScrollTime ) > SCROLLINGSPEEDMS ) {								// Il est temps pour l'animation !
			if ( ( this -> screen_.ViewSpecial == true ) && ( this -> screen_.ScrollPosition < VFD_NBCHARS ) ) {			// On est sur la page de l'heure pour afficher la page d'information
				if ( this -> screen_.ping_pong ) friend_.scrollDisplayRight ();
				else friend_.scrollDisplayLeft ();
				this -> screen_.ScrollPosition ++;
				this -> screen_.ScrollTime = micros ();
				if ( this -> screen_.ScrollPosition == VFD_NBCHARS ) this -> _mustUpdateChars = true;
			}
			else if ( ( this -> screen_.ViewSpecial == false ) && ( this -> screen_.ScrollPosition > 0 ) ) {			// On est sur la page d'information pour afficher l'heure
				if ( this -> screen_.ping_pong ) friend_.scrollDisplayLeft ();
				else friend_.scrollDisplayRight ();
				this -> screen_.ScrollPosition --;
				this -> screen_.ScrollTime = micros ();
				if ( this -> screen_.ScrollPosition == ( VFD_NBCHARS - 1 ) ) this -> _mustUpdateChars = true;
			}
			else {
				this -> screen_.UpdateTime = time_.seconds;
				this -> screen_.ViewSpecial ^= 0x01;
				if ( this -> screen_.ScrollPosition == 0 ) this -> screen_.ping_pong ^= 0x01;
			}
		}
	}

	if ( this -> _mustUpdateChars ) this -> _mustUpdateChars = this -> updateChars ();
}

void CLASS_SCREEN_::changeTimeFormat () {
	if ( this -> clock_view_.BigClock ) {
		memmove ( this -> screen_.UpperLine, this -> _timeDayStr_Big, VFD_NBCHARS );
		memmove ( this -> screen_.LowerLine, this -> _timeDateStr_Big, VFD_NBCHARS );
	}
	else {
		memmove ( this -> screen_.UpperLine, this -> _timeStr_Little, VFD_NBCHARS );
		memmove ( this -> screen_.LowerLine, this -> _dateStr_Little, VFD_NBCHARS );
	}
}

/** @brief			Affichage d'un écran de chargement.
 *  @details			Petite animation affichant une barre de chargement.
 */
void CLASS_SCREEN_::loading () {
	friend_.clear ();										// Effacement de l'écran !
	friend_.setCursor ( 5, 0 );									// Positionnement du curseur sur la première ligne, caractère n° 5.
	friend_.print ( "Loading..." );									// Et affichage du texte.

	for ( uint8_t _pos = 0x40; _pos < ( 0x41 + VFD_NBCHARS ); _pos ++ ) {				// Pour le positionnement du curseur.			( Première ligne entre 0 et 40, seconde ligne entre 64 et 104 )
		for ( uint8_t _char = 0x10; _char < 0x15; _char ++ ) {					// Choix du caractère à afficher.			( Voir tableau pour les caractères en ROM... )
			friend_.writeOn ( _char, _pos );						// On ré-écrit le nouveau caractère à la bonne adresse.
			delay ( 12 );									// Une petite attente.					( 20 adresses x 5 caractères x 12 mS = 1,2 S pour l'animation ! )
		}
	}

	delay ( 200 );
	friend_.clear ();
}

/** @brief			Affichage d'un écran de chargement.
 *  @details			Animation affichant un pourcentage et une barre de chargement.
 * 				Reste affiché tant que la barre n'est pas à 100%
 * 				Retourne le pourcentage déjà affiché !
 *  @param _percent_		Le nombre de pourcent ou stopper la barre jusqu'au prochain appel dela fonction.
 */
uint8_t CLASS_SCREEN_::loading ( uint8_t _percent_ ) {
	static uint8_t _passed = 0;
	static uint8_t _pos;
	static uint8_t _char;

	if ( _passed == 0 ) {										// Début de routine d'affichage.
		friend_.clear ();									// Effacement de l'écran !
		friend_.setCursor ( 0 );								// Positionnement du curseur.
		friend_.print ( "   Loading...      %" );						// Et affichage du texte.
	}

	do {
		_pos = ( ( uint8_t ) ( _passed / 5 ) ) | 0x40;						// Positionnement du curseur. Toujours entre 0x40 et 0x53 !	( Première ligne entre 0 et 40, seconde ligne entre 64 et 104 )
		_char = ( _passed % 5 ) | 0x10;								// Le caractère. Toujours entre 0x10 et 0x14 !			( Voir tableau pour les caractères en ROM... )

		friend_.setCursor ( VFD_NBCHARS - 4, 0 );
		if ( _passed < 100 ) friend_.print ( _passed < 10 ? "  " : " " );
		friend_.print ( _passed, DEC );
		friend_.writeOn ( _char, _pos );							// Ecriture ( ou ré-écriture ) du nouveau caractère à la bonne adresse.
		_passed ++;										// Incrémentation du compteur.				( Représente la barre de progression, de 0 à 100% )
		delay ( 12 );										// Une petite attente.					( 20 adresses x 5 caractères x 12 mS = 1,2 S pour l'animation ! )

		if ( ( _passed % 100 ) == _percent_ ) {
			delay ( 250 );
			return _passed;
		}
	} while ( _passed < 101 );

	if ( _passed >= 100 ) {
		_passed = 0;
		delay ( 500 );
		friend_.clear ();
	}

	return 100;
}

/** @brief
 *  @details
 *  @param _brightness_
 */
void CLASS_SCREEN_::begin ( vfd_brightness_t _brightness_ ) {
	this -> _brightness = _brightness_;
	friend_.begin ( _brightness_ );

	this -> changeTimeFormat ();
	this -> _mustUpdateChars = this -> updateChars ();
}

/** @brief
 *  @details
 */
void CLASS_SCREEN_::loop () {
	if ( this -> state_.Display ) {
		if ( ! friend_.displayControl_.bit.Display ) friend_.display ();

		this -> updateTime ();							// Boucle de mise à jour de l'heure
		this -> updateDate ();							// Boucle de mise à jour de la date
		this -> updateInformations ();						// Boucle de mise à jour des informations

		this -> scrollScreen ();						// Boucle pour changer d'écran

		friend_.setCursor ( 0, 0 );
		friend_.write ( this -> screen_.UpperLine, VFD_MAXCHARS );		// Affichage de la première ligne
		friend_.setCursor ( 0, 1 );
		friend_.write ( this -> screen_.LowerLine, VFD_MAXCHARS );		// Affichage de la seconde ligne
	}

	if ( ( ! this -> state_.Display ) && ( friend_.displayControl_.bit.Display ) ) friend_.noDisplay ();
}

/** @brief
 *  @details
 *  @param _brightness_
 */
void CLASS_SCREEN_::setBrightness ( vfd_brightness_t _brightness_ ) {
	this -> _brightness = _brightness_;
	friend_.setBrightness ( _brightness_ );
}

/*uint8_t CLASS_SCREEN_::wakeup ( vfd_brightness_t _brightness_ ) {
	if ( ! friend_.displayControl_.bit.Display ) {
		friend_.setBrightness ( _brightness_ );					// Allumage de l'affichage à la fin de la fonction setBrightness ()
		this -> _screenDisplayUpdateTime = micros ();
	}
	else {
	}

	return friend_.displayFunction_.bit.Brightness;
}*/

/*void CLASS_SCREEN_::shutdown () {
	if ( friend_.displayControl_.bit.Display ) {
		if ( friend_.displayFunction_.bit.Brightness != VFD_Brightness25 ) {
		}
		else friend_.noDisplay ();
	}
}*/


CLASS_SCREEN_ screen_;
