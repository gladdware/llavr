/*
 * This file is part of LL-AVR
 *
 * LL-AVR is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LL-AVR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with LL-AVR.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LLAVR_COMMON_H_
#define LLAVR_COMMON_H_

// common includes
#include <stdlib.h>
#include <string.h>
#include <math.h>

// common avr includes
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// bool value if we're not c++
#ifndef __cplusplus
typedef uint8_t bool
#define true    ((bool)1)
#define false   ((bool)0)
#endif

/*
 * Many of these defines were lifted from the Arduino.h header
 * See https://github.com/arduino/Arduino
 */

#ifdef __cplusplus
extern "C" {
#endif

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

#define PI          3.1415926535897932384626433832795
#define HALF_PI     1.5707963267948966192313216916398
#define TWO_PI      6.283185307179586476925286766559
#define DEG_TO_RAD  0.017453292519943295769236907684886
#define RAD_TO_DEG  57.295779513082320876798154814105

// common math
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

// bit/byte manipulations
#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

// convert to bit value
#define bit(num)    _BV(num)

// clear/set bit in register
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// signify "no register set"
#define NOREG ((volatile uint8_t*)0)

// "word" value (16 bits)
typedef unsigned int word;

// "byte" value (8 bits)
typedef uint8_t byte;

#ifdef __cplusplus
} /* extern C */
#endif

#endif /* LLAVR_COMMON_H_ */
