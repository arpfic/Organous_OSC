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
#ifndef _MAIN_H
#define _MAIN_H

//#if !FEATURE_LWIP
//    #error [NOT_SUPPORTED] LWIP not supported for this target
//#endif

#include "EthernetInterface.h"
#include "mbed.h"
#include "nsapi_types.h"
#include "string.h"
#include "config.h"
#include "main_debug.h"
#include "PinDetect.h"
#include "UDPSocket.h"
#include "FastPWM.h"
#include "main_driver_hal.h"
#include "PCA9956A.h"
#include "tOSC.h"

DigitalOut driver_a_table[ENABLE_PINS] = {
    DRV_EN1,  DRV_EN2,  DRV_EN3,  DRV_EN4,  DRV_EN5,  DRV_EN6,  DRV_EN7,  DRV_EN8,
DRV_EN9,  DRV_EN10, DRV_EN11, DRV_EN12, DRV_EN13, DRV_EN14, DRV_EN15, DRV_EN16,
DRV_EN17, DRV_EN18, DRV_EN19, DRV_EN20, DRV_EN21, DRV_EN22, DRV_EN23, DRV_EN24
    };
    
DigitalOut driver_b_table[ENABLE_PINS] = {
    DRV_EN25, DRV_EN26, DRV_EN27, DRV_EN28, DRV_EN29, DRV_EN30, DRV_EN31, DRV_EN32,
DRV_EN33, DRV_EN34, DRV_EN35, DRV_EN36, DRV_EN37, DRV_EN38, DRV_EN39, DRV_EN40,
DRV_EN41, DRV_EN42, DRV_EN43, DRV_EN44, DRV_EN45, DRV_EN46, DRV_EN47, DRV_EN48
    };

static void handle_socket();
static void receive_message();
void handler_Packetevent();
void init_msgON();
static void debug_OSC(const char* incoming_msg);
static void debug_OSCmsg(char* incoming_msg, int in_length);
static void send_UDPmsg(char*, int);
void driver_A_error_handler();
void driver_B_error_handler();
void button_pressed();
void button_released();
void menu_main();
void menu_tools();
void menu_tools_connect();
void menu_tools_debug();
void menu_tools_hardreset();
void menu_tools_softreset();
void menu_main_coil();
void menu_main_forceoff_all();
void menu_main_pwm_all();
void menu_main_oe();
void menu_main_tone();
void sampler_timer();
int main();

static void debug_OSCmsg(char* incoming_msg, int in_length)
{
    char buffer[MAX_PQT_LENGTH];
    int out_length = 0;

    if (in_length <= MAX_PQT_LENGTH) {
        out_length = debug_OSCwritemsg(incoming_msg, in_length, buffer);
        send_UDPmsg(buffer, out_length);
    }
}

static void debug_OSC(const char* incoming_msg)
{
    char buffer[MAX_PQT_LENGTH];
    int out_length = 0;

    if (strlen(incoming_msg) < MAX_PQT_LENGTH) {
        out_length = debug_OSCwrite((char *)incoming_msg, buffer);
        send_UDPmsg(buffer, out_length);
    }
}

#endif // _MAIN_H
