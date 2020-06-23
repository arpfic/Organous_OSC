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
};

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
    led_drv.current(ALLPORTS, 0.5); //  Set all ports output current 50%
    led_drv.pwm(ALLPORTS, 1.0); //  Set all ports output current 50%
    // Init the register
    for (int i = 0; i < ENABLE_PINS; i++)
        OUTRegister[i] = OUT_IDLE;
    // oe = 0 means always on
    oe.write(0.0f);
    oe.period(1.0f);
    drv_rst = 1;
    // Attack-sustain Thread callback start
    coilThrd.start(callback(&coilQueue, &EventQueue::dispatch_forever));
}

/* Simple on() function, whose purpose is to set :
 * - PWM with PCA9956A, and
 * - ENABLE with NUCLEO_F767ZI's GPIO to DRV8844
 * Note : a multi-user stack is implemented.
 */
void CoilDriver::on(int port, float percent)
{
    if (percent >= 0.0f && percent <= 1.0f) {
        if (port == ALLPORTS) {
            for (int i = 0; i < ENABLE_PINS; i++) {
                // Enable the OUT
                drv_ena[i] = 1;
                // Increment if not already to max
                if (OUTRegister[i] < MAX_OUT_ENABLED) {
                    OUTRegister[i] = OUTRegister[i] + OUT_ENABLED1;
                }
            }
        } else {
            // Enable the OUT
            drv_ena[port] = 1;
            // Increment if not already to max
            if (OUTRegister[port] < MAX_OUT_ENABLED) {
                OUTRegister[port] = OUTRegister[port] + OUT_ENABLED1;
            }
        }
        // NB: ALLPORTS is already implemented into Class
        led_drv.pwm(port, (float)1.0 - percent);
    }
}

/* Same idea with off()
 */
void CoilDriver::off(int port)
{
    if (port == ALLPORTS) {
        for (int i = 0; i < ENABLE_PINS; i++) {
            // Decrement and do nothing more if more than OUT_ENABLED1
            if (OUTRegister[i] > OUT_ENABLED1) {
                OUTRegister[i] = OUTRegister[i] - OUT_ENABLED1;
            } else if (OUTRegister[i] == OUT_ENABLED1 || OUTRegister[i] == OUT_IDLE) {
                // Set to IDLE AND disable OUT
                OUTRegister[i] = OUT_IDLE;
                drv_ena[i] = 0;
                led_drv.pwm(i, (float)1.0);
            }
        }
    } else {
        if (OUTRegister[port] > OUT_ENABLED1) {
            OUTRegister[port] = OUTRegister[port] - OUT_ENABLED1;
        } else if (OUTRegister[port] == OUT_ENABLED1 || OUTRegister[port] == OUT_IDLE) {
            OUTRegister[port] = OUT_IDLE;
            drv_ena[port] = 0;
            led_drv.pwm(port, (float)1.0);
        }
    }
}

/* Same as off(), but the stack is flushed
 */
void CoilDriver::forceoff(int port)
{
    if (port == ALLPORTS) {
        for (int i = 0; i < ENABLE_PINS; i++) {
            OUTRegister[i] = OUT_IDLE;
            drv_ena[i] = 0;
            led_drv.pwm(i, (float)1.0);
        }
    } else {
        OUTRegister[port] = OUT_IDLE;
        drv_ena[port] = 0;
        led_drv.pwm(port, (float)1.0);
    }
}

/* Simple glue function to set PWM in PCA9956A.
 */
void CoilDriver::pwmSet(int port, float percent)
{
    if (percent >= 0.0f && percent <= 1.0f)
        led_drv.pwm( port, (float)1.0 - percent );
}

/* Simple function to enable/disable ENABLE_PINS (DRV8844)
 */
void CoilDriver::drvEnable(int port, int state)
{
    if (state >= 0 && state <= 1) {
        if (port == ALLPORTS) {
            for (int i = 0; i < ENABLE_PINS; i++) {
                drv_ena[i] = state;
            }
        } else {
            drv_ena[port] = state;
        }
    }
}

// Function called by coilQueue in coilOn() : Coil sustain to COIL_SUSTAIN PWM.
void CoilDriver::coilSustain(int port, float percent_sustain)
{
    if (drv_ena[port].read() != 0)
        led_drv.pwm(port, (float)1.0 - percent_sustain);
    return;
}

/* Set the coil to percent_attack PWM ratio, and...
 * execute coilSustain() after a delay with a queue to PWM ratio percent_sustain.
 */
void CoilDriver::coilOn(int port, float percent_attack, float percent_sustain, int millisec)
{
    on(port, percent_attack);
    coilQueue.call_in(millisec, this, &CoilDriver::coilSustain, port, percent_sustain);
}

// Same but with fixed COIL_ATTACK_DELAY millisec.
void CoilDriver::coilOn(int port)
{
    on(port, COIL_ATTACK);
    coilQueue.call_in(COIL_ATTACK_DELAY, this, &CoilDriver::coilSustain, port, COIL_SUSTAIN);
}

// gluecode to off()
void CoilDriver::coilOff(int port)
{
    off(port);
}

// Write PWM ratio (percent) to FastPWM oe
void CoilDriver::oeCycle(float percent)
{
    if (percent >= 0.0f && percent <= 1.0f)
        oe.write(percent);
}

// Same but with the period
void CoilDriver::oePeriod(float period_sec)
{
    if (period_sec > 0.0f)
        oe.period(period_sec);
}

/* Set ENABLE and the motor's PWM connected to IN1-IN2 or IN3-IN4 of a DRV8844
 * in a PUSH-PULL style
 */
int CoilDriver::motor(int port, int next_port, float percent_speed){
    if (next_port % 2 && next_port == port + 1 &&
            percent_speed >= -1.0f && percent_speed <= 1.0f) {
        if (percent_speed < 0.0f) {
            // Set PWM to PUSH PULL
            led_drv.pwm(port,      (float)1.0 + percent_speed);// Note the "+"
            led_drv.pwm(next_port, (float)1.0);// inversed :)
        } else if (percent_speed > 0.0f) {
            led_drv.pwm(next_port, (float)1.0 - percent_speed);
            led_drv.pwm(port,      (float)1.0);
        } else { // speed == 0.0f
            led_drv.pwm(port,      (float)1.0);
            led_drv.pwm(next_port, (float)1.0);
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
        led_drv.pwm(port,      (float)1.0);
        led_drv.pwm(next_port, (float)1.0);
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
        led_drv.pwm(port,      (float)1.0);
        led_drv.pwm(next_port, (float)1.0);
        // Set ENABLE to 1
        drv_ena[port]      = 0;
        drv_ena[next_port] = 0;
        return  0;
    } else {
        return -1;
    }
}
