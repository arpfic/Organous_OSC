/*
    Copyright (c) 2020 Damien Leblois
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/
#ifndef _MAIN_DRIVER_HAL_H
#define _MAIN_DRIVER_HAL_H

#include "mbed.h"
#include "config.h"
#include "main_driver_register.h"
#include "PinDetect.h"
#include "FastPWM.h"
#include "SoftPWM.h"
#include "PCA9956A.h"

/* CoilDriver class, a HAL for controling OUT pins with :
 * - PWM control from the I2C LED driver (PCA9956A) to H-bridges INPUTS (DRV8844)
 *   with the led_drv object.
 * - I/O control from GPIOs (NUCLEO_F767ZI) to H-bridges ENABLEs (DRV8844) with
 *   DigitalOut driver_a_table[] or driver_b_table[] (in main.h).
 */
class CoilDriver
{
protected:
    EventQueue coilQueue;
    Thread coilThrd;

private:
    PinName sda, scl;

    I2C*    i2c_p;
    I2C     &i2c;

    event_callback_t i2c_cb_function;
    char    i2c_addr;

    PCA9956A*   led_drv_p;
    PCA9956A    &led_drv;
    FastPWM     oe;

    void    init(void);
    void    coilSustain(int port, uint8_t sustain, int sustain_user);

public:
    CoilDriver(PinName _i2c_sda, PinName _i2c_scl, PinName _pinoe,
               PinName _pindrv_rst, PinName _pindrv_fault, DigitalOut *_driver_table,
               event_callback_t _i2c_cb_function, char _i2c_addr = DEFAULT_I2C_TAG);

    /* This is a stack of multiuser calls to coilOn/coilOff
     */
    enum OUTState {
        OUT_IDLE = 0,
        OUT_ENABLED1,
        OUT_ENABLED2,
        OUT_ENABLED3,
        OUT_ENABLED4
    };

    DrvRegister outRegister;

    // Note : all DRV8844s shared RESET and FAULT PINS.
    DigitalOut  drv_rst;
    PinDetect   drv_fault;
    // pointer to DRV8844s driver_X_table ENABLE PINS (see main.h)
    DigitalOut* drv_ena;

    void    on(int port, uint8_t ratio);
    void    off(int port);
    void    forceoff(int port);
    void    pwmSet(int port, uint8_t ratio);
    void    drvEnable(int port, int state);

    /* coilOn function is designed to drive coils through DRV8844 with :
     * - a brief peak (COIL_ATTACK) of COIL_ATTACK_DELAY millisec, then
     * - a sustain of COIL_SUSTAIN millisec
     * This help saving solenoids valves.
     */
    void    coilOn(int port, uint8_t attack, uint8_t sustain, int millisec);
    void    coilOn(int port);
    void    coilOff(int port);

    /* motor function is designed to drive motor with TWO PINS with :
     * - port and next_port, paired on the same DRV8844 to IN1-IN2 or IN3-IN4
     * - a speed from -255 to +255
     * - you can bracke it (all OUT ON) or coast it (all OUT off)
     * Note : This pairing is because pin 6 (SRC12) is the source for the
     * low-side FETs of OUT1 and OUT2, and pin 9 (SRC34) is the source for
     * the low-side FETs of OUT3 and OUT4.
     * !!! RETURN -1 if ports are wrong !!!
     */
    int    motor(int port, int next_port, int speed);
    int    motorBrake(int port, int next_port);
    int    motorCoast(int port, int next_port);

    /* Special control of OE in PCA9956A, wich allows for hardware blinking
     * and dimming of all LEDs -- connected to NUCLEO_F767ZI and controlled
     * by FastPWM class.
     */
    void    oeCycle(float ratio);
    void    oePeriod(float period_sec);

    virtual ~CoilDriver();
};

#endif // _MAIN_DRIVER_HAL_H
