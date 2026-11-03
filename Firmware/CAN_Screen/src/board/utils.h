#pragma once

#include "Arduino.h"

inline bool is_impair (uint32_t _number_ ) __attribute__ (( always_inline ));
inline bool is_pair ( uint32_t _number_ ) __attribute__ (( always_inline ));
inline bool is_negative ( size_t _number_ ) __attribute__ (( always_inline ));
inline bool is_positive ( size_t _number_ ) __attribute__ (( always_inline ));
inline bool is_between ( size_t _number_, size_t _min_, size_t _max_ ) __attribute__ (( always_inline ));
inline bool is_equal ( size_t _number_, size_t _compare_, size_t _delta_ ) __attribute__ (( always_inline ));
inline bool is_equal ( size_t _number_, size_t _compare_, size_t _deltaMin_, size_t _deltaMax_ ) __attribute__ (( always_inline ));
inline int16_t moy ( int16_t _value1_, int16_t _value2_ ) __attribute__ (( always_inline ));

inline bool is_impair ( uint32_t _number_ )								{ return ( _number_ & 0x1 ); }
inline bool is_pair ( uint32_t _number_ )								{ return !is_impair ( _number_ ); }
inline bool is_negative ( size_t _number_ )								{ return bitRead ( _number_, 31 ); }
inline bool is_positive ( size_t _number_ )								{ return !is_negative ( _number_ ); }
inline bool is_between ( size_t _number_, size_t _min_, size_t _max_ )					{ return ( _number_ > _min_ ? ( _number_ < _max_ ? true : false ) : false ); }
inline bool is_equal ( size_t _number_, size_t _compare_, size_t _delta_ )				{ return is_between ( _number_, _compare_ - _delta_, _compare_ + _delta_ ); }
inline bool is_equal ( size_t _number_, size_t _compare_, size_t _deltaMin_, size_t _deltaMax_ )	{ return is_between ( _number_, _compare_ - _deltaMin_, _compare_ + _deltaMax_ ); }
inline int16_t moy ( int16_t _value1_, int16_t _value2_ )						{ return ( int16_t ) ( ( _value1_ + _value2_ ) / 2 ); }

