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

#ifndef LLAVR_HARDWARESERVO_H_
#define LLAVR_HARDWARESERVO_H_

#include "HardwareTimer.h"

/** @brief Default minimum pulse width (microseconds */
#define HWSRVO_DEFAULT_MIN_US   ((uint16_t)1000)
/** @brief Default maximum pulse width (microseconds */
#define HWSRVO_DEFAULT_MAX_US   ((uint16_t)2000)

/**
 * @brief Available 16-bit hardware PWM output pins
 */
typedef enum {
#if defined(OCR1A)
    OC1A,
#endif
#if defined(OCR1B)
    OC1B,
#endif
#if defined(OCR1C)
    OC1C,
#endif
#if defined(OCR3A)
    OC3A,
#endif
#if defined(OCR3B)
    OC3B,
#endif
#if defined(OCR3C)
    OC3C,
#endif
#if defined(OCR4A)
    OC4A,
#endif
#if defined(OCR4B)
    OC4B,
#endif
#if defined(OCR4C)
    OC4C,
#endif
#if defined(OCR5A)
    OC5A,
#endif
#if defined(OCR5B)
    OC5B,
#endif
#if defined(OCR5C)
    OC5C,
#endif
} ServoPin;

/**
 * @brief API for controlling servos with 16-bit AVR hardware timers
 */
class HardwareServo {
public:
    /**
     * @brief Constructor
     *
     * NOTE: If the given min. value is >= the given max. value, the given
     *   values are ignored and the default values are set.
     *
     * NOTE: The given pin is automatically set to be an output.
     *
     * NOTE: The given output pin defines which hardware timer this servo object
     *   will use, overriding any non-HardwareServo use of the timer.
     *
     * @param outputPin Which pin to output the PWM pulses on
     * @param minUs The minimum pulse width (microseconds) to use for this servo
     *   (defaults to HWSRVO_DEFAULT_MIN_US)
     * @param maxUs The maximum pulse width (microseconds) to use for this servo
     *   (defaults to HWSRVO_DEFAULT_MAX_US)
     * @param initUs The initial pulse width (microseconds) to use for this
     *   servo (defaults to HWSRVO_DEFAULT_MIN_US)
     */
    HardwareServo(ServoPin outputPin,
            uint16_t minUs = HWSRVO_DEFAULT_MIN_US,
            uint16_t maxUs = HWSRVO_DEFAULT_MAX_US,
            uint16_t initUs = HWSRVO_DEFAULT_MIN_US);

    /**
     * @brief Set the pulse width (in microseconds) to output to this servo
     *
     * NOTE: Values less than the set minimum set the minimum value. Values
     *   greater than the set maximum value set the maximum value.
     *
     * @param pulseWidthUs The pulse width to set (microseconds)
     *
     * @return true if the requested pulse width was set
     */
    bool setPulseWidth(uint16_t pulseWidthUs);

    /**
     * @brief Set the servo angle, in +/- degrees from center; useful for servos
     *   attached to control surfaces
     *
     * NOTE: Assumes -90.0 degrees == min. pulse width and +90.0 degrees == max.
     *   pulse width.
     *
     * NOTE: Values greater than +90.0 set the max. pulse width. Values less
     *   than -90.0 set the min. pulse width.
     *
     * @param degrees The servo angle to set (-90.0 <= degrees <= 90.0)
     *
     * @return true if the requested angle was set
     */
    bool setAngle(float degrees);

    /**
     * @brief Set the servo position, as a percentage of the maximum; useful for
     *   servos attached to throttles
     *
     * NOTE: Assumes 0.0 == min. pulse width and 100.0 == max. pulse width.
     *
     * NOTE: Values greater than 100.0 set the max. pulse width. Values less
     *   than 0.0 set the min. pulse width.
     *
     * @param percentage The servo position to set (0.0 <= percentage <= 100.0)
     *
     * @return true if the requested position was set
     */
    bool setPosition(float percentage);

protected:
    /** @brief the HW timer to use for this servo */
    HardwareTimer *timer;

    /** @brief number of timer ticks per 1 microsecond */
    float timerTicks1us;

    /** @brief number of microseconds (pulse width) per degree servo travel */
    uint16_t microsPerDegree;

    /** @brief number of microseconds (pulse width) per 1% servo travel */
    uint16_t microsPerPercent;

private:
    /** @brief the output pin for this servo */
    ServoPin outputPin;

    /** @brief the minimum pulse width (microseconds) */
    uint16_t minPulseWidthUs;

    /** @brief the maximum pulse width (microseconds) */
    uint16_t maxPulseWidthUs;

    /** @brief the middle (0 degrees/50%) pulse width (microseconds) */
    uint16_t midPulseWidthUs;

    /** @brief the current pulse width (microseconds) */
    uint16_t pulseWidthUs;
};

#endif /* LLAVR_HARDWARESERVO_H_ */
