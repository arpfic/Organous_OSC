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
#ifndef _MAIN_DRIVER_REGISTER_H
#define _MAIN_DRIVER_REGISTER_H

/* -- debugging purpose on UNIX
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <math.h>
#define ENABLE_PINS                             24
#define OUT_MAXUSERS                            4
*/
#include "mbed.h"
#include "config.h"

class DrvRegister
{
private:
    typedef struct outRegister{
        uint8_t  users[ENABLE_PINS];
        uint8_t  enables[ENABLE_PINS];
        uint32_t values[ENABLE_PINS];
        float    oe_ratio;
        float    oe_period;
    } outRegister_t;

    void init(void);

public:
    DrvRegister();

    // Create a db, public for now
    outRegister_t reg;

    bool reg_readEnable(int port);
    bool reg_readEnable(int port, char user);
    int  reg_readValue(int port);
    int  reg_readValue(int port, char user);
    int  reg_readUser(int port);
    int  reg_cleanValues(int port);
    int  reg_writeEnable(int port, bool value);
    int  reg_writeValue(int port, int value);
    int  reg_increaseUser(int port);
    int  reg_decreaseUser(int port);

    float  reg_readOEratio(void);
    float  reg_readOEperiod(void);
    float  reg_writeOEratio(float ratio);
    float  reg_writeOEperiod(float period);

    // More high-level functions
    int  reg_pushPort(int port, int value, bool enable);
    int  reg_pullPort(int port, char* user, int* value, bool* enable);
    int  reg_getPort(int port, char* user, int* value, bool* enable);
    
    // SET and GET the MAIN STATE register at once. Take arrays as inputs/outputs
    void reg_getAll(char* users, int* values, bool* enables);
    void reg_setAll(char* users, int* values, bool* enables);

    int  resetPort(int port);
    void resetAll(void);

    virtual ~DrvRegister();
};

#endif // _MAIN_DRIVER_REGISTER_H