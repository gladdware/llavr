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

#include "HardwareServo.h"

// TODO remove
#include "HardwareSerial.h"

/**
 * @brief Calculate the number of timer ticks in per one microsecond based on
 *   the given value, which sould be the number of timer ticks per 20ms
 */
static inline float __ticksPerMicroS(uint16_t ticksIn20ms) {
    return (float)(ticksIn20ms) / 20000.0F;
}

/**
 * @brief Setup the given timer for use as a PWM generator for servo timing
 *
 * NOTE: Assumes maximum F_CPU of 20000000UL (20MHz)
 *
 * NOTE: Servo timing is setup using hardware timers in "fast pwm" mode, with a
 *   frequency as close as possible to 50Hz (i.e., generates pulses every 20ms).
 *
 * NOTE: Overrides any previous setting for the given timer, *unless* its
 *   current settings match those that would be set here.
 *
 * @param t The hardware timer to set up for servo use
 *
 * @return the number of timer ticks per 20 milliseconds
 */
static uint16_t __setupServoTimer(HardwareTimer *t) {
    TimerPrescaler prescale = TIMER_PRESCALE_8;
    uint32_t tmpTop;
    uint16_t resultTicks;

    // sanity check
    if(t == NULL_HWTIMER) {
        return 0;
    }

    /*
     * We try to get a top value that is <= 0xFFFF, but high enough that we
     * still have decent resolution (e.g., >= 1 tick per microsecond)
     */

    tmpTop = (F_CPU / (50UL * 8UL));

    if(tmpTop > 0xFFFFUL) {
        // increase the prescaler
        prescale = TIMER_PRESCALE_64;
        tmpTop = (F_CPU / (50UL * 64UL));

    } else if(tmpTop < 20000UL) {
        // decrease prescaler
        prescale = TIMER_PRESCALE_NONE;
        tmpTop = (F_CPU / (50UL));
    }

    // cast down to 16-bit
    resultTicks = (uint16_t)(tmpTop & 0xFFFFUL);

    // check if we need to setup the timer
    if(t->getMode() != TIMER_MODE_FASTPWM ||
            t->getPrescale() != prescale ||
            t->getTop() != (resultTicks - 1)) {
        // setup the timer
        t->setPrescaler(prescale);
        t->setFastPwmMode(resultTicks - 1);
    }

    return resultTicks;
}

/**
 * @brief Sets the given pin to be an output
 */
static void __setPinModeOutput(ServoPin pin) {

    /*
     * TODO make sure these pin mapping work for devices other than ATmega2560
     */

    switch(pin) {
#if defined(OCR1A)
    case OC1A:
        sbi(DDRB, PB5);
        break;
#endif
#if defined(OCR1B)
    case OC1B:
        sbi(DDRB, PB6);
        break;
#endif
#if defined(OCR1C)
    case OC1C:
        sbi(DDRB, PB7);
        break;
#endif
#if defined(OCR3A)
    case OC3A:
        sbi(DDRE, PE3);
        break;
#endif
#if defined(OCR3B)
    case OC3B:
        sbi(DDRE, PE4);
        break;
#endif
#if defined(OCR3C)
    case OC3C:
        sbi(DDRE, PE5);
        break;
#endif
#if defined(OCR4A)
    case OC4A:
        sbi(DDRH, PH3);
        break;
#endif
#if defined(OCR4B)
    case OC4B:
        sbi(DDRH, PH4);
        break;
#endif
#if defined(OCR4C)
    case OC4C:
        sbi(DDRH, PH5);
        break;
#endif
#if defined(OCR5A)
    case OC5A:
        sbi(DDRL, PL3);
        break;
#endif
#if defined(OCR5B)
    case OC5B:
        sbi(DDRL, PL4);
        break;
#endif
#if defined(OCR5C)
    case OC5C:
        sbi(DDRL, PL5);
        break;
#endif
    }
}

