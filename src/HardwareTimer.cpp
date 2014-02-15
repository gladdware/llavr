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

#include "HardwareTimer.h"

#define __CS_PRESCALE_NONE ((uint8_t)(bit(CS00)))
#define __CS_PRESCALE_8    ((uint8_t)(bit(CS01)))
#define __CS_PRESCALE_64   ((uint8_t)(bit(CS00) | bit(CS01)))
#define __CS_PRESCALE_256  ((uint8_t)(bit(CS02)))
#define __CS_PRESCALE_1024 ((uint8_t)(bit(CS00) | bit(CS02)))

/**
 * @brief Set the clock select bits into the given value based on the given
 *   prescale value
 */
static inline void __setClockSelect(uint8_t *value, TimerPrescaler ps) {
    switch(ps) {
    case TIMER_PRESCALE_NONE:
        *value |= __CS_PRESCALE_NONE;
        break;

    case TIMER_PRESCALE_8:
        *value |= __CS_PRESCALE_8;
        break;

    case TIMER_PRESCALE_64:
        *value |= __CS_PRESCALE_64;
        break;

    case TIMER_PRESCALE_256:
        *value |= __CS_PRESCALE_256;
        break;

    case TIMER_PRESCALE_1024:
        *value |= __CS_PRESCALE_1024;
        break;
    }
}

/**
 * @brief Write to a 16-bit register
 */
static inline void __setWideReg(
        volatile uint8_t *regH, volatile uint8_t *regL, uint16_t value) {
    if(regH != NOREG) {
        *regH = highByte(value);
    }

    *regL = lowByte(value);
}

HardwareTimer::HardwareTimer(
        bool is16Bit, uint8_t numCompareChannels,
        volatile uint8_t *tccrA, volatile uint8_t *tccrB, volatile uint8_t *tccrC,
        volatile uint8_t *tcnth, volatile uint8_t *tcntl,
        volatile uint8_t *ocrAh, volatile uint8_t *ocrAl,
        volatile uint8_t *ocrBh, volatile uint8_t *ocrBl,
        volatile uint8_t *ocrCh, volatile uint8_t *ocrCl,
        volatile uint8_t *icrh, volatile uint8_t *icrl,
        volatile uint8_t *timsk, volatile uint8_t *tifr)
    : is16Bit(is16Bit),
      numOcrChannels(numCompareChannels),
      tccrA(tccrA), tccrB(tccrB), tccrC(tccrC),
      tcnth(tcnth), tcntl(tcntl),
      ocrAh(ocrAh), ocrAl(ocrAl),
      ocrBh(ocrBh), ocrBl(ocrBl),
      ocrCh(ocrCh), ocrCl(ocrCl),
      icrh(icrh), icrl(icrl),
      timsk(timsk), tifr(tifr),
      prescale(TIMER_PRESCALE_NONE) {
    // nop
}

void HardwareTimer::setPrescaler(TimerPrescaler prescale) {
    if(prescale != this->prescale) {
        // set the new value
        this->prescale = prescale;
    }
    // else nothing to do
}

void HardwareTimer::setNormalMode() {
    uint8_t ctrlA = 0;
    uint8_t ctrlB = 0;
    uint8_t ctrlC = 0;

    // normal mode: WGM0..2 set to all zeros
    // just need to set clock select

    __setClockSelect(&ctrlB, prescale);

    // set the control registers
    resetTimerControl(ctrlA, ctrlB, ctrlC);

    // reset timer counter
    __setWideReg(tcnth, tcntl, 0);
}

void HardwareTimer::setFastPwmMode(uint16_t topValue) {
    uint8_t ctrlA = 0;
    uint8_t ctrlB = 0;
    uint8_t ctrlC = 0;

    /*
     * For 16-bit timers, use mode 14 (TOP == ICRn)
     * For 8-bit timers, use mode 3 (TOP == 0xFF)
     */

    if(is16Bit) {
        bitSet(ctrlA, WGM11);
        bitSet(ctrlB, WGM12);
        bitSet(ctrlB, WGM13);

        // set icrn to top value
        __setWideReg(icrh, icrl, topValue);
    } else {
        bitSet(ctrlA, WGM00);
        bitSet(ctrlA, WGM01);
    }

    __setClockSelect(&ctrlB, prescale);

    // set the control registers
    resetTimerControl(ctrlA, ctrlB, ctrlC);

    // reset timer counter
    __setWideReg(tcnth, tcntl, 0);
}

