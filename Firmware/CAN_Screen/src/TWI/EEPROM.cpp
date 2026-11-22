#include "./EEPROM.h"
#include "../board/pins.h"
#include "../../configuration.h"

TwoWire internal_I2C_ ( &sercom1, I2C_SDA, I2C_SCL );					// Initialisation du port I²C

/** @brief
 *  @details
 *  @param _address_		Adresse mémoire à lire.
 */
/*byte CLASS_I2CEEPROM_::ReadByte ( size_t _address_ ) {
	byte _EEPROM_data = MASK_8BITS;

	#if DISABLED(NO_EEPROM)
		internal_I2C_.beginTransmission ( ( uint8_t ) EEPROM_ADDR );
		internal_I2C_.write ( ( uint8_t ) ( _address_ & MASK_16BHIGH ) );
		internal_I2C_.write ( ( uint8_t ) ( _address_ & MASK_16BLOW ) );

		if ( internal_I2C_.endTransmission ( false ) == 0 ) {
			internal_I2C_.requestFrom ( ( uint8_t ) EEPROM_ADDR, ( uint8_t ) 1 );
			if ( internal_I2C_.available () ) _EEPROM_data = internal_I2C_.read ();
		}
	#endif

	return _EEPROM_data;
}*/

/** @brief
 *  @details
 *  @param _address_		Adresse mémoire à écrire.
 *  @param _data_		Donnée sur 8 bits à sauver.
 */
/*uint8_t CLASS_I2CEEPROM_::WriteByte ( size_t _address_, uint8_t _data_ ) {
	uint8_t _error = 0;

	#if DISABLED(NO_EEPROM)
		internal_I2C_.beginTransmission ( ( uint8_t ) EEPROM_ADDR );
		internal_I2C_.write ( ( uint8_t ) ( _address_ & MASK_16BHIGH ) );
		internal_I2C_.write ( ( uint8_t ) ( _address_ & MASK_16BLOW ) );
		internal_I2C_.write ( ( uint8_t ) _data_ );
		_error = internal_I2C_.endTransmission ();

		delay ( 5 );
	#endif

	return _error;
}*/

/** @brief			Lecture d'une donnée en mémoire.
 *  @details			Lecture d'une donnée de # bits en mémoire.
 *  @param _address_		Adresse mémoire à lire.
 *  @param _size_		Taille à lire.
 */
/*size_t CLASS_I2CEEPROM_::Read ( size_t _address_, size_t _size_ = MASK_8BITS ) {
	size_t _value = 0;

	if ( _size_ <= MASK_8BITS ) {									// Lecture d'un entier sur 8 bits.
		_value = this -> ReadByte ( _address_ );
	}
	else if ( ( _size_ > MASK_8BITS ) && ( _size_ <= MASK_16BITS ) ) {				// Lecture d'un entier sur 16 bits.
		uint8_t _high_byte = this -> ReadByte ( _address_ + 1 );
		uint8_t _low_byte = this -> ReadByte ( _address_ );

		_value = ( ( size_t ) _high_byte << 8 ) | _low_byte;
	}
	else if ( _size_ == MASK_LONG ) {								// Lecture d'un entier long, converti en entier sur 16 bits. ( Durée dans le programme ).
		 uint8_t _coeff = this -*> ReadByte ( _address_ + 1 );
		 uint8_t _readval = this -> ReadByte ( _address_ );

		 if ( _coeff == Time_Micros ) _value = _readval * 100000;				// Durée ( micros () ) de l'ordre du 1/10 de secondes.
		 else if ( _coeff == Time_Seconds ) _value = _readval * CONV_SECONDSTOMICROS;		// Durée ( micros () ) de l'ordre de la seconde.
		 else if ( _coeff == Time_Minutes ) _value = _readval * CONV_MINUTESTOMICROS;		// Durée ( micros () ) de l'ordre de la minute.
		 else if ( _coeff == Time_Hours ) _value = _readval * CONV_HOURSTOMICROS;		// Durée ( micros () ) de l'ordre de l'heure.
	}

	return _value;
}*/

/** @brief			Ecriture d'une donnée en mémoire.
 *  @details			Ecriture d'une donnée 16 bits en mémoire. Remplacement de l'ancienne valeur.
 *  @param _address_		Adresse mémoire à écrire.
 *  @param _data_		Donnée sur 16 bits à sauver.
 */
/*uint8_t CLASS_I2CEEPROM_::Write ( size_t _address_, uint16_t _data_ ) {
	uint8_t _error = 0;
	uint8_t _high_byte = ( _data_ & MASK_16BHIGH ) >> 8;
	uint8_t _low_byte = _data_ & MASK_16BLOW;

	_error = this -> WriteByte ( _address_, _low_byte );
	_error |= this -> WriteByte ( _address_ + 1, _high_byte );

	return _error;
}*/

