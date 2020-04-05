#include "main_driver_hal.h"

// Default constructor
CoilDriver::CoilDriver(PinName _i2c_sda, PinName _i2c_scl, PinName _pinoe,
                       PinName _pindrv_rst, PinName _pindrv_fault, DigitalOut *_driver_table,
                       char _i2c_addr)
    :   i2c_p( new I2C( _i2c_sda, _i2c_scl ) ), i2c( *i2c_p ),
        i2c_addr(_i2c_addr),
        led_drv_p(new PCA9956A(i2c, i2c_addr)), led_drv(*led_drv_p),
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
 * - drv_rst est à 0
 * - Fréquence I2C
 * - Limitation du courant dans les LED
 * - On allume toutes les LED (pour rappel la sortie du driver est inversé)
 * - DigitalOut sur chaque drv_ena est initialité de lui-même à 0
 * - OE est activé
 * - drv_rst est à 1 pour le moment
 */
void CoilDriver::init( void )
{
    drv_rst = 0;
    i2c.frequency(1000000);
    led_drv.current(ALLPORTS, 0.5); //  Set all ports output current 50%
    led_drv.pwm(ALLPORTS, 1.0); //  Set all ports output current 50%
    // oe = 0 means always on
    oe.write(0.0f);
    oe.period(1.0f);
    drv_rst = 1;
    coilThrd.start(callback(&coilQueue, &EventQueue::dispatch_forever));
}

void CoilDriver::on(int port, float percent)
{
    if (percent >= 0.0f && percent <= 1.0f) {
        if (port == ALLPORTS) {
            for (int i = 0; i < ENABLE_PINS; i++) {
                drv_ena[i] = 1;
            }
        } else {
            drv_ena[port] = 1;
        }
        led_drv.pwm( port, (float)1.0 - percent );
    }
}

void CoilDriver::off(int port)
{
    if (port == ALLPORTS) {
        for (int i = 0; i < ENABLE_PINS; i++) {
            drv_ena[i] = 0;
        }
    } else {
        drv_ena[port] = 0;
    }
    led_drv.pwm( port, (float)1.0 );
}

void CoilDriver::pwmSet(int port, float percent)
{
    if (percent >= 0.0f && percent <= 1.0f)
        led_drv.pwm( port, (float)1.0 - percent );
}

void CoilDriver::drvEnable(int port, bool state)
{
    if (port == ALLPORTS) {
        for (int i = 0; i < ENABLE_PINS; i++) {
            drv_ena[i] = state;
        }
    } else {
        drv_ena[port] = state;
    }
}

void CoilDriver::coilSustain(int port, float percent_sustain)
{
    if (drv_ena[port].read() != 0)
        led_drv.pwm(port, (float)1.0 - percent_sustain);
    return;
}

void CoilDriver::coilOn(int port, float percent_attack, float percent_sustain, int millisec)
{
    on(port, percent_attack);
    coilQueue.call_in(millisec, this, &CoilDriver::coilSustain, port, percent_sustain);
}

void CoilDriver::coilOn(int port)
{
    on(port, COIL_ATTACK);
    coilQueue.call_in(COIL_ATTACK_DELAY, this, &CoilDriver::coilSustain, port, COIL_SUSTAIN);
}

void CoilDriver::coilOff(int port)
{
    off(port);
}

void CoilDriver::oeCycle(float percent)
{
    if (percent >= 0.0f && percent <= 1.0f)
        oe.write(percent);
}

void CoilDriver::oePeriod(float period_sec)
{
    if (period_sec > 0.0f)
        oe.period(period_sec);
}