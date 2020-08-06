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
void menu_lowlevel();
void menu_tools();

void menu_main_coil();
void menu_main_motor();
void menu_main_motor_brake();
void menu_main_motor_coast();
void menu_main_tone();
void menu_lowlevel_output();
void menu_lowlevel_output_all();
void menu_lowlevel_output_state();
void menu_lowlevel_pwm();
void menu_lowlevel_pwm_all();
void menu_lowlevel_pwm_state();
void menu_lowlevel_oe();
void menu_tools_connect();
void menu_tools_debug();
void menu_tools_hardreset();
void menu_tools_softreset();
void menu_tools_forceoff_all();
void menu_tools_count();

long int debug_count = 0;
int debug_smallcount = 0;

/* Menu structure with function pointers. Still OK with a modest menu but 
 * there is for sure better ideas for a better algorithmic complexity.
 */
struct menu_cases {
    const char* menu_string;
    void (*menu_func)(void);
};

menu_cases cases [] = {
    { "/" IF_OSC_NAME ,             menu_main       },
    { "/" IF_OSC_NAME "/ll",        menu_lowlevel   },
    { "/tools",                   menu_tools      }
};

menu_cases main_cases [] = {
    { "/" IF_OSC_NAME "/coil",        menu_main_coil        },
    { "/" IF_OSC_NAME "/motor",       menu_main_motor       },
    { "/" IF_OSC_NAME "/motor_brake", menu_main_motor_brake },
    { "/" IF_OSC_NAME "/motor_coast", menu_main_motor_coast },
    { "/" IF_OSC_NAME "/tone",        menu_main_tone        }
};

menu_cases lowlevel_cases [] = {
    { "/" IF_OSC_NAME "/ll/output",       menu_lowlevel_output       },
    { "/" IF_OSC_NAME "/ll/output_all",   menu_lowlevel_output_all   },
    { "/" IF_OSC_NAME "/ll/output_state", menu_lowlevel_output_state },
    { "/" IF_OSC_NAME "/ll/pwm",          menu_lowlevel_pwm          },
    { "/" IF_OSC_NAME "/ll/pwm_all",      menu_lowlevel_pwm_all      },
    { "/" IF_OSC_NAME "/ll/pwm_state",    menu_lowlevel_pwm_state    },
    { "/" IF_OSC_NAME "/ll/oe",           menu_lowlevel_oe           }
};

menu_cases tools_cases [] = {
    { "/tools/connect",      menu_tools_connect      },
    { "/tools/debug",        menu_tools_debug        },
    { "/tools/hardreset",    menu_tools_hardreset    },
    { "/tools/softreset",    menu_tools_softreset    },
    { "/tools/forceoff_all", menu_tools_forceoff_all },
    { "/tools/count",        menu_tools_count        }
};

/* -----------------------------------------------------------------------------
 * MENU OSC Parser : HERE WE ACT !
 */

/* OSC msg  : /main/coil ii PORT INTENSITY
 * Purpose  : drive coilOn/coilOff functions
 * Note     : For now, INTENSITY is almost useless : we just launch coilOff if == 0
 */
void menu_main_coil()
{
    if (p_osc->format[0] == 'i' && p_osc->format[1] == 'i') {
        int port = tosc_getNextInt32(p_osc);
        int intensity = tosc_getNextInt32(p_osc);
        if (port >= 0 && port < 24 ) {
            if (intensity == 0) {
                driver_A->coilOff(port);
                if (debug_on) {
                    char buf[64];
                    sprintf(buf, "COIL %i : %i use(s)", port, (int)driver_A->outRegister.reg_readUser(port));
                    debug_OSC(buf);
                }
            } else {
                driver_A->coilOn(port);
            }
#if B_SIDE == 1
        } else if (port >= 24 && port < 48 ) {
            if (intensity == 0) {
                driver_B->coilOff(port - 24);
                if (debug_on) {
                    char buf[64];
                    sprintf(buf, "COIL %i : %i use(s)", port, (int)driver_B->outRegister.reg_readUser(port - 24));
                    debug_OSC(buf);
                }
            } else {
                driver_B->coilOn(port - 24);
            }
#endif
        }
    }
}