/** @brief			Mise à jour d'une donnée en mémoire.
 *  @details			Vérification puis modification d'une donnée 8 bits en mémoire.
 *  @param _address_		Adresse mémoire pour la lecture/écriture.
 *  @param _data_		Donnée sur 8 bits à modifier.
 */
/*uint8_t CLASS_I2CEEPROM_::Update ( size_t _address_, uint8_t _data_ ) {
	uint8_t _error = 0;
	byte _EEPROM_data = this -> ReadByte ( _address_ );

	if ( _data_ != _EEPROM_data ) _error = this -> WriteByte ( _address_, _data_ );

	return _error;
}*/

/** @brief			Mise à jour d'une donnée en mémoire.
 *  @details			Vérification puis modification d'une donnée 16 bits en mémoire.
 *  @param _address_		Adresse mémoire pour la lecture/écriture.
 *  @param _data_		Donnée sur 16 bits à modifier.
 */
/*uint8_t CLASS_I2CEEPROM_::Update ( size_t _address_, uint16_t _data_ ) {
	uint8_t _error = 0;
	uint16_t _EEPROM_data = this -> Read ( _address_, MASK_16BITS );

	if ( _data_ != _EEPROM_data ) _error = this -> Write ( _address_, _data_ );

	return _error;
}*/

/** @brief			Écrit un bloc de données de taille quelconque en gérant les pages de 32 octets
 *  @param _address_		Adresse mémoire de départ
 *  @param _buffer_		Pointeur vers les données à sauvegarder
 *  @param _length_		Nombre d'octets à écrire
 */
uint8_t CLASS_I2CEEPROM_::WriteBlock ( size_t _address_, const uint8_t* _buffer_, size_t _length_ ) {
	uint8_t _error = 0;
	size_t _bytesWritten = 0;

	#if DISABLED(NO_EEPROM)
		while ( _bytesWritten < _length_ ) {
			// 1. Calculer combien d'octets on peut écrire dans la page actuelle
			size_t _pageOffset = _address_ % EEPROM_PAGE_SIZE;			// Position dans la page de 32 octets
			size_t _maxPageBytes = EEPROM_PAGE_SIZE - _pageOffset;			// Espace restant dans la page
			size_t _bytesToCopy = _length_ - _bytesWritten;

			// Si ce qu'il reste à écrire dépasse la page, on limite à l'espace restant
			if ( _bytesToCopy > _maxPageBytes ) _bytesToCopy = _maxPageBytes;

			// 2. Lancer la transaction I2C pour cette page
			internal_I2C_.beginTransmission ( ( uint8_t ) EEPROM_ADDR );
			internal_I2C_.write ( ( uint8_t ) ( ( _address_ & MASK_16BHIGH ) >> 8 ) );
			internal_I2C_.write ( ( uint8_t ) ( _address_ & MASK_16BLOW ) );

			// Écrire le paquet d'octets autorisé dans la page
			for ( size_t _offset = 0; _offset < _bytesToCopy; _offset ++ ) {
				internal_I2C_.write ( _buffer_ [ _bytesWritten + _offset ] );
			}

			_error |= internal_I2C_.endTransmission ();

			// 3. Attendre la fin de l'écriture matérielle de la page (5ms)
			delay ( 5 );

			// 4. Mettre à jour les index pour la page suivante
			_bytesWritten += _bytesToCopy;
			_address_     += _bytesToCopy;
		}
	#endif

	return _error;
}

/** @brief			Lit un bloc de données de taille quelconque de manière séquentielle
 *  @details			La lecture séquentielle n'a pas de limite de page, la puce incrémente l'adresse tout seule !
 *  @param _address_		Adresse mémoire de départ
 *  @param _buffer_		Pointeur vers les données à sauvegarder
 *  @param _length_		Nombre d'octets à écrire
 */
uint8_t CLASS_I2CEEPROM_::ReadBlock ( size_t _address_, uint8_t* _buffer_, size_t _length_ ) {
	uint8_t _error = 0;

	#if DISABLED(NO_EEPROM)
		internal_I2C_.beginTransmission ( ( uint8_t ) EEPROM_ADDR );
		internal_I2C_.write ( ( uint8_t ) ( ( _address_ & MASK_16BHIGH ) >> 8 ) );
		internal_I2C_.write ( ( uint8_t ) ( _address_ & MASK_16BLOW ) );

		if ( internal_I2C_.endTransmission ( false ) == 0 ) {
			internal_I2C_.requestFrom ( ( uint8_t ) EEPROM_ADDR, ( uint8_t ) _length_ );

			for ( size_t _offset = 0; _offset < _length_; _offset ++ ) {
				if ( internal_I2C_.available () ) _buffer_ [ _offset ] = internal_I2C_.read ();
			}
		}
		else _error = 1;
	#endif

	return _error;
}

CLASS_I2CEEPROM_ eeprom_;