void HardwareTimer::setCompareValue(uint8_t channels, uint16_t value, bool inverting) {
    uint8_t curTccrA, curTccrB, curTccrC = 0;
    uint8_t comVal = 0;

    // set compare setting
    bitSet(comVal, 1);
    if(inverting) {
        bitSet(comVal, 0);
    }

    // get current tccrx
    curTccrA = *tccrA;
    curTccrB = *tccrB;

    if(is16Bit) {
        curTccrC = *tccrC;
    }

    // check for each channel

    if(channels & TIMER_OCR_A) {
        curTccrA |= (comVal << COM0A0);
        __setWideReg(ocrAh, ocrAl, value);
    }

    if(channels & TIMER_OCR_B) {
        curTccrA |= (comVal << COM0B0);
        __setWideReg(ocrBh, ocrBl, value);
    }

    if(is16Bit && (channels & TIMER_OCR_C)) {
        curTccrA |= (comVal << COM1C0);
        __setWideReg(ocrCh, ocrCl, value);
    }

    // reset control
    resetTimerControl(curTccrA, curTccrB, curTccrC);
}

void HardwareTimer::resetTimerControl(uint8_t controlA, uint8_t controlB, uint8_t controlC) {
    uint8_t saveSreg;

    // save the current status register
    saveSreg = SREG;

    // disable interrupts
    cli();

    // set register A
    *tccrA = controlA;

    // set register C if 16-bit
    if(is16Bit) {
        *tccrC = controlC;
    }

    // set register B last since it controls clock generation
    *tccrB = controlB;

    // re-enable saved status
    SREG = saveSreg;
}

// static timers

#if defined(TCCR0A)
HardwareTimer Timer0(
        false, 2,
        &TCCR0A, &TCCR0B, NOREG,
        NOREG, &TCNT0,
        NOREG, &OCR0A,
        NOREG, &OCR0B,
        NOREG, NOREG,
        NOREG, NOREG,
        &TIMSK0, &TIFR0);
#endif

#if defined(TCCR1A)
HardwareTimer Timer1(
        true, 3,
        &TCCR1A, &TCCR1B, &TCCR1C,
        &TCNT1H, &TCNT1L,
        &OCR1AH, &OCR1AL,
        &OCR1BH, &OCR1BL,
        &OCR1CH, &OCR1CL,
        &ICR1H, &ICR1L,
        &TIMSK1, &TIFR1);
#endif

#if defined(TCCR2A)
HardwareTimer Timer2(
        false, 2,
        &TCCR2A, &TCCR2B, NOREG,
        NOREG, &TCNT2,
        NOREG, &OCR2A,
        NOREG, &OCR2B,
        NOREG, NOREG,
        NOREG, NOREG,
        &TIMSK2, &TIFR2);
#endif

#if defined(TCCR3A)
HardwareTimer Timer3(
        true, 3,
        &TCCR3A, &TCCR3B, &TCCR3C,
        &TCNT3H, &TCNT3L,
        &OCR3AH, &OCR3AL,
        &OCR3BH, &OCR3BL,
        &OCR3CH, &OCR3CL,
        &ICR3H, &ICR3L,
        &TIMSK3, &TIFR3);
#endif

#if defined(TCCR4A)
HardwareTimer Timer4(
        true, 3,
        &TCCR4A, &TCCR4B, &TCCR4C,
        &TCNT4H, &TCNT4L,
        &OCR4AH, &OCR4AL,
        &OCR4BH, &OCR4BL,
        &OCR4CH, &OCR4CL,
        &ICR4H, &ICR4L,
        &TIMSK4, &TIFR4);
#endif

#if defined(TCCR5A)
HardwareTimer Timer5(
        true, 3,
        &TCCR5A, &TCCR5B, &TCCR5C,
        &TCNT5H, &TCNT5L,
        &OCR5AH, &OCR5AL,
        &OCR5BH, &OCR5BL,
        &OCR5CH, &OCR5CL,
        &ICR5H, &ICR5L,
        &TIMSK5, &TIFR5);
#endif
