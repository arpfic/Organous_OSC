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

/* OSC menu parser. See main.cpp for details.
 */
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

/* Menu structure with function pointers. Still OK with a modest menu but 
 * there is for sure better ideas for a better algorithmic complexity.
 */
struct menu_cases {
    const char* menu_string;
    void (*menu_func)(void);
};

menu_cases cases [] = {
    { "/main", menu_main },
    { "/tools", menu_tools }
};

menu_cases tools_cases [] = {
    { "/tools/connect",     menu_tools_connect },
    { "/tools/debug",       menu_tools_debug },
    { "/tools/hardreset",   menu_tools_hardreset },
    { "/tools/softreset",   menu_tools_softreset }
};

menu_cases main_cases [] = {
    { "/main/coil",         menu_main_coil },
    { "/main/forceoff_all",  menu_main_forceoff_all },
    { "/main/pwm_all",      menu_main_pwm_all },
    { "/main/oe",           menu_main_oe },
    { "/main/tone",         menu_main_tone }
//    { "/main/count",        menu_main_count }
};

/* -----------------------------------------------------------------------------
 * MENU OSC Parser : HERE WE ACT !
 */

/* OSC msg  : /tools/connect NONE (Bang)
 * Purpose  : connect NUCLEO_F767ZI to client (set IP address)
 */
void menu_tools_connect()
{
    // Copy IP from incoming packet
    strncpy(master_address, const_cast<char*>(source_addr->get_ip_address()), 16 * sizeof(char));

    // RE-assign socket with the new address
    delete source_addr;
    source_addr = new SocketAddress;

    debug_OSC("OK");
    if (debug_on)
        debug_OSC(master_address);
}

/* OSC msg  : /tools/debug NONE (Bang)
 * Purpose  : set debug ON to send many messages to client
 */
void menu_tools_debug()
{
    debug_on = 1;
    debug_OSC("DEBUG ON");
}

/* OSC msg  : /tools/hardreset NONE (Bang)
 * Purpose  : Hard reset the board
 */
void menu_tools_hardreset()
{
    // Blink for fun
    led_green = led_blue = led_red = 1;
    debug_OSC("REBOOTING...");
    //wait(1);
    // Ticker destruct
    sample_ticker.detach();
    // Delete classes
    delete driver_A;
#if B_SIDE == 1
    delete driver_B;
#endif
    delete eth;
    delete my_socket;
    delete source_addr;
    delete master_address;
    // Just reset the main board
    NVIC_SystemReset();
    //HAL_NVIC_SystemReset();
}

/* OSC msg  : /tools/softreset NONE (Bang)
 * Purpose  : "Soft reset" the board : set to init as possible
 */
void menu_tools_softreset()
{
    // Blink for fun
    led_green = led_blue = led_red = 1;
    debug_OSC("RESET STATES...");
    // Ticker destruct
    sample_ticker.detach();
    // Set All enables to 0
    driver_A->forceoff(ALLPORTS);
    // Deleting all objects for reninit
    delete driver_A;
    // Create new objects
    driver_A = new CoilDriver(PCA_A_SDA, PCA_A_SCL, PCA_A_OE, DRV_A_RST,
                              DRV_A_FAULT, driver_a_table, A_SIDE_I2C_TAG);
#if B_SIDE == 1
    driver_B->forceoff(ALLPORTS);
    delete driver_B;
    driver_B = new CoilDriver(PCA_B_SDA, PCA_B_SCL, PCA_B_OE, DRV_B_RST,
                              DRV_B_FAULT, driver_b_table, B_SIDE_I2C_TAG);
#endif
    led_green = led_blue = led_red = 0;
}

/* OSC msg  : /main/coil ii PORT INTENSITY
 * Purpose  : drive coilOn/coilOff functions
 * Note     : coilOff if INTENSITY == 0
 */
void menu_main_coil()
{
    // Blink for fun
    led_blue = !led_blue;
    if (p_osc->format[0] == 'i' && p_osc->format[1] == 'i') {
        int tone = tosc_getNextInt32(p_osc);
        int release = tosc_getNextInt32(p_osc);
        if (tone >= 0 && tone < 24 ) {
            if (release == 0) {
                if (debug_on) {
                    char buf[64];
                    sprintf(buf, "COIL %i : %i use(s)", tone, (int)driver_A->OUTRegister[tone]);
                    debug_OSC(buf);
                }
                driver_A->coilOff(tone);
            } else {
                driver_A->coilOn(tone);
            }
#if B_SIDE == 1
        } else if (tone >= 24 && tone < 48 ) {
            if (release == 0) {
                if (debug_on) {
                    char buf[64];
                    sprintf(buf, "COIL %i : %i use(s)", tone, (int)driver_B->OUTRegister[tone - 24]);
                    debug_OSC(buf);
                }
                driver_B->coilOff(tone - 24);
            } else {
                driver_B->coilOn(tone - 24);
            }
#endif
        }
    }
}