/* OSC msg  : /main/motor iii PORT NEXT_PORT SPEED
 * Purpose  : drive motor function. SPEED set between -255 and +255
 * Note     : This is a PUSH/PULL configuration
 */
void menu_main_motor(){
    if (p_osc->format[0] == 'i' && p_osc->format[1] == 'i'
            && p_osc->format[2] == 'i') {
        int port      = tosc_getNextInt32(p_osc);
        int next_port = tosc_getNextInt32(p_osc);
        int speed     = tosc_getNextInt32(p_osc);
        if (speed >= -255 && speed <= 255 &&
                port >= 0 && port < 23 ) { //port is even
            int r = driver_A->motor(port, next_port, speed);
            if (r != 0)
                debug_OSC("/main/motor : wrong PINs configuration (see manual)");
#if B_SIDE == 1
        } else if (speed >= -255 && speed <= 255 &&
                port >= 24 && port < 47 ) {
            int r = driver_B->motor(port - 24, next_port - 24, speed);
            if (r != 0)
                debug_OSC("/main/motor : wrong PINs configuration (see manual)");
#endif
        }
    }
}

/* OSC msg  : /main/motor_brake ii PORT NEXT_PORT
 * Purpose  : drive motor_brake function (all enable and pwm sets to 1)
 * Note     : This is a PUSH/PULL configuration
 */
void menu_main_motor_brake(){
    if (p_osc->format[0] == 'i' && p_osc->format[1] == 'i') {
        int port      = tosc_getNextInt32(p_osc);
        int next_port = tosc_getNextInt32(p_osc);
        if (port >= 0 && port < 23 ) { // port is even
            int r = driver_A->motorBrake(port, next_port);
            if (r != 0)
                debug_OSC("/main/motor_brake : wrong PINs configuration (see manual)");
#if B_SIDE == 1
        } else if (port >= 24 && port < 47 ) {
            int r = driver_A->motorBrake(port - 24, next_port - 24);
            if (r != 0)
                debug_OSC("/main/motor_brake : wrong PINs configuration (see manual)");
#endif
        }
    }
}

/* OSC msg  : /main/motor_coast ii PORT NEXT_PORT
 * Purpose  : drive motor_coast function (all enable and pwm sets to 0)
 * Note     : This is a PUSH/PULL configuration
 */
void menu_main_motor_coast(){
    if (p_osc->format[0] == 'i' && p_osc->format[1] == 'i') {
        int port      = tosc_getNextInt32(p_osc);
        int next_port = tosc_getNextInt32(p_osc);
        if (port >= 0 && port < 23 ) { // port is even
            int r = driver_A->motorCoast(port, next_port);
            if (r != 0)
                debug_OSC("/main/motor_coast : wrong PINs configuration (see manual)");
#if B_SIDE == 1
        } else if (port >= 24 && port < 47 ) {
            int r = driver_A->motorCoast(port - 24, next_port - 24);
            if (r != 0)
                debug_OSC("/main/motor_coast : wrong PINs configuration (see manual)");
#endif
        }
    }
}

/* OSC msg  : /main/tone f FREQ
 * Purpose  : Play a "note" in class-D style through all DRV8844 OUT and with OE setting
 * Note     : Very experimental and ugly. But promising.
 */
void menu_main_tone()
{
    if (p_osc->format[0] == 'f') {
        float tone = tosc_getNextFloat(p_osc);
        if (tone >0) {
            // Little sampler Period init
            driver_A->oePeriod(1.0/200000.0);
            driver_A->drvEnable(ALLPORTS, 1);
#if B_SIDE == 1
            driver_B->oePeriod(1.0/200000.0);
            driver_B->drvEnable(ALLPORTS, 1);

#endif
            sample_ticker.detach();
            sample_ticker.attach_us(&sampler_timer, (tone)*128);
        }
    }
}

