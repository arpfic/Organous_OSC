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
#include "main_debug.h"
#include "menu.h"

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

/* Here we realy decode OSC messages -- and we parse addr to menu subfunctions
 */
void handler_Packetevent()
{
    if (debug_on == 1)
        debug_OSCmsg(mainpacket_buffer, mainpacket_length);

    tosc_message osc;
    p_osc = &osc;

    if (tosc_isBundle(mainpacket_buffer)) {
        // Blink for fun
        led_blue = !led_blue;

        tosc_bundle bundle;
        tosc_parseBundle(&bundle, mainpacket_buffer, mainpacket_length);

        while (tosc_getNextMessage(&bundle, &osc)) {
            for (menu_cases* p_case = cases;
                    p_case != cases + sizeof(cases) / sizeof(cases[0]);
                    p_case++) {
                if (strncmp(tosc_getAddress(&osc), p_case->menu_string,
                            strlen(p_case->menu_string)) == 0) {
                    (*p_case->menu_func)();
                }
            }
        }
    } else if (!tosc_parseMessage(&osc, mainpacket_buffer, mainpacket_length)) {
        // Blink for fun
        led_blue = !led_blue;

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
    char buffer[MAX_PQT_SENDLENGTH];

    sprintf(buffer, "UP ! My IP address is :");
    sprintf(buffer + strlen(buffer), " %s", const_cast<char*>(eth->get_ip_address()));
    sprintf(buffer + strlen(buffer), " PLEASE CONNECT !");

    debug_OSC(buffer);

    // Everything is OK
    led_green = !led_green;
}

/* Send UDP packet through UDPSocket. Call by debug*()
 */
static void send_UDPmsg(char* incoming_msg, int in_length)
{
    source_addr->set_ip_address(master_address);
    source_addr->set_port(OSC_CLIENT_PORT);
    my_socket->sendto(*(source_addr), incoming_msg, in_length);
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

// TONE experimental BROKEN function -- Interrupt routine
// used to output next analog sample whenever a timer interrupt occurs
void sampler_timer()
{
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
    debug_OSC("ERROR (temp/voltage/current) on card A. Please reset");
    led_red = 1;
}

void driver_B_error_handler()
{
    debug_OSC("ERROR (temp/voltage/current) on card B. Please reset");
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
                              DRV_A_FAULT, driver_a_table, i2c_err_callback, A_SIDE_I2C_TAG);
    // Set-up driver_A error feedbacks with PinDetect
    driver_A->drv_fault.attach_asserted_held(queue.event(driver_A_error_handler));
    driver_A->drv_fault.setSamplesTillHeld(20);
    driver_A->drv_fault.setAssertValue(0);
    driver_A->drv_fault.setSampleFrequency();
#if B_SIDE == 1
    driver_B = new CoilDriver(PCA_B_SDA, PCA_B_SCL, PCA_B_OE, DRV_B_RST,
                              DRV_B_FAULT, driver_b_table, i2c_err_callback, B_SIDE_I2C_TAG);
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
    //my_socket = new TCPSocket(eth);
    my_socket->set_blocking(false);
    my_socket->open(eth);
    // Set-up SocketAddress source_addr
    source_addr = new SocketAddress;
    // TCP/IP stuff. Bind UDPSocket to the OSC_CLIENT_PORT.
    while(my_socket->bind(OSC_CLIENT_PORT) != 0);

    // Callback ANY packet to handle_socket() -- here is the main magic function.
    my_socket->sigio(callback(handle_socket));
    // and... Dispatch forever the queue !
    //thrd.start(callback(&queue, &EventQueue::dispatch_forever));

    /* At this step we are "On the Air", so we can dend up a welcome message to
     * broadcast. We can communicate in both sides with BROADCAST address, but it's
     * important to connect each others with more intimity, because :
     * more intimity == more efficiency
     */
    init_msgON();

    // (not so) BROKEN tone function : precompute 128 sample points on one sine wave cycle
    // used for continuous sine wave output later
    for(int i=0; i<128; i++) {
        sinusoid_data[i]=((1.0 + sin((float(i)/128.0*6.28318530717959)))/2.0);
    }

    // Dispatch forever the queue, directly and not in a thread (bugs appends) NO !
    // Dispatch forever the queue in a thread :
    //thrd.start(callback(&queue, &EventQueue::dispatch_forever));

    while (true) {
//        led_red = !led_red;
		queue.dispatch_forever();
        // Not even sleep !
    }
}
