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
#include "main_driver_hal.h"

// Default constructor
CoilDriver::CoilDriver(PinName _i2c_sda, PinName _i2c_scl, PinName _pinoe,
                       PinName _pindrv_rst, PinName _pindrv_fault, DigitalOut *_driver_table,
                       event_callback_t _i2c_cb_function, char _i2c_addr)
    :   i2c_p( new I2C( _i2c_sda, _i2c_scl ) ), i2c( *i2c_p ),
        i2c_cb_function(_i2c_cb_function),
        i2c_addr(_i2c_addr),
        led_drv_p(new PCA9956A(i2c, i2c_cb_function, i2c_addr)), led_drv(*led_drv_p),
        oe(_pinoe),
        drv_rst(_pindrv_rst),
        drv_fault(_pindrv_fault),
        drv_ena(_driver_table)
{
    init();
}

CoilDriver::~CoilDriver()
{
    if ( NULL != led_drv_p )
        delete  led_drv_p;
    if ( NULL != i2c_p )
        delete  i2c_p;
}

/* init()
 * - drv_rst sets to 0 to enable drivers (see the DRV8844 datasheet)
 * - I2C freq to Fastmode (1Mhz)
 * - LED current sets to 0.5 to activate DRV8844
 * - Remember that OUTPUTS are inversed because of the PCA9956A mechanism, so
 *   we have to set all LEDS to ON.
 * - OUTRegister stack is initiazed to OUT_IDLE
 * - OE (PCA9956A) have to be 0 to activate OUTPUTS.
 * - TODO: maybe set drv_rst = 1 ?
 * - coilOn queue and callback is started
 */
void CoilDriver::init( void )
{
    drv_rst = 0;
    i2c.frequency(1000000);
    led_drv.current(ALLPORTS, 127); //  Set all ports output current 50%
    led_drv.pwm(ALLPORTS, OFF);     //  Set all ports output to OFF
    // oe = 0 means always on
    oe.write(0.0f);
    oe.period(1.0f);
    drv_rst = 1;
    // coil* Attack-sustain Thread callback start
    coilThrd.start(callback(&coilQueue, &EventQueue::dispatch_forever));
}

/*----------------------------------------------------------------------------/
/  LOW-LEVEL FUNCTIONS                                                        /
/----------------------------------------------------------------------------*/
/* Simple on() function, whose purpose is to set :
 * - PWM with PCA9956A, and
 * - ENABLE with NUCLEO_F767ZI's GPIO to DRV8844
 */
void CoilDriver::on(int port, uint8_t ratio)
{
    // NB: ALLPORTS is already implemented into Class
    if (port == ALLPORTS) {
        for (int i = 0; i < ENABLE_PINS; i++) {
            if (outRegister.reg_pushPort(i, ratio, true) != -1) {
                led_drv.pwm(i, 255 - ratio);
                drv_ena[i] = 1;
            }
        }
    } else {
        if (outRegister.reg_pushPort(port, ratio, true) != -1) {
        led_drv.pwm(port, 255 - ratio);
        // Enable the OUT
        drv_ena[port] = 1;
        }
    }
}

/* Same idea with off()
 */
void CoilDriver::off(int port)
{
    char user = 0;
    int value = 0; 
    bool enable = false;

    if (port == ALLPORTS) {
        for (int i = 0; i < ENABLE_PINS; i++) {
            if (outRegister.reg_pullPort(i, &user, &value, &enable) != -1) {
                drv_ena[i] = enable;
                led_drv.pwm(i, (255 - value));
            }
        }
    } else {
        if (outRegister.reg_pullPort(port, &user, &value, &enable) != -1) {
            drv_ena[port] = enable;
            led_drv.pwm(port, (255 - value));
        }
    }
}

/* Same as off(), but the stack is flushed
 */
void CoilDriver::forceoff(int port)
{
    if (port == ALLPORTS) {
        for (int i = 0; i < ENABLE_PINS; i++) {
            outRegister.resetAll();
            drv_ena[i] = 0;
            led_drv.pwm(i, OFF);
        }
    } else {
        outRegister.resetPort(port);
        drv_ena[port] = 0;
        led_drv.pwm(port, OFF);
    }
}

/* Simple glue function to set PWM in PCA9956A.
 */
void CoilDriver::pwmSet(int port, uint8_t ratio)
{
    if (port == ALLPORTS) {
        for (int i = 0; i < ENABLE_PINS; i++) {
            if (outRegister.reg_readUser(i) == 0)
                outRegister.reg_increaseUser(i);
            outRegister.reg_writeValue(i, ratio);
        }
        led_drv.pwm(ALLPORTS, 255 - ratio);
    } else {
        if (outRegister.reg_readUser(port) == 0)
            outRegister.reg_increaseUser(port);
        outRegister.reg_writeValue(port, ratio);
        led_drv.pwm(port, 255 - ratio);
    }
}