/* OSC msg  : /lowlevel/output ii PORT 0/1
 * Purpose  : set/unset OUTPUT with ENABLE pin on DRV8844 (see datasheet)
 */
void menu_lowlevel_output()
{
    if (p_osc->format[0] == 'i' && p_osc->format[1] == 'i') {
        int port  = tosc_getNextInt32(p_osc);
        int state = tosc_getNextInt32(p_osc);
        if (state >= 0 && state <= 1 && port >= 0 && port < 24 ) {
            if (debug_on) {
                char buf[64];
                sprintf(buf, "OUT %i %i", port, state);
                debug_OSC(buf);
            }
            driver_A->drvEnable(port, state);
#if B_SIDE == 1
        } else if (state >= 0 && state <= 1 && port >= 24 && port < 48 ) {
            if (debug_on) {
                char buf[64];
                sprintf(buf, "OUT %i %i", port, state);
                debug_OSC(buf);
            }
            driver_B->drvEnable(port - 24, state);
#endif
        }
    }
}

/* OSC msg  : /lowlevel/output_all i 0/1
 * Purpose  : set/unset ALL OUTPUTS with ENABLE pin on DRV8844 (see datasheet)
 */
void menu_lowlevel_output_all()
{
    if (p_osc->format[0] == 'i') {
        int state = tosc_getNextInt32(p_osc);
        if (state >= 0 && state <= 1) {
            if (debug_on) {
                char buf[64];
                sprintf(buf, "ALL OUT %i", state);
                debug_OSC(buf);
            }
            driver_A->drvEnable(ALLPORTS, state);
#if B_SIDE == 1
        } else if (state >= 0 && state <= 1) {
            if (debug_on) {
                char buf[64];
                sprintf(buf, "ALL OUT %i", state);
                debug_OSC(buf);
            }
            driver_B->drvEnable(ALLPORTS, state);
#endif
        }
    }
}

/* OSC msg  : /lowlevel/output_state i PORT
 * Purpose  : just return OUTPUT state
 */
void menu_lowlevel_output_state()
{
    if (p_osc->format[0] == 'i') {
        int port  = tosc_getNextInt32(p_osc);
        if (port >= 0 && port < 24 ) {
            char buf[64];
            sprintf(buf, "OUT %i %i", port, driver_A->drv_ena[port].read());
            debug_OSC(buf);
#if B_SIDE == 1
        } else if (port >= 24 && port < 48 ) {
            char buf[64];
            sprintf(buf, "OUT %i %i", port, driver_B->drv_ena[port - 24].read());
            debug_OSC(buf);
#endif
        }
    }
}

/* OSC msg  : /lowlevel/pwm ii PORT RATIO
 * Purpose  : control blinking of LEDS with RATIO between 0 and 255
 */
void menu_lowlevel_pwm()
{
    if (p_osc->format[0] == 'i' && p_osc->format[1] == 'i') {
        int port = tosc_getNextInt32(p_osc);
        int pwm  = tosc_getNextInt32(p_osc);
        if (pwm >= 0 && pwm <= 255 && port >= 0 && port < 24 ) {
            if (debug_on) {
                char buf[64];
                sprintf(buf, "OUT %i PWM %d", port, pwm);
                debug_OSC(buf);
            }
            driver_A->pwmSet(port, (uint8_t)pwm);
#if B_SIDE == 1
        } else if (pwm >= 0 && pwm <= 255 && port >= 24 && port < 48 ) {
            if (debug_on) {
                char buf[64];
                sprintf(buf, "OUT %i PWM %d", port, pwm);
                debug_OSC(buf);
            }
            driver_B->pwmSet(port - 24, (uint8_t)pwm);
#endif
        }
    }
}

