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
#include "main_driver_register.h"

// Default constructor
DrvRegister::DrvRegister()
{
    init();
}

DrvRegister::~DrvRegister()
{
}

void DrvRegister::init(void){
    // Init the register
    for (int i = 0; i < ENABLE_PINS; i++){
        reg.users[i]  = 0;
        reg.enables[i] = 0;
        reg.values[i] = 0;
    }
    reg.oe_ratio = 0.0f; // means always on
    reg.oe_period = 1.0f;
}

// Return the ENABLE on the CURRENT user
bool DrvRegister::reg_readEnable(int port){
    bool enable;
    
    if(reg.users[port] > 0 && port >= 0 && port < ENABLE_PINS){
        // bit reading game
        enable = (reg.enables[port] >> (reg.users[port] - 1)) & 1;
        return enable;
    } else {
        // No user or out of range
        return 0;
    }
}

// Return the VALUE on the users of CHOICE from 1 to OUT_MAXUSERS. 0 if empty.
bool DrvRegister::reg_readEnable(int port, char user){
    bool enable;
    
    if (user <= reg.users[port] && user > 0 && port >= 0 && port < ENABLE_PINS){
        // bit reading game
        enable = (reg.enables[port] >> (user - 1)) & 1;
        return enable;
    } else {
        // No user or out of range
        return 0;
    }
}

// Return the VALUE on the CURRENT user
int DrvRegister::reg_readValue(int port){
    int value;

    if(reg.users[port] > 0 && port < ENABLE_PINS){
        // bit reading game
        value = (reg.values[port] >> ((reg.users[port] - 1) * 8)) & 0xFF;
        return value;
    } else if (reg.users[port] == 0){
        return 0;
    } else {
        // No user or out of range
        return -1;
    }
}

// Return the VALUE on the users from 1 to OUT_MAXUSERS
int DrvRegister::reg_readValue(int port, char user){
    int value;

    if (user <= reg.users[port] && user > 0 && port >= 0 && port < ENABLE_PINS){
        // bit reading game
        value = (reg.values[port] >> ((user - 1) * 8)) & 0xFF;
        return value;
    } else if (user == 0){
        return 0;
    } else {
        // No user or out of range
        return -1;
    }
}

// Return the CURRENT user. -1 if error.
int DrvRegister::reg_readUser(int port){
    if(port >= 0 && port < ENABLE_PINS ){
        return reg.users[port];
    } else {
        return -1;
    }
}

// Clean the current byte in the register. Return 0 if OK, -1 if error.
int  DrvRegister::reg_cleanValues(int port){
    if(port >= 0 && port < ENABLE_PINS){
        reg.values[port]  &= ~(0xff << ((reg.users[port] - 1) * 8));
        reg.enables[port] &= ~(0x01 << (reg.users[port] - 1));
        return 0;
    } else {
        return -1;
    }
}

// write ENABLE to the CURRENT user. Return enable if ok and -1 if error
int DrvRegister::reg_writeEnable(int port, bool enable){
    // bit writing game
    if (reg.users[port] > 0 && reg.users[port] < OUT_MAXUSERS && port >= 0 && port < ENABLE_PINS){
        reg.enables[port] |= (uint8_t)enable << reg.users[port];
        return enable;
    } else {
        return -1;
    }
}

// Return VALUE on the current user, and -1 if error
int DrvRegister::reg_writeValue(int port, int value){
    // bit writing game
    if (reg.users[port] > 0 && reg.users[port] < OUT_MAXUSERS && value < 256 && port >= 0 && port < ENABLE_PINS){
        // write the value
        reg.values[port] |= (uint8_t)value << (reg.users[port] * 8);
        return value;
    } else {
        // failed, full of users on the port
        return -1;
    }
}

// INCREASE and RETURN the CURRENT user. -1 if full or if error.
int DrvRegister::reg_increaseUser(int port){
    if (reg.users[port] < OUT_MAXUSERS && port >= 0 && port < ENABLE_PINS){
        reg.users[port]++;
        return (reg.users[port]);
    } else {
        // failed, full of users on the port
        return -1;
    }
}

// DECREASE and RETURN the CURRENT user. -1 if empty or if error.
int DrvRegister::reg_decreaseUser(int port){
    if (reg.users[port] > 0 && port >= 0 && port < ENABLE_PINS){
        reg.users[port]--;
        return (reg.users[port]);
    } else {
        // failed, no more users on the port
        return -1;
    }
}

// WRITE and RETURN the CURRENT OE PERIOD. -1 if error.
float DrvRegister::reg_readOEratio(void){
        return reg.oe_ratio;
}

float DrvRegister::reg_readOEperiod(void){
        return reg.oe_period;
}

// WRITE and RETURN the CURRENT OE PERIOD. -1 if error.
float DrvRegister::reg_writeOEratio(float ratio){
    if (ratio >= 0.0f && ratio <= 1.0f) {
        reg.oe_ratio = ratio;
        return reg.oe_ratio;
    } else {
        // failed, no more users on the port
        return -1;
    }
}

