/*
 * This file is part of LL-AVR
 * Copyright (C) 2014 Alex Gladd
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

#ifndef LLAVR_HARDWARE_TIMER_H_
#define LLAVR_HARDWARE_TIMER_H_

#include "llavr-common.h"

typedef enum {
    TIMER_PRESCALE_NONE,
    TIMER_PRESCALE_8,
    TIMER_PRESCALE_64,
    TIMER_PRESCALE_256,
    TIMER_PRESCALE_1024
} TimerPrescaler;

typedef enum {
    TIMER_OCR_A = bit(0),
    TIMER_OCR_B = bit(1),
    TIMER_OCR_C = bit(2),
} TimerCompareChannel;

class HardwareTimer {
public:
    /**
     * @brief Constructor
     *
     * NOTE: Users should never need to explicitly call this constructor.
     *   Instead, use one of the pre-defined TimerX globals, which vary in
     *   availability by MCU.
     */
    HardwareTimer(bool is16Bit, uint8_t numCompareChannels,
            volatile uint8_t *tccrA, volatile uint8_t *tccrB, volatile uint8_t *tccrC,
            volatile uint8_t *tcnth, volatile uint8_t *tcntl,
            volatile uint8_t *ocrAh, volatile uint8_t *ocrAl,
            volatile uint8_t *ocrBh, volatile uint8_t *ocrBl,
            volatile uint8_t *ocrCh, volatile uint8_t *ocrCl,
            volatile uint8_t *icrh, volatile uint8_t *icrl,
            volatile uint8_t *timsk, volatile uint8_t *tifr);

    /**
     * @brief Set the prescale value to use for this timer (see enum
     *   TimerPrescaler)
     *
     * NOTE: The new prescaler value does not take effect until the next call
     *   to one of the set*Mode() functions.
     *
     * @param prescale The prescale setting to use
     */
    void setPrescaler(TimerPrescaler prescale);

    /**
     * @brief Immediately enable "normal mode" on this timer
     *
     * NOTE: The timer count is reset to zero and any existing output compare
     *   settings are cleared.
     */
    void setNormalMode();

    // TODO
//    void setCtcMode();

    /**
     * @brief Immediately enable "fast pwm mode" on this timer, the operation of
     *   which is determined by the given parameters
     *
     * NOTE: If the underlying timer is an 8-bit timer the TOP value is always
     *   set to 0xFF, and the given value is ignored.
     *
     * NOTE: The timer count is reset to zero and any existing output compare
     *   settings are cleared.
     *
     * @param topValue The TOP value to use for fast pwm on this timer (defaults
     *   to 0xFFFF)
     */
    void setFastPwmMode(uint16_t topValue = 0xFFFF);

    // TODO
//    void setOverflowInterrupt();

    // TODO
//    void setCompareMatchInterrupt();

    /**
     * @brief Set the output compare register (OCRnx) of the given channels to
     *   the given value
     *
     * NOTE: If the underlying timer is an 8-bit timer, only the least-
     *   significant 8 bits of the given value are used to set the register.
     *
     * NOTE: Not all timers have all channels available; attempting to set a
     *   compare value on an invalid channel will fail silently.
     *
     * NOTE: User is responsible for setting the correct ports to outputs if
     *   expecting the output compare unit to drive the corresponding ports
     *
     * @param channels A bitmask defining which channels to set the value on
     *   (see enum TimerCompareChannel)
     * @param value The compare value to set
     * @param inverting False to clear the corresponding port on compare match,
     *   true to set the corresponding port on compare match (defaults to false)
     */
    void setCompareValue(uint8_t channels, uint16_t value, bool inverting = false);

    /**
     * @brief Convenience shortcut for setting the compare value for channel A
     */
    void setCompareValueA(uint16_t value, bool inverting = false) {
        setCompareValue((uint8_t)TIMER_OCR_A, value, inverting);
    }

    /**
     * @brief Convenience shortcut for setting the compare value for channel B
     */
    void setCompareValueB(uint16_t value, bool inverting = false) {
        setCompareValue((uint8_t)TIMER_OCR_B, value, inverting);
    }

    /**
     * @brief Convenience shortcut for setting the compare value for channel C
     */
    void setCompareValueC(uint16_t value, bool inverting = false) {
        setCompareValue((uint8_t)TIMER_OCR_C, value, inverting);
    }

protected:
    /**
     * @brief Overwrite existing timer control registers with the given values
     *
     * NOTE: Interrupts are disabled while resetting control values.
     *
     * NOTE: If the underlying timer is an 8-bit timer, only the A and B values
     *   are used.
     *
     * @param controlA The value to set for TCCRnA
     * @param controlB The value to set for TCCRnB
     * @param controlC The value to set for TCCRnC
     */
    void resetTimerControl(uint8_t controlA, uint8_t controlB, uint8_t controlC);

    TimerPrescaler prescale;

private:
    bool is16Bit;               ///< is the timer 16-bit
    uint8_t numOcrChannels;     ///< number of output compare channels
    volatile uint8_t *tccrA, *tccrB, *tccrC;///< timer control registers
    volatile uint8_t *tcnth, *tcntl;        ///< timer counter registers
    volatile uint8_t *ocrAh, *ocrAl;        ///< timer output compare channel A
    volatile uint8_t *ocrBh, *ocrBl;        ///< timer output compare channel B
    volatile uint8_t *ocrCh, *ocrCl;        ///< timer output compare channel C
    volatile uint8_t *icrh, *icrl;          ///< timer input capture registers
    volatile uint8_t *timsk, *tifr;         ///< timer interrupt mask/flags
};

/*
 * The following static timer object will be allocated based on the targeted
 * MCU.
 */

#if defined(TCCR0A)
extern HardwareTimer Timer0;
#endif

#if defined(TCCR1A)
extern HardwareTimer Timer1;
#endif

#if defined(TCCR2A)
extern HardwareTimer Timer2;
#endif

#if defined(TCCR3A)
extern HardwareTimer Timer3;
#endif

#if defined(TCCR4A)
extern HardwareTimer Timer4;
#endif

#if defined(TCCR5A)
extern HardwareTimer Timer5;
#endif

#endif /* LLAVR_HARDWARE_TIMER_H_ */
