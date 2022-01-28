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

#include "EthernetInterface.h"
#include "mbed.h"
#include "nsapi_types.h"
#include "string.h"
//#include "stats_report.h"
#include "config.h"
#include "PinDetect.h"
#include "UDPSocket.h"
#include "TCPSocket.h"
#include "FastPWM.h"
#include "SoftPWM.h"
#include "platform/CircularBuffer.h"
#include "main_socket_buffer.h"
#include "main_driver_hal.h"
#include "PCA9956A.h"
#include "tOSC.h"
#include "MemoryPool.h"
#include "MIDIMessage.h"
#include <cstdint>

// UDPSocket/TCPSocket Callbacks
static void handle_udp_socket();
static void handle_tcp_socket();
// Same purpose, but called by handle_udp_socket() through a queue
static void receive_udp_message();
static void receive_tcp_message();

/* When the NUCLEO_F767ZI is started and connected, this function send a welcome
 * packet to broadcast (255.255.255.255) and ask the main user to connect to it
 * with OSC /tools/connect to save the user IP address.
 */
void init_msgON();
// Callbacks : init_msgON() when the button is pressed
void button_pressed();
void button_released();

// Callbacks to DRV8844 error PINS PinDetect.
void driver_A_error_handler();
void driver_B_error_handler();

// UP OSC messages to client(s)
static void send_UDPmsg(char*, int);

int main();

// Callback to /main/tone OSC BROKEN function.
void sampler_timer();

// Regrouping OSC task in a Thread
void osc_task();

// Callback for MIDI RX
void on_rx_interrupt();

// Regrouping MIDI task in a Thread
void midi_task();

/* -----------------------------------------------------------------------------
 * MAIN objects, enum etc.
 */

/* Table of DigitalOut ENABLE PINs of DRV8844 : theses are to be TRUE to enable
 * OUTPUTS (see http://www.ti.com/lit/ds/symlink/drv8844.pdf and config.h)
 */
DigitalOut driver_a_table[ENABLE_PINS] = {
    DRV_EN1,  DRV_EN2,  DRV_EN3,  DRV_EN4,  DRV_EN5,  DRV_EN6,  DRV_EN7,  DRV_EN8,
DRV_EN9,  DRV_EN10, DRV_EN11, DRV_EN12, DRV_EN13, DRV_EN14, DRV_EN15, DRV_EN16,
DRV_EN17, DRV_EN18, DRV_EN19, DRV_EN20, DRV_EN21, DRV_EN22, DRV_EN23, DRV_EN24
    };
#if B_SIDE == 1
DigitalOut driver_b_table[ENABLE_PINS] = {
    DRV_EN25, DRV_EN26, DRV_EN27, DRV_EN28, DRV_EN29, DRV_EN30, DRV_EN31, DRV_EN32,
DRV_EN33, DRV_EN34, DRV_EN35, DRV_EN36, DRV_EN37, DRV_EN38, DRV_EN39, DRV_EN40,
DRV_EN41, DRV_EN42, DRV_EN43, DRV_EN44, DRV_EN45, DRV_EN46, DRV_EN47, DRV_EN48
    };
#endif

// Basic I/O objects
DigitalOut  led_green(LED_GREEN);
DigitalOut  led_blue(LED_BLUE);
DigitalOut  led_red(LED_RED);
InterruptIn button(PC_13);// USER_BUTTON

// DEBUG Stats
#define SLEEP_TIME                  100 // (msec)
#define PRINT_AFTER_N_LOOPS         100
// SystemReport sys_state( SLEEP_TIME * PRINT_AFTER_N_LOOPS /* Loop delay time in ms */);

// Ethernet interface pointers
EthernetInterface   *eth;
UDPSocket           *udp_socket;
SocketAddress       *ip;
SocketAddress       *client_addr;

Thread oscTask;

/* NET I/O queue and thread : it's important to be always available to incoming
 * packets
 */
EventQueue queue_msg(QUEUE_MSG_EVENTS * EVENTS_EVENT_SIZE);
EventQueue queue_io(QUEUE_IO_EVENTS * EVENTS_EVENT_SIZE);
Thread thrd_msg;
Thread thrd_io;

// Same Thread but for (hopefully not so often) DRV8844 errors
Thread *thread_errA;
#if B_SIDE == 1
Thread *thread_errB;
#endif

// Pointer to OSC packet/message
tosc_message* p_osc;

// Small buffer and Circular buffer :
// Packet size limitation = MAX_PQT_LENGTH
char    mainpacket_buffer[MAX_PQT_LENGTH];
int     mainpacket_length = 0;
// Circular buffer between ethernet socket and I2C
CircularBuffer<socketpacket*, PACKETS_TABLE_SIZE> socketpacket_buf;

// Pointers to DRV8844 Driver class
CoilDriver* driver_A;
#if B_SIDE == 1
CoilDriver* driver_B;
#endif

// Main client IP address. TODO: support more that one client ?
char*   master_address;

// Is debug on or off by OSC /tools/debug ?
char    debug_on = 0;

// MIDI Serial & Thread and Ticker for working with different packet lengths
static RawSerial midi_din(MIDI_UART_TX, MIDI_UART_RX);
Thread midiTask;

/* Mailbox of MIDI Packets */
typedef struct {
    uint8_t    midiPacket[MIDI_MAX_MSG_LENGTH]; /* AD result of measured voltage */
    int        midiPacketlenght = 0;
} midiPacket_t;
Mail<midiPacket_t, MIDIMAIL_SIZE>  rx_midiPacket_box;
midiPacket_t                       *rx_midi_outbox;
int                                packetbox_full = 0;
int                                rx_midi_Statusbyte = 0;

// MIDI variables
volatile int            rx_idx = 0;
uint8_t                 rx_buffer[MIDI_MAX_MSG_LENGTH + 1];


// BROKEN : little sampler ticker/timer (beta)
volatile int k = 0;
float sinusoid_data[128];
Ticker sample_ticker;
float tone = 500.0;

#endif // _MAIN_H