/* OSC msg  : /lowlevel/pwm_all i RATIO
 * Purpose  : control blinking of all LEDS at the same time with RATIO between 0 and 255
 * Note     : Can be used in conjunction with OE
 */
void menu_lowlevel_pwm_all()
{
    if (p_osc->format[0] == 'i') {
        int pwm = tosc_getNextInt32(p_osc);
        if (pwm >= 0 && pwm <= 255) {
            if (debug_on) {
                char buf[64];
                sprintf(buf, "ALL PWM %2d", pwm);
                debug_OSC(buf);
            }
            driver_A->pwmSet(ALLPORTS, (uint8_t)pwm);
#if B_SIDE == 1
            driver_B->pwmSet(ALLPORTS, (uint8_t)pwm);
#endif
        }
    }
}

/* OSC msg  : /lowlevel/pwm_state i PORT
 * Purpose  : read the PWM state of PORT
 * Note     : Nothing for now.
 */
void menu_lowlevel_pwm_state(){}

/* OSC msg  : /lowlevel/oe ff CYCLE_RATIO PERIOD_SEC
 * Purpose  : set OE FastPWM config and control blinking of all LEDS at the same time
 * Note     : can be used in conjunction with other functions -- currently we DON'T
 *            touch ENABLE table
 */
void menu_lowlevel_oe()
{
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

/* OSC msg  : /tools/connect NONE (Bang)
 * Purpose  : connect NUCLEO_F767ZI to client (set IP address)
 */
void menu_tools_connect()
{
    // Copy IP from incoming packet
    strncpy(master_address, const_cast<char*>(client_addr->get_ip_address()), 16 * sizeof(char));

    // RE-assign socket with the new address
    // See if it's necessary ?
    delete client_addr;
    client_addr = new SocketAddress;

    debug_OSC("CONNECTION OK");
    if (debug_on)
        debug_OSC(master_address);
}

/* OSC msg  : /tools/debug NONE (Bang)
 * Purpose  : set debug ON to send many messages to client
 */
void menu_tools_debug()
{
    if (p_osc->format[0] == 'i') {
        int i = tosc_getNextInt32(p_osc);
        if (i == 0) {
            debug_on = 0;
            debug_OSC("DEBUG OFF");
        } else {
            debug_on = 1;
            debug_OSC("DEBUG ON");
        }
    }
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
    delete udp_socket;
    delete client_addr;
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
                              DRV_A_FAULT, driver_a_table, i2c_err_callback, A_SIDE_I2C_TAG);
#if B_SIDE == 1
    driver_B->forceoff(ALLPORTS);
    delete driver_B;
    driver_B = new CoilDriver(PCA_B_SDA, PCA_B_SCL, PCA_B_OE, DRV_B_RST,
                              DRV_B_FAULT, driver_b_table, i2c_err_callback, B_SIDE_I2C_TAG);
#endif
    led_green = led_blue = led_red = 0;
}

/* OSC msg  : /tools/forceoff_all NONE (Bang)
 * Purpose  : re-init OE and call forceoff ALLPORTS
 */
void menu_tools_forceoff_all()
{
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

/* OSC msg  : /tools/count i COUNT
 * Purpose  : Just ping pong from client for network reliability test
 */
void menu_tools_count()
{
    if (p_osc->format[0] == 'i') {
        int i = tosc_getNextInt32(p_osc);
		debug_smallcount = debug_smallcount + i;
        debug_count = debug_count + i;
    }
	if (debug_smallcount > 99) {
		char buffer[MAX_PQT_SENDLENGTH];
		sprintf(buffer, "%ld", debug_count);
		debug_OSC(buffer);
		debug_smallcount = 0;
	}
}

// Menu parsers to function pointers
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

void menu_lowlevel()
{
    for (menu_cases* p_case = lowlevel_cases;
            p_case != lowlevel_cases + sizeof(lowlevel_cases) / sizeof(lowlevel_cases[0]);
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

