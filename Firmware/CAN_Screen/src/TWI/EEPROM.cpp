#include "./EEPROM.h"
#include "../board/pins.h"
#include "../../configuration.h"

TwoWire internal_I2C_ ( &sercom1, I2C_SDA, I2C_SCL );					// Initialisation du port I²C

/** @brief
 *  @details
 *  @param _address_		Adresse mémoire à lire.
 */
byte CLASS_I2CEEPROM_::ReadByte ( size_t _address_ ) {
	byte _EEPROM_data = MASK_8BITS;
	uint8_t _error = 0;
	uint8_t _address_inPage = _address_ % MASK_8BITS;						// Pour toujours avoir une adresse mémoire de 8 bits.
	uint8_t _EEPROM_Page_ADDR = EEPROM_ADDR + ( _address_ / MASK_8BITS );				// Quand l'adresse mémoire dépasse 8 bits, on change de page dans l'EEPROM => On voit une adresse par page sur le bus I2C.

	#if DISABLED(NO_EEPROM)
		//if ( ( ( ( INTERNAL_TWI -> TWI_MMR & 0x7F0000 ) >> 16 ) != _EEPROM_Page_ADDR ) || ( INTERNAL_TWI -> TWI_SR != 0xF005 ) ) _error = internal_I2C_.endTransmission ();
		if ( ( ( SERCOM1 -> I2CM.ADDR.bit.ADDR ) >> 1 ) != _EEPROM_Page_ADDR ) _error = internal_I2C_.endTransmission ();

		internal_I2C_.beginTransmission ( ( uint8_t ) _EEPROM_Page_ADDR );
		internal_I2C_.write ( ( uint8_t ) _address_inPage );
		_error = internal_I2C_.endTransmission ();

		internal_I2C_.requestFrom ( ( uint8_t ) _EEPROM_Page_ADDR, ( uint8_t ) 1 );

		if ( internal_I2C_.available () ) _EEPROM_data = internal_I2C_.read ();

		_error = internal_I2C_.endTransmission ();
	#endif

	return _EEPROM_data;
}

/** @brief
 *  @details
 *  @param _address_		Adresse mémoire à écrire.
 *  @param _data_		Donnée sur 8 bits à sauver.
 */
uint8_t CLASS_I2CEEPROM_::WriteByte ( size_t _address_, uint8_t _data_ ) {
	uint8_t _error = 0;
	uint8_t _address_inPage = _address_ % MASK_8BITS;
	uint8_t _EEPROM_Page_ADDR = EEPROM_ADDR + ( _address_ / MASK_8BITS );

	#if DISABLED(NO_EEPROM)
		//if ( ( ( ( INTERNAL_TWI -> TWI_MMR & 0x7F0000 ) >> 16 ) != _EEPROM_Page_ADDR ) || ( INTERNAL_TWI -> TWI_SR != 0xF005 ) ) _error = internal_I2C_.endTransmission ();
		if ( ( ( SERCOM1 -> I2CM.ADDR.bit.ADDR ) >> 1 ) != _EEPROM_Page_ADDR ) _error = internal_I2C_.endTransmission ();

		internal_I2C_.beginTransmission ( ( uint8_t ) _EEPROM_Page_ADDR );
		internal_I2C_.write ( ( uint8_t ) _address_inPage );
		internal_I2C_.write ( ( uint8_t ) _data_ );
		_error = internal_I2C_.endTransmission ();

		delay ( 15 );
	#endif

	return _error;
}

/** @brief
 *  @details
 *  @param _address_		Adresse mémoire à lire.
 */
byte CLASS_I2CEEPROM_::Read ( size_t _address_ ) {
	byte _value = MASK_8BITS;

	_value = this -> ReadByte ( _address_ );
	return _value;
}

/** @brief			Lecture d'une donnée en mémoire.
 *  @details			Lecture d'une donnée de # bits en mémoire.
 *  @param _address_		Adresse mémoire à lire.
 *  @param _size_		Taille à lire.
 */
