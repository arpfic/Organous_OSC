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
#include "main.h"

// Handler
static void handle_socket()
{
    queue.call(receive_message);
}

// Read data from the socket
static void receive_message()
{
    // Zeroing the buffer
    memset(mainpacket_buffer, 0, sizeof(mainpacket_buffer));
    bool something_in_socket = true;
    // Read all messages
    while (something_in_socket) {
        mainpacket_length = my_socket->recvfrom(source_addr,
                mainpacket_buffer, sizeof(mainpacket_buffer) - 1);
        if (mainpacket_length > 0) {
            handler_Packetevent();
        } else if (mainpacket_length != NSAPI_ERROR_WOULD_BLOCK) {
            // Error while receiving
            led_red = !led_red;
            something_in_socket = false;
        } else {
            // Or there was nothing to read.
            something_in_socket = false;
        }
    }
}

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
                              DRV_A_FAULT, driver_a_table, 0xD2);
#if B_SIDE == 1
    driver_B->forceoff(ALLPORTS);
    delete driver_B;
    driver_B = new CoilDriver(PCA_B_SDA, PCA_B_SCL, PCA_B_OE, DRV_B_RST,
                              DRV_B_FAULT, driver_b_table, 0x2A);
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

/* Here we realy decode OSC messages -- and we parse addr to menu subfunctions
 */
void handler_Packetevent()
{
    if (debug_on == 1)
        debug_OSCmsg(mainpacket_buffer, mainpacket_length);

    tosc_message osc;
    p_osc = &osc;

    if (!tosc_parseMessage(&osc, mainpacket_buffer, mainpacket_length)) {
        for (menu_cases* p_case = cases;
                p_case != cases + sizeof(cases) / sizeof(cases[0]);
                p_case++) {
            if (strncmp(tosc_getAddress(&osc), p_case->menu_string,
                        strlen(p_case->menu_string)) == 0) {
                (*p_case->menu_func)();
            }
        }
    } else {
        led_red = !led_red;
    }
}

/* Init NUCLEO_F767ZI message. See main.h
 */
void init_msgON()
{
    char buffer[MAX_PQT_LENGTH];

    sprintf(buffer, "UP ! My IP address is :");
    sprintf(buffer + strlen(buffer), " %s", const_cast<char*>(eth->get_ip_address()));
    sprintf(buffer + strlen(buffer), " PLEASE CONNECT !");

    debug_OSC(buffer);

    // Everything is OK
    led_green = !led_green;
}

/* Button basic functions : we just resend init_msgON() when pressed
 */
void button_pressed()
{
    led_red = 1;
    queue.call(init_msgON);
}

void button_released()
{
    led_red = 0;
}

// TONE experimental BROKED function -- Interrupt routine
// used to output next analog sample whenever a timer interrupt occurs
void sampler_timer()
{
    driver_A->drvEnable(ALLPORTS, 1);
    driver_B->drvEnable(ALLPORTS, 1);
    // send next analog sample out to D to A
    driver_A->oeCycle(sinusoid_data[k]);
#if B_SIDE == 1
    driver_B->oeCycle(sinusoid_data[k]);
#endif
    // increment pointer and wrap around back to 0 at 128
    k = (k+2) & 0x07F;
}

/* (Very) basic implementation of EthernetInterface Status callback : it just
 * reset the board when disconnected. TODO: let's DO it better.
 */
void eth_status_callback(nsapi_event_t status, intptr_t param)
{
    if (param != NSAPI_STATUS_GLOBAL_UP && param != NSAPI_STATUS_LOCAL_UP)
        NVIC_SystemReset(); // Just reset the main board and voilà !
}

/* Callbacks to DRV8844 error PINS PinDetect : means OVERCURRENT or OVERTEMP, etc.
 * ---> We send an OSC Message
 */
void driver_A_error_handler()
{
    debug_OSC("OVERCURRENT ERROR on card A. Please reset");
    led_red = 1;
}

void driver_B_error_handler()
{
    debug_OSC("OVERCURRENT ERROR on card B. Please reset");
    led_red = 1;
}

/* MAIN function
 */
int main()
{
    // Init master_address IP client to broadcast for convenience
    master_address = (char *)malloc(16 * sizeof(char));
    sprintf(master_address, "255.255.255.255");

    // Init homemade CoilDriver class
    driver_A = new CoilDriver(PCA_A_SDA, PCA_A_SCL, PCA_A_OE, DRV_A_RST,
                              DRV_A_FAULT, driver_a_table, 0xD2);
    // Set-up driver_A error feedbacks with PinDetect
    driver_A->drv_fault.attach_asserted_held(queue.event(driver_A_error_handler));
    driver_A->drv_fault.setSamplesTillHeld(20);
    driver_A->drv_fault.setAssertValue(0);
    driver_A->drv_fault.setSampleFrequency();
#if B_SIDE == 1
    driver_B = new CoilDriver(PCA_B_SDA, PCA_B_SCL, PCA_B_OE, DRV_B_RST,
                              DRV_B_FAULT, driver_b_table, 0x2A);
    driver_B->drv_fault.attach_asserted_held(queue.event(driver_B_error_handler));
    driver_B->drv_fault.setSamplesTillHeld(20);
    driver_B->drv_fault.setAssertValue(0);
    driver_B->drv_fault.setSampleFrequency();
#endif

    // Set-up button
    button.fall(&button_released);
    button.rise(&button_pressed);

    // Set-up EthernetInterface in DHCP mode
    eth = new EthernetInterface;
    nsapi_error_t status = eth->connect();
    // Attach to status callback
    eth->attach(&eth_status_callback);
    // Set-up UDPSocket my_socket because OSC IS in UDP. TODO : try and benchmark TCP ?
    my_socket = new UDPSocket(eth);
    my_socket->set_blocking(false);
    my_socket->open(eth);
    // Set-up SocketAddress source_addr
    source_addr = new SocketAddress;
    // TCP/IP stuff. Bind UDPSocket to the OSC_CLIENT_PORT.
    while(my_socket->bind(OSC_CLIENT_PORT) != 0);

    // Callback ANY packet to handle_socket() -- here is the main magic function.
    my_socket->sigio(callback(handle_socket));
    // and... Dispatch forever the queue !
    thrd.start(callback(&queue, &EventQueue::dispatch_forever));

    /* At this step we are "On the Air", so we can dend up a welcome message to
     * broadcast. We can communicate in both sides with BROADCAST address, but it's
     * important to connect each others with more intimity, because :
     * more intimity == more efficiency
     */
    init_msgON();

    // BROKEN tone function : precompute 128 sample points on one sine wave cycle
    // used for continuous sine wave output later
    for(int i=0; i<128; i++) {
        sinusoid_data[i]=((1.0 + sin((float(i)/128.0*6.28318530717959)))/2.0);
    }

    while (true) {
        // Not even sleep !
    }
}