HardwareServo::HardwareServo(
        ServoPin outputPin, uint16_t minUs, uint16_t maxUs, uint16_t initUs)
    : timer(NULL_HWTIMER),
      timerTicks1us(0.0F),
      outputPin(outputPin),
      minPulseWidthUs(minUs),
      maxPulseWidthUs(maxUs),
      pulseWidthUs(initUs) {
    uint16_t ticksPer20ms;

    // sanity check min/max
    if(minPulseWidthUs >= maxPulseWidthUs) {
        // reset to defaults
        minPulseWidthUs = HWSRVO_DEFAULT_MIN_US;
        maxPulseWidthUs = HWSRVO_DEFAULT_MAX_US;
    }

    // calculate middle pulse width
    midPulseWidthUs =
            (uint16_t)round((float)((minPulseWidthUs + maxPulseWidthUs) / 2.0F));

    // calculate microseconds per degree
    microsPerDegree =
            (uint16_t)round((float)((maxPulseWidthUs - minPulseWidthUs) / 180.0F));

    // calculate microseconds per 1%
    microsPerPercent =
            (uint16_t)round((float)((maxPulseWidthUs - minPulseWidthUs) / 100.0F));

    // set the correct timer reference based on our pin
    switch(outputPin) {
    // Timer 1
#if defined(TCCR1A)
#if defined(OCR1A)
    case OC1A:
#endif
#if defined(OCR1B)
    case OC1B:
#endif
#if defined(OCR1C)
    case OC1C:
#endif
        timer = &Timer1;
        break;
#endif

    // Timer 3
#if defined(TCCR3A)
#if defined(OCR3A)
    case OC3A:
#endif
#if defined(OCR3B)
    case OC3B:
#endif
#if defined(OCR3C)
    case OC3C:
#endif
        timer = &Timer3;
        break;
#endif

    // Timer 4
#if defined(TCCR4A)
#if defined(OCR4A)
    case OC4A:
#endif
#if defined(OCR4B)
    case OC4B:
#endif
#if defined(OCR4C)
    case OC4C:
#endif
        timer = &Timer4;
        break;
#endif

    // Timer 5
#if defined(TCCR5A)
#if defined(OCR5A)
    case OC5A:
#endif
#if defined(OCR5B)
    case OC5B:
#endif
#if defined(OCR5C)
    case OC5C:
#endif
        timer = &Timer5;
        break;
#endif
    }

    // set pin output
    __setPinModeOutput(outputPin);

    // setup the timer
    ticksPer20ms = __setupServoTimer(timer);

    // calculate ticks per micro
    timerTicks1us = __ticksPerMicroS(ticksPer20ms);

    // TODO remove
    Serial.println(F("*** Servo setup values:"));
    Serial.print(F("  minUs: ")); Serial.println(minPulseWidthUs, DEC);
    Serial.print(F("  midUs: ")); Serial.println(midPulseWidthUs, DEC);
    Serial.print(F("  maxUs: ")); Serial.println(maxPulseWidthUs, DEC);
    Serial.print(F("  tTop:  ")); Serial.println(ticksPer20ms, DEC);
    Serial.print(F("  tk/Us: ")); Serial.println(timerTicks1us, 4);
}

void HardwareServo::setPulseWidth(uint16_t pulseWidthUs) {
    uint16_t pw, ocValue;
    float pulseTicks;

    if(pulseWidthUs < minPulseWidthUs) {
        pw = minPulseWidthUs;
    } else if(pulseWidthUs > maxPulseWidthUs) {
        pw = maxPulseWidthUs;
    } else {
        pw = pulseWidthUs;
    }

    this->pulseWidthUs = pw;

    // calculate ticks per pulse width
    pulseTicks = timerTicks1us * ((float)pw);

    // round to get ocValue
    ocValue = (uint16_t)(round(pulseTicks));

    // TODO remove
    Serial.print(F("*** Servo setting pulse ticks: "));
    Serial.println(ocValue, DEC);

    // set the new compare value in the right channel
    switch(outputPin) {
#if defined(OCR1A)
    case OC1A:
        timer->setCompareValueA(ocValue);
        break;
#endif
#if defined(OCR1B)
    case OC1B:
        timer->setCompareValueB(ocValue);
        break;
#endif
#if defined(OCR1C)
    case OC1C:
        timer->setCompareValueC(ocValue);
        break;
#endif
#if defined(OCR3A)
    case OC3A:
        timer->setCompareValueA(ocValue);
        break;
#endif
#if defined(OCR3B)
    case OC3B:
        timer->setCompareValueB(ocValue);
        break;
#endif
#if defined(OCR3C)
    case OC3C:
        timer->setCompareValueC(ocValue);
        break;
#endif
#if defined(OCR4A)
    case OC4A:
        timer->setCompareValueA(ocValue);
        break;
#endif
#if defined(OCR4B)
    case OC4B:
        timer->setCompareValueB(ocValue);
        break;
#endif
#if defined(OCR4C)
    case OC4C:
        timer->setCompareValueC(ocValue);
        break;
#endif
#if defined(OCR5A)
    case OC5A:
        timer->setCompareValueA(ocValue);
        break;
#endif
#if defined(OCR5B)
    case OC5B:
        timer->setCompareValueB(ocValue);
        break;
#endif
#if defined(OCR5C)
    case OC5C:
        timer->setCompareValueC(ocValue);
        break;
#endif
    }
}

void HardwareServo::setAngle(float degrees) {
    float offset;
    uint16_t pw;

    if(degrees < -90.0F) {
        // set the min
        setPulseWidth(minPulseWidthUs);
    } else if(degrees > 90.0F) {
        // set the max
        setPulseWidth(maxPulseWidthUs);
    } else {
        // calculate pulse width
        offset = degrees * (float)microsPerDegree;
        pw = midPulseWidthUs + (uint16_t)round(offset);

        // set new pulse width
        setPulseWidth(pw);
    }
}

void HardwareServo::setPosition(float percentage) {
    float width;
    uint16_t pw;

    if(percentage < 0.0F) {
        // set the min
        setPulseWidth(minPulseWidthUs);
    } else if(percentage > 100.0F) {
        // set the max
        setPulseWidth(maxPulseWidthUs);
    } else {
        // calculate pulse width
        width = percentage * (float)microsPerPercent;
        pw = (uint16_t)round(width);

        // set new pulse width
        setPulseWidth(pw);
    }
}