size_t CLASS_I2CEEPROM_::Read ( size_t _address_, size_t _size_ = MASK_8BITS ) {
	size_t _value = _size_;

	if ( _size_ <= MASK_8BITS ) {									// Lecture d'un entier sur 8 bits.
		_value = this -> ReadByte ( _address_ );
	}
	else if ( ( _size_ > MASK_8BITS ) && ( _size_ <= MASK_16BITS ) ) {				// Lecture d'un entier sur 16 bits.
		uint8_t _high_byte = this -> ReadByte ( _address_ + 1 );
		uint8_t _low_byte = this -> ReadByte ( _address_ );

		_value = _high_byte << 8 | _low_byte;
	}
	/*else if ( _size_ == MASK_LONG ) {								// Lecture d'un entier long, converti en entier sur 16 bits. ( Durée dans le programme ).
	 u int8_t _coeff = this -*> ReadByte ( _address_ + 1 );
	 uint8_t _readval = this -> ReadByte ( _address_ );

	 if ( _coeff == Time_Micros ) _value = _readval * 100000;				// Durée ( micros () ) de l'ordre du 1/10 de secondes.
	 else if ( _coeff == Time_Seconds ) _value = _readval * CONV_SECONDSTOMICROS;		// Durée ( micros () ) de l'ordre de la seconde.
	 else if ( _coeff == Time_Minutes ) _value = _readval * CONV_MINUTESTOMICROS;		// Durée ( micros () ) de l'ordre de la minute.
	 else if ( _coeff == Time_Hours ) _value = _readval * CONV_HOURSTOMICROS;		// Durée ( micros () ) de l'ordre de l'heure.
}*/

	return _value;
}

/** @brief			Ecriture d'une donnée en mémoire.
 *  @details			Ecriture d'une donnée 16 bits en mémoire. Remplacement de l'ancienne valeur.
 *  @param _address_		Adresse mémoire à écrire.
 *  @param _data_		Donnée sur 16 bits à sauver.
 */
uint8_t CLASS_I2CEEPROM_::Write ( size_t _address_, uint16_t _data_ ) {
	uint8_t _error = 0;
	uint8_t _high_byte = ( _data_ & MASK_16BHIGH ) >> 8;
	uint8_t _low_byte = _data_ & MASK_16BLOW;

	_error = this -> WriteByte ( _address_, _low_byte );
	_error |= this -> WriteByte ( _address_ + 1, _high_byte );

	return _error;
}

/** @brief			Mise à jour d'une donnée en mémoire.
 *  @details			Vérification puis modification d'une donnée 8 bits en mémoire.
 *  @param _address_		Adresse mémoire pour la lecture/écriture.
 *  @param _data_		Donnée sur 8 bits à modifier.
 */
uint8_t CLASS_I2CEEPROM_::Update ( size_t _address_, uint8_t _data_ ) {
	uint8_t _error = 0;
	byte _EEPROM_data = this -> ReadByte ( _address_ );

	if ( _data_ != _EEPROM_data ) _error = this -> WriteByte ( _address_, _data_ );

	return _error;
}

/** @brief			Mise à jour d'une donnée en mémoire.
 *  @details			Vérification puis modification d'une donnée 16 bits en mémoire.
 *  @param _address_		Adresse mémoire pour la lecture/écriture.
 *  @param _data_		Donnée sur 16 bits à modifier.
 */
uint8_t CLASS_I2CEEPROM_::Update ( size_t _address_, uint16_t _data_ ) {
	uint8_t _error = 0;
	uint16_t _EEPROM_data = this -> Read ( _address_, MASK_16BITS );

	if ( _data_ != _EEPROM_data ) _error = this -> Write ( _address_, _data_ );

	return _error;
}

CLASS_I2CEEPROM_ eeprom_;
