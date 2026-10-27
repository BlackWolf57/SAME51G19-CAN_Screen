#pragma once

#include "Arduino.h"

inline bool is_impair (uint32_t _number_ ) __attribute__ (( always_inline ));
inline bool is_pair ( uint32_t _number_ ) __attribute__ (( always_inline ));
inline bool is_negative ( size_t _number_ ) __attribute__ (( always_inline ));
inline bool is_positive ( size_t _number_ ) __attribute__ (( always_inline ));
inline int16_t moy ( int16_t _value1_, int16_t _value2_ ) __attribute__ (( always_inline ));

inline bool is_impair ( uint32_t _number_ )							{ return ( _number_ & 0x1 ); }
inline bool is_pair ( uint32_t _number_ )							{ return !is_impair ( _number_ ); }
inline bool is_negative ( size_t _number_ )							{ return bitRead ( _number_, 31 ); }
inline bool is_positive ( size_t _number_ )							{ return !is_negative ( _number_ ); }
inline int16_t moy ( int16_t _value1_, int16_t _value2_ )					{ return ( int16_t ) ( ( _value1_ + _value2_ ) / 2 ); }

