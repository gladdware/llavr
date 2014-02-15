# LL-AVR

_Low-level core library for AVR peripherals_

## Features

Provides an easy-to-use set of APIs for manipulating the low-level features and
peripherals found on AVR platforms such as the ATmega series of microprocessors.

The goal is to provide wrappers for low-level things like Timers, UARTs/USARTs,
I2C, SPI, Interrupts, etc., while also providing APIs for common uses of these
low-level peripherals (e.g., Servos using hardware Timers for PWM).

Some of the code in this library comes from other common AVR libraries such as
Wiring and Arduino.

## Building

I'm currently using Eclipse with the AVR Plugin to develop and build the library.

Build tested under the following configuration:
 - Eclipse Kepler SR1
   * C/C++ Development Tools 8.2.1
   * AVR Eclipse Plugin 2.4.1
 - avr-gcc 4.7.2
 - avr-libc 1.8.0
 - binutils-avr 2.20.1

## License

Copyright 2014 Alex Gladd and others

LL-AVR is licensed under the GNU LGPLv3

![LGPLv3 Logo](https://www.gnu.org/graphics/lgplv3-88x31.png)

LL-AVR is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LL-AVR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with LL-AVR.  If not, see <http://www.gnu.org/licenses/>.
