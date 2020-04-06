#ifndef _MAIN_DRIVER_HAL_H
#define _MAIN_DRIVER_HAL_H

#include "mbed.h"
#include "config.h"
#include "PinDetect.h"
#include "FastPWM.h"
#include "PCA9956A.h"

class CoilDriver
{
protected:
    EventQueue coilQueue;
    Thread coilThrd;

private:
    PinName sda, scl;

    I2C*    i2c_p;
    I2C     &i2c;

    char    i2c_addr;

    PCA9956A*   led_drv_p;
    PCA9956A    &led_drv;
    FastPWM oe;

    void    init(void);
    void    coilSustain(int port, float percent_sustain);

public:
    // Pour remplir un registre des sorties
    enum OUTState {
        OUT_IDLE = 0,
        OUT_ENABLED1,
        OUT_ENABLED2,
        OUT_ENABLED3,
        OUT_ENABLED4
    };

    int OUTRegister[ENABLE_PINS];

    CoilDriver(PinName _i2c_sda, PinName _i2c_scl, PinName _pinoe,
               PinName _pindrv_rst, PinName _pindrv_fault, DigitalOut *_driver_table,
               char _i2c_addr = DEFAULT_I2C_TAG);

    DigitalOut drv_rst;
    PinDetect drv_fault;
    DigitalOut* drv_ena;

    void    on(int port, float percent);
    void    off(int port);
    void    forceoff(int port);
    void    pwmSet(int port, float percent);
    void    drvEnable(int port, bool state);

    void    coilOn(int port, float percent_attack, float percent_sustain, int millisec);
    void    coilOn(int port);
    void    coilOff(int port);

    void    oeCycle(float percent);
    void    oePeriod(float period_sec);

    virtual ~CoilDriver();
};

#endif // _MAIN_DRIVER_HAL_H