/* OSC msg  : /main/forceoff_all NONE (Bang)
 * Purpose  : re-init OE and call forceoff ALLPORTS
 */
void menu_main_forceoff_all()
{
    // Blink for fun
    led_blue = !led_blue;
    driver_A->forceoff(ALLPORTS);
    driver_A->oeCycle(0.0f);
    driver_A->oePeriod(1.0f);
#if B_SIDE == 1
    driver_B->forceoff(ALLPORTS);
    driver_B->oeCycle(0.0f);
    driver_B->oePeriod(1.0f);
    sample_ticker.detach();
#endif
}

/* OSC msg  : /main/pwm_all f RATIO
 * Purpose  : control blinking of all LEDS at the same time
 * Note     : Can be used in conjunction with OE
 */
void menu_main_pwm_all()
{
    // Blink for fun
    led_blue = !led_blue;
    if (p_osc->format[0] == 'f') {
        float pwm = tosc_getNextFloat(p_osc);
        if (pwm > 0 && pwm <=1 ) {
            driver_A->pwmSet(ALLPORTS, pwm);
            driver_A->drvEnable(ALLPORTS, true);
#if B_SIDE == 1
            driver_B->pwmSet(ALLPORTS, pwm);
            driver_B->drvEnable(ALLPORTS, true);
#endif
        } else if (pwm == 0.0f) {
            driver_A->pwmSet(ALLPORTS, pwm);
            driver_A->drvEnable(ALLPORTS, false);
#if B_SIDE == 1
            driver_B->pwmSet(ALLPORTS, pwm);
            driver_B->drvEnable(ALLPORTS, false);
#endif
        }
    }
}

/* OSC msg  : /main/oe ff CYCLE_RATIO PERIOD_SEC
 * Purpose  : set OE FastPWM config and control blinking of all LEDS at the same time
 * Note     : can be used in conjunction with other functions -- currently we DON'T
 *            touch ENABLE table
 */
void menu_main_oe()
{
    // Blink for fun
    led_blue = !led_blue;
    if (p_osc->format[0] == 'f') {
        float cycle = tosc_getNextFloat(p_osc);
        if (cycle > 0 && cycle <=1 ) {
            driver_A->oeCycle(cycle);
#if B_SIDE == 1
            driver_B->oeCycle(cycle);
#endif
        }
    }
    if (p_osc->format[1] == 'f') {
        float period = tosc_getNextFloat(p_osc);
        if (period > 0) {
            driver_A->oePeriod(period);
#if B_SIDE == 1
            driver_B->oePeriod(period);
#endif
        }
    }
}

/* OSC msg  : /main/tone f FREQ
 * Purpose  : Play a "note" in class-D style OUT through all DRV8844 and with OE setting
 * Note     : COMPLETELY BROKEN at this state and to be debbuged (with the new mbed-os)
 */
void menu_main_tone()
{
    // Blink for fun
    led_blue = !led_blue;
    if (p_osc->format[0] == 'f') {
        float tone = tosc_getNextFloat(p_osc);
        if (tone >0) {
            // Little sampler Period init
            driver_A->oePeriod(1.0/200000.0);
#if B_SIDE == 1
            driver_B->oePeriod(1.0/200000.0);
#endif
            sample_ticker.detach();
            sample_ticker.attach_us(&sampler_timer, (tone)*128);
        }
    }
}

// Menu parsers to function pointers.
void menu_main()
{
    for (menu_cases* p_case = main_cases;
            p_case != main_cases + sizeof(main_cases) / sizeof(main_cases[0]);
            p_case++) {
        if (strcmp(tosc_getAddress(p_osc), p_case->menu_string) == 0) {
            (*p_case->menu_func)();
        }
    }
}

void menu_tools()
{
    for (menu_cases* p_case = tools_cases;
            p_case != tools_cases + sizeof(tools_cases) / sizeof(tools_cases[0]);
            p_case++) {
        if (strcmp(tosc_getAddress(p_osc), p_case->menu_string) == 0) {
            (*p_case->menu_func)();
        }
    }
}