/* Simple function to enable/disable ENABLE_PINS (DRV8844)
 */
void CoilDriver::drvEnable(int port, int state)
{
    if (state >= 0 && state <= 1) {
        if (port == ALLPORTS) {
            for (int i = 0; i < ENABLE_PINS; i++) {
                if (outRegister.reg_readUser(i) == 0)
                    outRegister.reg_increaseUser(i);
                outRegister.reg_writeEnable(i, (bool)state);
                drv_ena[i] = state;
            }
        } else {
            if (outRegister.reg_readUser(port) == 0)
                outRegister.reg_increaseUser(port);
            outRegister.reg_writeEnable(port, (bool)state);
            drv_ena[port] = state;
        }
    }
}

/*----------------------------------------------------------------------------/
/  HIGH-LEVEL FUNCTIONS                                                      /
/----------------------------------------------------------------------------*/
/* Internal callback function called by coilQueue in coilOn() :
 * Coil sustain to COIL_SUSTAIN PWM.
 */
void CoilDriver::coilSustain(int port, uint8_t sustain, int sustain_user)
{
    // Is the state changed between call_in() ?
    if (sustain_user == outRegister.reg_readUser(port)) {
        outRegister.reg_cleanValues(port);
        outRegister.reg_decreaseUser(port);
        if (outRegister.reg_pushPort(port, sustain, true) != -1) {
            // ena still 1
            led_drv.pwm(port, 255 - sustain);
        }
    }
}

/* Set the coil to attack PWM ratio, and...
 * execute coilSustain() after a delay with a queue to PWM ratio sustain.
 * TODO: don't call the queue if error
 */
void CoilDriver::coilOn(int port, uint8_t attack, uint8_t sustain, int millisec)
{
    on(port, attack);
    int sustain_user = outRegister.reg_readUser(port);
    coilQueue.call_in(millisec, this, &CoilDriver::coilSustain, port, sustain, sustain_user);
}

// Same but with fixed COIL_ATTACK_DELAY millisec.
void CoilDriver::coilOn(int port)
{
    on(port, COIL_ATTACK);
    int sustain_user = outRegister.reg_readUser(port);
    coilQueue.call_in(COIL_ATTACK_DELAY, this, &CoilDriver::coilSustain, port, COIL_SUSTAIN, sustain_user);
}

// gluecode to off()
void CoilDriver::coilOff(int port)
{
    off(port);
}

// Write PWM ratio (0-255) to FastPWM oe
void CoilDriver::oeCycle(float ratio)
{
    float r = outRegister.reg_writeOEratio(ratio);
    if (r != -1)
        oe.write(r);
}

// Same but with the period
void CoilDriver::oePeriod(float period_sec)
{
    float r = outRegister.reg_writeOEperiod(period_sec);
    if (r != -1)
        oe.period(r);
}

/* Set ENABLE and the motor's PWM connected to IN1-IN2 or IN3-IN4 of a DRV8844
 * in a PUSH-PULL style
 */
int CoilDriver::motor(int port, int next_port, int speed){
    if (next_port % 2 && next_port == port + 1 &&
            speed >= -255 && speed <= 255) {
        if (speed < 0) {
            // Set PWM to PUSH PULL
            led_drv.pwm(port,      (uint8_t)(255 + speed));// Note the "+"
            led_drv.pwm(next_port, OFF);// inversed :)
        } else if (speed > 0) {
            led_drv.pwm(next_port, (uint8_t)(255 - speed));
            led_drv.pwm(port,      OFF);
        } else { // speed == 0
            led_drv.pwm(port,      OFF);
            led_drv.pwm(next_port, OFF);
        }
        // Open valves !
        drv_ena[port]      = 1;
        drv_ena[next_port] = 1;
        return  0;
    } else {
        return -1;
    }
}

// Brake the motor by turning BOTH ENABLE to 1 *and* the PWM to NULL (1.0)
int CoilDriver::motorBrake(int port, int next_port) {
    if (next_port % 2 && next_port == port + 1) {
        // Set PWM to MAXIMUM
        led_drv.pwm(port,      OFF);
        led_drv.pwm(next_port, OFF);
        // Set ENABLE to 1
        drv_ena[port]      = 1;
        drv_ena[next_port] = 1;
        return  0;
    } else {
        return -1;
    }
}

// Coast the motor by turning BOTH ENABLE to 0 *and* the PWM to NULL (1.0)
int CoilDriver::motorCoast(int port, int next_port){
    if (next_port % 2 && next_port == port + 1) {
        // Set PWM to MINIMUM
        led_drv.pwm(port,      OFF);
        led_drv.pwm(next_port, OFF);
        // Set ENABLE to 1
        drv_ena[port]      = 0;
        drv_ena[next_port] = 0;
        return  0;
    } else {
        return -1;
    }
}