float DrvRegister::reg_writeOEperiod(float period){
    if (period > 0.0f) {
        reg.oe_period = period;
        return reg.oe_period;
    } else {
        // failed, no more users on the port
        return -1;
    }
}

/* PUSH or set (i.e. it can't be write twice) :
 * - the USER
 * - the VALUE
 * - the ENABLE state
 * It's an "all in one" function to speed up process.
 * Return the CURRENT USER if OK and -1 if error or if empty
 */
int DrvRegister::reg_pushPort(int port, int value, bool enable){
    if (reg.users[port] < OUT_MAXUSERS && value < 256 && port >= 0 && port < ENABLE_PINS){
        // write the value
        reg.values[port] |= (uint8_t)value << (reg.users[port] * 8);
        // write the enable
        reg.enables[port] |= (uint8_t)enable << reg.users[port];
        // increase the user
        reg.users[port]++;
        return reg.users[port];
    } else {
        return -1;
    }
}

/* PULL or extract the PREVIOUS STATE (we clean values so it can't be read twice) :
 * - the USER
 * - the VALUE
 * - the ENABLE state
 * It's an "all in one" function to speed up process. Return 0 if OK and
 * return -1 if error
 */
int DrvRegister::reg_pullPort(int port, char* user, int* value, bool* enable){
    if (port >= 0 && port < ENABLE_PINS){
        if(reg.users[port] > 0){
            // DECREASE NOW the USER
            reg.users[port] = reg.users[port] - 1;
            // read the reg
            *user = reg.users[port];
            *value  = (reg.values[port] >> ((reg.users[port] - 1) * 8)) & 0xFF;
            *enable = (reg.enables[port] >> (reg.users[port] - 1)) & 1;
            // then clean
            reg.values[port]  &= ~(0xff << (reg.users[port] * 8));
            reg.enables[port] &= ~(1 << reg.users[port]);
        } else if (reg.users[port] == 0) { // Last user, so we set all to 0'
            *user = reg.users[port] ;
            *value = 0;
            *enable = false;
        }
        return 0;
    } else {
        return -1;
    }
}

/* GET the CURRENT STATE :
 * - the USER
 * - the VALUE
 * - the ENABLE state
 * It's an "all in one" function to speed up process. Return 0 if OK and
 * return -1 if error
 */
int DrvRegister::reg_getPort(int port, char* user, int* value, bool* enable){
    if (port >= 0 && port < ENABLE_PINS){
        if(reg.users[port] > 0){
            // read the reg
            *user = reg.users[port];
            *value  = (reg.values[port] >> ((reg.users[port] - 1) * 8)) & 0xFF;
            *enable = (reg.enables[port] >> (reg.users[port] - 1)) & 1;
        } else if (reg.users[port] == 0) { // Last user, so we set all to 0'
            *user = reg.users[port] ;
            *value = 0;
            *enable = false;
        }
        return 0;
    } else {
        return -1;
    }
}

/* GET the MAIN STATE (i.e. don't touch) the register, so we write an array of :
 * - USERS
 * - VALUES
 * - ENABLE states
 * PLEASE NOTE : - we assume arrays of ENABLE_PINS elements
 */
void DrvRegister::reg_getAll(char* users, int* values, bool* enables){
    for(int i = 0; i < ENABLE_PINS ; i++){
        // read the main user
        users[i] = reg.users[i];

        if(reg.users[i] > 0){
            // read the value
            values[i] = (reg.values[i] >> ((reg.users[i] - 1) * 8)) & 0xFF;
            // read the enable
            enables[i] = (reg.enables[i] >> (reg.users[i] - 1)) & 1;
        } else {
            values[i] = -1;
            enables[i] = 0;
        }
    }
}

/* SET a NEW STATE (i.e. write on) the register. It takes an array of :
 * - VALUES
 * - ENABLE states
 * AND we set the MAIN STATE of user in a array
 * PLEASE NOTE : - we assume arrays of ENABLE_PINS elements
 *               - every user is incremented by 1,
 *               - if a value is set to -1, we don't touch the port
 */
void DrvRegister::reg_setAll(char* users, int* values, bool* enables){
    for(int i = 0; i < ENABLE_PINS ; i++){
        if (reg.users[i] < OUT_MAXUSERS && values[i] != -1 && values[i] < 256){
            // write the value
            reg.values[i] |= (uint8_t)values[i] << (reg.users[i] * 8);
            // write the enable
            reg.enables[i] |= (uint8_t)enables[i] << reg.users[i];
            // increase the user
            reg.users[i]++;
            users[i] = reg.users[i];
        }
    }
}

/* RESET a PORT. Return 0 if ok and -1 if error.
 *
 */
int DrvRegister::resetPort(int port){
    if (port >= 0 && port < ENABLE_PINS){
        reg.users[port]  = 0;
        reg.enables[port] = 0;
        reg.values[port] = 0;
        return 0;
    } else {
        return -1;
    }
}

void DrvRegister::resetAll(void){
    init();
}